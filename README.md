# Parking Space Status Display Terminal

**Author:** AHMED MD SHAKIL  

## Project Overview
This project simulates a smart parking space monitoring terminal using an STM32 microcontroller. The system detects vehicle presence using simulated geomagnetic sensors (GPIO inputs), displays availability via LEDs, and uploads real-time data to a PC via USART serial communication. It also includes a Real-Time Clock (RTC) module to trigger alerts during peak traffic hours.

## Features
* **Real-time Monitoring:** Tracks 4 parking slots using digital inputs.
* **Visual Feedback:** LED indicators show the status of each slot (ON = Available, OFF = Occupied).
* **Data Logging:** Sends "Available Spaces" count to a PC serial terminal via USART2.
* **RTC Alerts:** Triggers a "Peak Hour" alarm at scheduled times (e.g., 18:00).

## Hardware Pinout
Based on the STM32F103C8T6 configuration:

| Component          | STM32 Pin | Function                        |
| :---               | :---      | :---                            |
| **Sensors**        | PA0 - PA3 | GPIO Input (High = Occupied)    |
| **LED Indicators** | PB0 - PB3 | GPIO Output (Slot Status)       |
| **Status LED**     | PC13      | GPIO Output (System Full Alert) |
| **USART TX**       | PA2       | Serial Data Transmit            |
| **USART RX**       | PA3       | Serial Data Receive             |

## How to Run
1. **Hardware Setup:** 
Connect LEDs and Buttons/Sensors according to the pinout above.
2. **Software:** 
   - Open the project in STM32CubeIDE or Keil.
   - Compile and flash the code to the STM32 board.
3. **Monitoring:**
   - Open a Serial Terminal (PuTTY/Tera Term).
   - Set Baud Rate to **9600**.
   - Observe the "Available Spaces" output and LED status changes.