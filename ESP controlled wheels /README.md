# rover

ESP32 Wi-Fi rover controlled with the RemoteXY mobile app.

## Features
- Proportional joystick driving
- Differential steering
- Adjustable maximum speed with RemoteXY slider
- PWM motor control through L293D

## Motor wiring

| Motor | ESP32 GPIO |
|---|---|
| Left motor (J1 side) | GPIO 25 + GPIO 33 |
| Right motor | GPIO 27 + GPIO 26 |

The right motor polarity is inverted in software so forward joystick movement makes both wheels move forward.

## Software
Install the RemoteXY Arduino library 4.1.8 or later, select the correct ESP32 board, then upload `Ganesh_Rover.ino`.

## Power
Use a suitable regulated supply for the ESP32 and an appropriate motor supply for the motors. A motor driver's maximum supported supply voltage does not mean motors rated for a lower voltage should be driven at that higher voltage.

## Current status
Manual proportional rover control is working. Future additions can include telemetry, battery monitoring, sensors, and autonomous driving.
