/*
 * Smart Greenhouse Temperature Monitoring System
 *
 * Author: Gouri Girish Nair
 *
 * Description:
 * Arduino firmware for an embedded greenhouse monitoring system.
 * The system measures ambient temperature using a BMP180 sensor,
 * displays real-time information on an OLED display, and automatically
 * actuates a servo-driven ventilation mechanism when a configurable
 * temperature threshold is exceeded.
 *
 * Features:
 * - Real-time temperature monitoring
 * - OLED user interface
 * - Configurable temperature thresholds
 * - Automatic servo control
 * - Audible buzzer alerts
 * - Serial debugging output
 */

#include <Servo.h> // servo library
#include "HCBMP180.h"// bmp180 sensor library
#include <Wire.h>// wire library for i2c communication
#include <Adafruit_GFX.h> // adafruit library
#include <Adafruit_SSD1306.h>// oled display library

#define SCREEN_WIDTH      128// oled screen width
#define SCREEN_HEIGHT     64  // oled screen height
#define OLED_RESET        -1 //oled reset pin

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  //initialize oled display

#define BUTTON_NEXT_PIN   2  //button s2 for navigating menu
#define BUTTON_SELECT_PIN 3  //button s3 for selecting options
#define BUZZER_PIN        10 //buzzer pin
#define SERVO_PIN         11 //servo motor pin
#define POTENTIOMETER_PIN A0 //potentiometer pin to adjust threshold

//temp thresholds in tenths of a degree
#define TEMP_THRESHOLD_1  250//25C
#define TEMP_THRESHOLD_2  260 //26C
#define TEMP_THRESHOLD_3  270  //27C

//initialize servo and sensor
Servo myservo;   //create a servo object
HCBMP180 sensor(I2CBMP180ADD);  //reate a bmp180 sensor object

//stating variables
int currentThresholdIndex = 0;  //index of the selected temps
int thresholds[] = {TEMP_THRESHOLD_1, TEMP_THRESHOLD_2, TEMP_THRESHOLD_3};  //thresholds
bool buttonNextPressed = false; //flag to track button s2
bool buttonSelectPressed = false;  //flag to track button s3
bool buzzerOn = false;  //flag to track buzzer state
int lastServoPos = 0;   //last position of servo
int currentPage = 0;  //0 = menu page, 1 = monitoring page

void setup() {

    Serial.begin(9600);  //initialize serial communication
    sensor.Init();  //initialize sensor

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("SSD1306 initialization failed"));
        while (true);
    }

    display.clearDisplay();  //clear the oled
    display.display();  //update the oled

    pinMode(BUZZER_PIN, OUTPUT); //set buzzer as output
    digitalWrite(BUZZER_PIN, LOW); //turn off buzzer
    pinMode(BUTTON_NEXT_PIN, INPUT_PULLUP); //set button s2 as input with pull-up resistor
    pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP); //set button s3 as input with pull-up resistor
    pinMode(POTENTIOMETER_PIN, INPUT); //set potentiometer pin as input

    myservo.attach(SERVO_PIN); //attach servo to pin
    myservo.write(0);  //set servo to 0 degrees

    digitalWrite(4, HIGH); //turn on indicator led
    delay(100);   //wait for 100 ms
    digitalWrite(4, LOW);  //turn off indicator led
}

void loop() {

    handleButtonInput();  //check button
    (currentPage == 0) ? displayMenu() : displayMonitoring(); //display menu
}

void handleButtonInput() {

    bool buttonNextState = digitalRead(BUTTON_NEXT_PIN) == LOW; //check if s2 is pressed
    bool buttonSelectState = digitalRead(BUTTON_SELECT_PIN) == LOW; //check if s3 is pressed

    //s2 to go to menu
    if (buttonNextState && !buttonNextPressed) {
        buttonNextPressed = true;
        currentPage = 0;  //go to menu
        delay(150);
    } else if (!buttonNextState) {
        buttonNextPressed = false;
    }

    //button s3 to select the threshold
    if (buttonSelectState && !buttonSelectPressed) {
        buttonSelectPressed = true;
        if (currentPage == 0) { //check if menu page is on
            Serial.print("Threshold Selected: ");  //print selected threshold
            Serial.println(thresholds[currentThresholdIndex] / 10);
            currentPage = 1;  //go back to temp page
        }
        delay(150);
    } else if (!buttonSelectState) {
        buttonSelectPressed = false; //reset flag
    }

    //potentiometer to toggle between the thresholds based on potentiometer value
    if (currentPage == 0) {
        currentThresholdIndex = map(analogRead(POTENTIOMETER_PIN), 0, 1023, 0, 2); //adjust threshold
    }
}

void displayMenu() {

    display.clearDisplay();  //clear display
    display.setTextSize(1); //text size
    display.setTextColor(SSD1306_WHITE); //text color
    display.setCursor(0, 0);//cursor position
    display.print("Select Temperature Threshold:"); //print menu

    //show all available thresholds
    for (int i = 0; i < 3; i++) {
        display.setCursor(0, 20 + (i * 10));//set cursor for each line
        display.print(i + 1);
        display.print(") ");
        display.print(thresholds[i] / 10);
        display.print("C ");
        if (currentThresholdIndex == i) display.print("<- Selected");//show selected threshold
    }

    display.setCursor(0, 55); //set cursor position
    display.print("Press S3 to confirm");//print s3
    display.display();  //update display
}

void displayMonitoring() {

    int temperature = sensor.GetTemp(); //get temperature from the sensor
    Serial.print("Current Temp: ");//print the current temperature
    Serial.println(temperature / 10.0);//print the temperature value

    display.clearDisplay();  //clear the display
    display.setTextSize(1);  //set text size for display
    display.setTextColor(SSD1306_WHITE);  //set text color to white
    display.setCursor(0, 0);  //set cursor to the top-left corner
    display.print("Gouri's Greenhouse");  //display greenhouse name

    display.setCursor(0, 15);  //move cursor to position for threshold display
    display.print("Threshold: "); //display threshold label
    display.print(thresholds[currentThresholdIndex] / 10);  //display threshold value
    display.print(" C");   //display "C" for Celsius

    display.setCursor(0, 30);  //move cursor to position for current temperature display
    display.print("Current Temp: ");//display current temp label
    display.print(temperature / 10.0); //display the current temperature
    display.print(" C"); //display "C" for Celsius

    if (temperature > thresholds[currentThresholdIndex]) {  //check if temperature exceeds threshold
        display.setCursor(0, 45);//move cursor to display high temp message
        display.print("HIGH TEMP"); //display high temp message
        activateCooling(); //activate cooling if temp is high
    } else {
        deactivateCooling(); //deactivate cooling if temp is within threshold
    }

    display.setCursor(0, 55); //move cursor to position for menu prompt
    display.print("Press S2 for Menu");//prompt user to press S2 for menu
    display.display();  //update the display
}

void activateCooling() {

    if (lastServoPos != 90) {  //check if servo is not at 90 degrees
        myservo.write(90); //set servo to 90 degrees
        lastServoPos = 90;  //update last servo position
    }

    if (!buzzerOn) {//check if buzzer is off
        shortBeepSequence();//activate short beep sequence
        buzzerOn = true;//set buzzer to on
    }
}

void deactivateCooling() {

    if (lastServoPos != 0) {//check if servo is not at 0 degrees
        myservo.write(0); //set servo to 0 degrees
        lastServoPos = 0; //update last servo position
    }

    if (buzzerOn) {//check if buzzer is on
        noTone(BUZZER_PIN);//turn off the buzzer
        buzzerOn = false;//set buzzer to off
    }
}

void shortBeepSequence() {

    for (int i = 0; i < 2; i++) {//repeat the beep sequence twice
        tone(BUZZER_PIN, 1000);//emit a tone at 1000 Hz
        delay(200);//wait for 200 milliseconds
        noTone(BUZZER_PIN);//stop the tone
        delay(200);//wait for 200 milliseconds
    }
}