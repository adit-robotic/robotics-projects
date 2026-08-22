#include <Arduino.h>
#define REMOTEXY_MODE__WIFI_POINT
#include <WiFi.h>

#define REMOTEXY_WIFI_SSID "esp32"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377
#define REMOTEXY_ACCESS_PASSWORD "12345678"

#include <RemoteXY.h>

#pragma pack(push, 1)
uint8_t const PROGMEM RemoteXY_CONF_PROGMEM[] = {
  255,3,0,0,0,63,0,19,0,0,0,101,115,112,51,50,0,31,2,106,
  200,200,84,1,1,3,0,5,200,0,143,143,123,9,60,60,32,2,26,31,
  4,12,45,7,86,23,4,15,70,0,2,26,129,246,95,71,29,39,33,36,
  12,64,36,115,112,101,100,32,0
};

struct {
  int8_t joystick_01_x;
  int8_t joystick_01_y;
  int8_t slider_01;
  uint8_t connect_flag;
} RemoteXY;
#pragma pack(pop)

#define LEFT_IN1 25
#define LEFT_IN2 33
#define RIGHT_IN1 27
#define RIGHT_IN2 26

#define PWM_FREQ 20000
#define PWM_RES 8

void motorLeft(int speed) {
  speed = constrain(speed, -255, 255);
  if (speed > 0) {
    ledcWrite(LEFT_IN1, speed);
    ledcWrite(LEFT_IN2, 0);
  } else if (speed < 0) {
    ledcWrite(LEFT_IN1, 0);
    ledcWrite(LEFT_IN2, -speed);
  } else {
    ledcWrite(LEFT_IN1, 0);
    ledcWrite(LEFT_IN2, 0);
  }
}

void motorRight(int speed) {
  speed = constrain(speed, -255, 255);
  // Right motor polarity is reversed relative to the left motor.
  if (speed > 0) {
    ledcWrite(RIGHT_IN1, 0);
    ledcWrite(RIGHT_IN2, speed);
  } else if (speed < 0) {
    ledcWrite(RIGHT_IN1, -speed);
    ledcWrite(RIGHT_IN2, 0);
  } else {
    ledcWrite(RIGHT_IN1, 0);
    ledcWrite(RIGHT_IN2, 0);
  }
}

void stopMotors() {
  motorLeft(0);
  motorRight(0);
}

void setup() {
  RemoteXY_Init();

  pinMode(LEFT_IN1, OUTPUT);
  pinMode(LEFT_IN2, OUTPUT);
  pinMode(RIGHT_IN1, OUTPUT);
  pinMode(RIGHT_IN2, OUTPUT);

  ledcAttach(LEFT_IN1, PWM_FREQ, PWM_RES);
  ledcAttach(LEFT_IN2, PWM_FREQ, PWM_RES);
  ledcAttach(RIGHT_IN1, PWM_FREQ, PWM_RES);
  ledcAttach(RIGHT_IN2, PWM_FREQ, PWM_RES);

  stopMotors();
}

void loop() {
  RemoteXYEngine.handler();

  int x = RemoteXY.joystick_01_x;
  int y = RemoteXY.joystick_01_y;

  if (abs(x) < 10) x = 0;
  if (abs(y) < 10) y = 0;

  int left = constrain(y + x, -100, 100);
  int right = constrain(y - x, -100, 100);

  int maxPWM = map(RemoteXY.slider_01, 0, 100, 0, 255);

  int leftPWM = (abs(left) * maxPWM) / 100;
  int rightPWM = (abs(right) * maxPWM) / 100;

  if (left < 0) leftPWM = -leftPWM;
  if (right < 0) rightPWM = -rightPWM;

  if (left == 0 && right == 0) {
    stopMotors();
  } else {
    motorLeft(leftPWM);
    motorRight(rightPWM);
  }
}
