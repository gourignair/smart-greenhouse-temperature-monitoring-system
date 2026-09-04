# 🌱 Smart Greenhouse Temperature Monitoring System

![Prototype](images/prototype_overview.jpg)

## Overview
This project is an embedded greenhouse monitoring system that monitors temperature in real time and automatically actuates a ventilation mechanism when a user-defined threshold is exceeded.

The project combines embedded software, PCB design, CAD modelling, electronics, and mechanical design into a complete engineering prototype.


## Features
- Real-time temperature monitoring using a BMP180 sensor
- OLED display with live temperature readings
- Interactive menu system
- User-configurable temperature thresholds
- Automatic servo-controlled window mechanism
- Audible buzzer alerts
- Serial debugging interface




## Hardware
- BMP180 Temperature Sensor
- OLED Display
- SG90 Servo Motor
- Potentiometer
- Push Buttons
- Buzzer
- Custom PCB



## PCB Design
![PCB Layout](images/pcb_layout.png)

![PCB Schematic](images/pcb_schematic.png)

The PCB was designed in Autodesk Eagle. The design integrates the sensor, OLED display, user controls, servo interface, and supporting circuitry while remaining within the project constraints.



## Mechanical Design
![CAD Assembly](images/cad_assembly.png)

Mechanical components were designed in Autodesk Fusion 360.

The assembly includes:

- Servo mount
- Roof mount
- Integrated enclosure
- Window actuation mechanism



## User Interface

### Monitoring Screen
![OLED](images/oled_monitoring_screen.jpg)

Displays the current temperature and selected threshold.

### Menu
![Menu](images/oled_menu.jpg)

Allows users to configure temperature thresholds using push buttons.



## Firmware
The firmware is located in the `firmware/` directory and implements:

- Sensor acquisition
- OLED interface
- User menu
- Servo control
- Buzzer alerts
- Serial debugging



## Project Structure
```text
firmware/
pcb/
cad/
images/
docs/
```



## Future Improvements
- Wi-Fi connectivity
- Cloud dashboard
- Humidity monitoring
- Data logging
- Mobile application
- Predictive climate control using machine learning



## Author
Gouri Girish Nair
Electronic & Computer Engineering
University of Kent
