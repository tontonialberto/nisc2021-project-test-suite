# Test Suite Project - Neapolis Innovation Summer Campus 2021
This project has been developed as the final project for NISC2021, a summer campus hosted by STMicroelectronics in Naples (IT) whose aim is to provide knowledge on 32-bit microcontroller programming using ChibiOS and ChibiStudio.

![Project showcase](/assets/grid.jpg)

## Authors
* Alberto Tontoni
* Angelo Barletta
* Daiana Cipollaro
* Claudio Spasiano

## Target
The demo runs on an STM32 Nucleo64-F401RE board.

## Features
Once connected to your pc, the Nucleo Board hosts a shell interface from serial port
whose commands execution manipulates both input and output hardware devices,
according on the commands' parameters inserted by the user.

## How to use
Connect Nucleo Board to a node via USB serial port, baud rate is 38400.
Once connected, digit "help" on the shell and press enter.
If everything is fine, Nucleo will show you the supported commands.

## Supported commands

- **LED [RED|GREEN|BLUE] [STATIC] [ON|OFF]**: turns on/off the selected rgb color
- **LED [RED|GREEN|BLUE] [DYNAMIC] [100-1000]**: blink selected led according to period (in ms)
- **JOY [XY|POLAR]**: joystick coordinates are shown on OLED display, either in cartesian (x,y) or polar (radius, angle) mode
- **OLED [LED|JOY]**: either rgb led state or joystick cartesian coordinates are shown on OLED display
- **BUZZ ON [PERIOD]**: buzz on and off buzzer periodically (period is expressed in seconds)
- **BUZZ OFF**: turn off buzzer
- **DIMMER ON [RED|GREEN|BLUE]**: once activated, the selected led intensity can be set using the rotary encoder.
- **DEMO**: once activated, rgb components are shown on OLED display, and rgb led will simulate a color wheel based on joystick input

## Software required
* ChibiStudio

## Hardware required
* STM32F401RE Nucleo Board
* RGB LED
* Buzzer
* SSD1306 OLED Display
* Rotary Encoder
* 2-Axis Joystick

## Connection Scheme
| Device     | Nucleo Board |
|------------|--------------|
| RGB Red    | PA8          |
| RGB Green  | PA9          |
| RGB Blue   | PA10         |
| Buzzer     | PA5          |
| OLED SDA   | PB9          |
| OLED SCL   | PB8          |
| Rotary CLK | PA1          |
| Rotary DT  | PA0          |
| Joystick X | PC0          |
| Joystick Y | PC1          |

## Code architecture

* The system state machine has been shaped using global state variables.
These variables are set inside the shell module and read from system threads.
* Also, when possible, hardware devices are represented by simple data structures, in order to reduce concurrent accesses to the hardware.
* For instance, rgb led state is represented by a data structure containing three percentages. 
* In practice, generally only one thread is responsible to access the physical resource according to corresponding data structure,
whereas the other threads can only set/get that variable.
* For example, there is a specialized thread whose only aim is to write the OLED display according to which command
has to be executed.
