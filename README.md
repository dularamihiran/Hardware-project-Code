# NovaNex - Automated Retail Management System

## Project Overview
NovaNex is a hardware project designed to automate retail processes, particularly for grain dispensing, weight measurement, temperature control, and stock management. This system integrates various hardware components such as load cells, servo motors, stepper motors, temperature sensors, LEDs, and GSM modules to ensure efficient operation and real-time alerts.

## Features
- Automated Grain Dispensing: Dispenses grains based on the user input and calculates the price.
- Weight Monitoring: Uses a load cell to measure the weight of dispensed items with real-time updates on an LCD.
- Temperature Control: Manages two DS18B20 temperature sensors to monitor and control heating elements via relays.
- SMS Alerts: Sends alerts via GSM when stock is below 2000 grams.
- Motor and Relay Control: Stepper motors and relays are used to handle the dispensing, sealing, and grain packaging processes.
- User Interface: LCD display and keypad for user input and real-time feedback.
  
## Components Used
- Arduino Mega 2560
- Load Cell (HX711)
- 16x2 I2C LCD Display
- 4x4 Keypad
- Servo Motor
- Stepper Motors
- LEDs
- GSM Module
- DS18B20 Temperature Sensors
- Relays

## Pin Connections
- Load Cell: DOUT to pin 28, SCK to pin 30
- LCD: I2C address 0x27
- Keypad: Row pins - 23, 25, 27, 29; Column pins - 31, 33, 35, 37
- Servo Motor: Pin 9
- LEDs: LED1 to pin 45, LED2 to pin 47
- Temperature Sensors: OneWire buses - pins 34 and 40
- Relays: Relay pins - 42, 44, 48, 53, 51, 49
- Stepper Motors: 
  - Motor 1: Pulse pin 5, Direction pin 6, Enable pin 7
  - Motor 2: Pulse pin 11, Direction pin 12, Enable pin 13
  - Motor 3: Pulse pin 2, Direction pin 3, Enable pin 4
- GSM Module: RX to pin 10, TX to pin 8

## How It Works
1. Grain Dispensing: Users input the amount of grains they want to purchase using the keypad. The system dispenses the requested weight and calculates the price based on a fixed rate per gram.
2. Weight Monitoring: The current weight of the grains is displayed on the LCD in real time.
3. Temperature Control: Two temperature sensors monitor the environment, and if temperatures exceed 120°C, the system turns off the heating elements via relays.
4. Stock Alerts: If the weight of available grains falls below 2000 grams, the system automatically sends a low-stock SMS alert to a predefined phone number.
5. Sealing Process: After dispensing, motors and relays control the grain sealing process.

## How to Run the Project
1. Clone this repository to your local machine.
2. Connect the hardware components as per the pin configuration mentioned above.
3. Upload the code to your Arduino Mega 2560 using the Arduino IDE.
4. Power on the system and use the keypad to input the grain weight for dispensing.
5. Monitor the process on the LCD screen and via SMS alerts.

## Key Functions
- `getWeight()`: Returns the current weight measured by the load cell.
- `handleGrainDispensing()`: Handles the grain dispensing process based on user input.
- `updateLEDs()`: Controls LEDs and sends SMS alerts based on stock levels.
- `runSealingProcess()`: Manages the sealing process using stepper motors and relays.
- `updateTemperatureControl()`: Monitors and controls temperature based on sensor data.

- ## Authors
- [Dulara Mihiran] 
- [Rumesh thisaranga] 
- [Kaveesha prabhuddi] 
- [Dasuni Nagodavithana] 
- [Piyumi Pathinayake] 

## Acknowledgements
Special thanks to our university faculty and team members for their support in completing this project.
