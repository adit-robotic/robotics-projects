#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <Servo.h>
#include <math.h>

#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 8
Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

#define SERVO_PIN 6
Servo radarServo;

#define TRIG_PIN 4
#define ECHO_PIN 5

#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREEN 0x07E0
#define DARKGREEN 0x0320
#define SAFFRON 0xFD20
#define NAVY 0x001F
#define GREY 0x7BEF
#define RED 0xF800

#define MAX_DISTANCE 40
#define RADAR_CX 64
#define RADAR_CY 121
#define RADAR_R 53

// Faster sweep
#define SERVO_STEP 6
#define SERVO_DELAY 3

int lastSweepAngle = -1;

void drawIndianFlag() {
  int x = 24, y = 12, w = 80;
  tft.fillRect(x, y, w, 18, SAFFRON);
  tft.fillRect(x, y + 18, w, 18, WHITE);
  tft.fillRect(x, y + 36, w, 18, GREEN);

  int cx = 64, cy = y + 27;
  tft.drawCircle(cx, cy, 9, NAVY);
  tft.fillCircle(cx, cy, 2, NAVY);

  for (int i = 0; i < 24; i++) {
    float a = i * 15.0 * 3.14159 / 180.0;
    tft.drawLine(cx + cos(a) * 2, cy + sin(a) * 2,
                 cx + cos(a) * 8, cy + sin(a) * 8, NAVY);
  }

  tft.drawLine(x - 4, y, x - 4, y + 64, WHITE);
  tft.fillCircle(x - 4, y - 2, 2, WHITE);
}

void drawLoader(int frame) {
  int cx = 64, cy = 108, radius = 11;
  tft.fillRect(45, 88, 38, 40, BLACK);

  for (int i = 0; i < 8; i++) {
    float a = i * 45.0 * 3.14159 / 180.0;
    int x = cx + cos(a) * radius;
    int y = cy + sin(a) * radius;
    tft.fillCircle(x, y, (i == frame) ? 3 : 2,
                   (i == frame) ? WHITE : GREY);
  }
}

// Returns 1..40 cm, or -1 for no valid target.
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 5000);
  if (duration == 0) return -1;

  int distance = duration * 0.0343 / 2;
  if (distance <= 0 || distance > MAX_DISTANCE) return -1;
  return distance;
}

void drawRadarArc(int radius, uint16_t color) {
  for (int angle = 0; angle <= 180; angle++) {
    float a = angle * 3.14159 / 180.0;
    tft.drawPixel(RADAR_CX + cos(a) * radius,
                  RADAR_CY - sin(a) * radius, color);
  }
}

void drawRadarGrid() {
  drawRadarArc(RADAR_R / 4, DARKGREEN);
  drawRadarArc(RADAR_R / 2, DARKGREEN);
  drawRadarArc(RADAR_R * 3 / 4, DARKGREEN);
  drawRadarArc(RADAR_R, GREEN);

  tft.drawLine(RADAR_CX - RADAR_R, RADAR_CY,
               RADAR_CX + RADAR_R, RADAR_CY, GREEN);

  int angles[] = {30, 60, 90, 120, 150};
  for (int i = 0; i < 5; i++) {
    float a = angles[i] * 3.14159 / 180.0;
    tft.drawLine(RADAR_CX, RADAR_CY,
                 RADAR_CX + cos(a) * RADAR_R,
                 RADAR_CY - sin(a) * RADAR_R, DARKGREEN);
  }

  tft.fillCircle(RADAR_CX, RADAR_CY, 2, GREEN);
}

void drawRadarHeader() {
  tft.fillRect(0, 0, 128, 20, BLACK);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(5, 4);
  tft.print("INDIA RADAR");

  tft.setTextColor(GREEN);
  tft.setCursor(99, 4);
  tft.print("LIVE");

  tft.drawFastHLine(4, 15, 120, GREY);
  tft.drawFastHLine(40, 18, 16, SAFFRON);
  tft.drawFastHLine(56, 18, 16, WHITE);
  tft.drawFastHLine(72, 18, 16, GREEN);
}

void drawRangeLabels() {
  tft.setTextSize(1);
  tft.setTextColor(GREY);

  tft.setCursor(RADAR_CX + 5, RADAR_CY - RADAR_R / 4 - 3);
  tft.print("10");
  tft.setCursor(RADAR_CX + 5, RADAR_CY - RADAR_R / 2 - 3);
  tft.print("20");
  tft.setCursor(RADAR_CX + 5, RADAR_CY - RADAR_R * 3 / 4 - 3);
  tft.print("30");
  tft.setCursor(RADAR_CX + 5, RADAR_CY - RADAR_R - 3);
  tft.print("40");
}

void drawRadarInfo(int angle, int distance) {
  tft.fillRect(0, 137, 128, 23, BLACK);
  tft.drawFastHLine(4, 136, 120, GREY);

  tft.setTextSize(1);
  tft.setTextColor(WHITE);

  tft.setCursor(5, 143);
  tft.print("ANG:");
  if (angle < 100) tft.print("0");
  if (angle < 10) tft.print("0");
  tft.print(angle);

  tft.setCursor(67, 143);
  tft.print("DIST:");

  if (distance < 1 || distance > MAX_DISTANCE) {
    tft.print("---");
  } else {
    tft.print(distance);
    tft.print("cm");
  }
}

void drawSweepLine(int angle, uint16_t color) {
  float a = angle * 3.14159 / 180.0;
  tft.drawLine(RADAR_CX, RADAR_CY,
               RADAR_CX + cos(a) * RADAR_R,
               RADAR_CY - sin(a) * RADAR_R, color);
}

void drawTarget(int angle, int distance) {
  if (distance < 1 || distance > MAX_DISTANCE) return;

  int r = map(distance, 0, MAX_DISTANCE, 0, RADAR_R);
  float a = angle * 3.14159 / 180.0;
  int x = RADAR_CX + cos(a) * r;
  int y = RADAR_CY - sin(a) * r;

  tft.drawCircle(x, y, 4, RED);
  tft.fillCircle(x, y, 2, RED);
}

void setupRadarScreen() {
  tft.fillScreen(BLACK);
  drawRadarHeader();
  drawRadarGrid();
  drawRangeLabels();
  drawRadarInfo(90, -1);
}

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(2);
  tft.fillScreen(BLACK);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  radarServo.attach(SERVO_PIN);
  radarServo.write(90);
  delay(300);

  drawIndianFlag();

  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.setCursor(39, 72);
  tft.print("15 AUGUST");
  tft.setCursor(38, 132);
  tft.print("LOADING...");

  unsigned long startTime = millis();
  int frame = 0;
  while (millis() - startTime < 8000) {
    drawLoader(frame);
    frame++;
    if (frame >= 8) frame = 0;
    delay(100);
  }

  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);

  tft.setCursor(35, 65);
  tft.print("RADAR");
  tft.setCursor(30, 90);
  tft.print("ONLINE");

  delay(1500);
  setupRadarScreen();
}

void scan(int startAngle, int endAngle, int step) {
  for (int angle = startAngle;
       (step > 0) ? (angle <= endAngle) : (angle >= endAngle);
       angle += step) {

    radarServo.write(angle);
    delay(SERVO_DELAY);

    int distance = getDistance();

    if (lastSweepAngle >= 0) {
      drawSweepLine(lastSweepAngle, BLACK);
      drawRadarGrid();
      drawRangeLabels();
    }

    drawSweepLine(angle, WHITE);

    if (distance >= 1 && distance <= MAX_DISTANCE)
      drawTarget(angle, distance);

    drawRadarInfo(angle, distance);
    lastSweepAngle = angle;
  }
}

void loop() {
  scan(0, 180, SERVO_STEP);
  scan(180, 0, -SERVO_STEP);
}
