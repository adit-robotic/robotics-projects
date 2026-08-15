# 🇮🇳 India 15 August Radar

Arduino Nano mini-radar project using a 1.8" 128×160 ST7735 TFT, one servo, and an HC-SR04 ultrasonic sensor.

## Features

- Indian flag startup screen
- Circular loading animation for 8 seconds
- `RADAR / ONLINE` boot screen
- TFT rotated 180°
- Clean radar UI
- 40 cm maximum detection range
- 10 / 20 / 30 / 40 cm radar rings
- Red target marker for valid objects
- `DIST:---` with no red target when nothing is detected
- Faster servo sweep
- Radar UI avoids full-screen clearing during scanning

## Hardware

- Arduino Nano
- 1.8" 128×160 ST7735 TFT
- 1× servo motor
- HC-SR04 ultrasonic sensor

## Wiring

### TFT

| TFT | Nano |
|---|---|
| CS | D10 |
| DC / A0 | D9 |
| RST | D8 |
| SDA / MOSI | D11 |
| SCK / CLK | D13 |
| VCC | 5V |
| GND | GND |
| LED | 5V |

### Servo

| Servo | Nano |
|---|---|
| Signal | D6 |
| GND | GND |
| VCC | Suitable 5V supply |

Use a separate suitable 5V supply for the servo if needed, and connect its GND to Nano GND.

### HC-SR04

| HC-SR04 | Nano |
|---|---|
| VCC | 5V |
| GND | GND |
| TRIG | D4 |
| ECHO | D5 |

## Libraries

Install from Arduino IDE Library Manager:

- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- Servo

SPI is included with the Arduino environment.

## Project layout

```text
india-radar/
├── README.md
└── src/
    └── india_radar.ino
```

## Upload

Open `src/india_radar.ino` in Arduino IDE.

Select:

- Board: Arduino Nano
- Correct processor option for your Nano
- Correct serial port

Then compile and upload.

## Sweep speed

Current settings:

```cpp
#define SERVO_STEP 6
#define SERVO_DELAY 3
```

Increase `SERVO_STEP` for fewer commanded positions. Decrease `SERVO_DELAY` for a more aggressive sweep. Actual servo speed also depends on the servo and its power supply.

## Detection behavior

The ultrasonic sensor returns a valid target only for approximately 1–40 cm.

- 1–40 cm → red target
- No echo → `DIST:---`
- More than 40 cm → `DIST:---`
- No valid reading → no red target

## Credits / Project

Credits @AditsRoboticsHub . A fun 15 August Independence Day electronics project combining an Indian-themed startup sequence with a physical radar scanner and TFT visualization.

🇮🇳 📡
