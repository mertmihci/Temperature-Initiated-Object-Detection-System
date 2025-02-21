# Temperature-Initiated Object Detection System

## Overview
This project is part of the **Introduction to Microprocessors and Microcontrollers** course and focuses on developing a **Temperature-Initiated Object Detection System** using the **TM4C123G** microcontroller. The system integrates temperature sensing, deep sleep mode, object detection, and user interface elements to achieve an intelligent and efficient monitoring solution.

## Features
- **Deep Sleep Mode**: Conserves power while monitoring ambient temperature using the **LM35 analog sensor**.
- **Temperature Verification**: Uses the **BMP280 digital sensor (I2C)** for precise measurement after waking up.
- **Auditory Alert**: Activates a **speaker** for 2 seconds if the threshold is exceeded.
- **Object Detection**: Scans a **-90° to 90° range** using an **HC-SR04 ultrasonic sensor** mounted on a **stepper motor**.
- **User Interface**:
  - **Nokia 5110 LCD** (SPI) displays real-time data.
  - **RGB LEDs** indicate object proximity.
  - **Keypad & Trimpot** allow user input.

## Hardware Components
- TM4C123G Microcontroller
- LM35 Analog Temperature Sensor
- BMP280 Digital Temperature Sensor
- HC-SR04 Ultrasonic Sensor
- Stepper Motor
- Nokia 5110 LCD Display
- Speaker
- RGB LEDs
- 4x4 Keypad
- Trimpot
- Push Buttons

## Software Implementation
- Developed in **C and ARM Assembly**
- Uses **I2C, SPI, GPIO, and PWM** for interfacing
- Interrupt-driven deep sleep wake-up mechanism

## Setup & Usage
1. Connect the required hardware components as per the circuit design.
2. Compile and upload the **Keil µVision** project to the TM4C123G.
3. Adjust the temperature threshold via the **trimpot** or **keypad**.
4. Observe real-time temperature and object detection on the **LCD**.
5. Reset the system using the designated push button.

## Contributors
- Mustafa Mert Mıhçı
- Ahmet Uğur Akdemir


