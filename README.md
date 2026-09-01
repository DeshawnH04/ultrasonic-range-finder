# Ultrasonic Range Finder

Embedded C project using a TM4C LaunchPad and HC-SR04 ultrasonic sensor to measure distance.

## Features
- Measures distance in centimeters
- Displays distance through UART
- Uses onboard LEDs to indicate distance ranges
- Uses GPIO, SysTick, interrupts, timers, and PLL

## Hardware
- TM4C LaunchPad
- HC-SR04 ultrasonic sensor
- Voltage divider resistors

## How It Works
Pressing the onboard switch triggers a distance measurement. The ultrasonic sensor sends an echo pulse, and the microcontroller measures the pulse width to calculate distance.

The LEDs indicate the measured range:
- Red LED blinks when distance is less than 10 cm
- Green LED turns on between 10 cm and 70 cm
- Blue LED turns on above 70 cm

Distance is also displayed on a PC serial terminal using UART.

## Course
CECS 347 - Embedded Systems
