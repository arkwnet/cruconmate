#include <M5StickC.h>
#include <TinyGPS++.h>
#include "img/splash.h"
#include "img/large.h"
#include "img/battery.h"
#include "img/gauge.h"
#include "img/gpson.h"
#include "img/kmh.h"

TFT_eSprite sprite(&M5.Lcd);
HardwareSerial GPSRaw(2);
TinyGPSPlus gps;

const int screenWidth = 160;
const int screenHeight = 80;
int screen = 0;
int count = 0;
int speed = 0;

void setup() {
  M5.begin();
  setCpuFrequencyMhz(20);
  M5.Lcd.setRotation(3);
  M5.Lcd.setSwapBytes(true);
  M5.Axp.ScreenBreath(11);
  sprite.createSprite(M5.Lcd.width(), M5.Lcd.height());
  GPSRaw.begin(9600, SERIAL_8N1, 33, 32);
}

void loop() {
  M5.update();
  switch (screen) {
    // Splash Screen
    case 0:
      if (count == 0) {
        M5.Lcd.pushImage(0, 0, screenWidth, screenHeight, splash);
      }
      if (count == 29) {
        count = -1;
        screen = 10;
      }
      break;
    // Speed Screen
    case 10:
      M5.Lcd.startWrite();
      sprite.fillRect(0, 0, 160, 80, BLACK);
      sprite.pushImage(4, 4, 24, 12, battery);
      sprite.pushImage(4, 21, 24, 18, gpson);
      if (speed >= 100) {
        drawLarge(37, 7, 1);
      }
      if (speed >= 10) {
        drawLarge(59, 7, speed / 10 % 10);
      }
      drawLarge(86, 7, speed % 10);
      sprite.pushImage(114, 26, 42, 16, kmh);
      sprite.pushImage(6, 56, 148, 20, gauge);
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      break;
  }
  count++;
  if (count >= 30) {
    count = 0;
  }
  delay(1000 / 30);
}

void drawLarge(int x, int y, int i) {
  unsigned short large[884];
  int bsize = sizeof(large);
  switch (i) {
    case 0:
      memcpy(large, large0, bsize);
      break;
    case 1:
      memcpy(large, large1, bsize);
      break;
    case 2:
      memcpy(large, large2, bsize);
      break;
    case 3:
      memcpy(large, large3, bsize);
      break;
    case 4:
      memcpy(large, large4, bsize);
      break;
    case 5:
      memcpy(large, large5, bsize);
      break;
    case 6:
      memcpy(large, large6, bsize);
      break;
    case 7:
      memcpy(large, large7, bsize);
      break;
    case 8:
      memcpy(large, large8, bsize);
      break;
    case 9:
      memcpy(large, large9, bsize);
      break;
  }
  sprite.pushImage(x, y, 26, 34, large);
  return;
}
