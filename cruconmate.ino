#include <M5StickC.h>
#include <TinyGPS++.h>
#include "img/splash.h"
#include "img/large.h"
#include "img/battery.h"
#include "img/gauge.h"
#include "img/gpsoff.h"
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
int target = 60;

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
    // Main Screen + Gauge
    case 10:
      drawSpeed();
      drawGauge();
      M5.Lcd.startWrite();
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      if (M5.BtnA.wasPressed()) {
        screen = 12;
        speed = target;
        count = -1;
      }
      break;
    // Main Screen + Gauge (Demo Mode)
    case 12:
      drawSpeed();
      drawGauge();
      M5.Lcd.startWrite();
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      if (M5.BtnA.wasPressed()) {
        screen = 10;
        count = -1;
      }
      break;
  }
  count++;
  if (count >= 30) {
    if (screen == 12) {
      int rand = random(0, 100);
      if (rand <= 25) {
        speed -= 2;
      } else if (rand > 25 && rand <= 50) {
        speed--;
      } else if (rand > 50 && rand <= 75) {
        speed++;
      } else if (rand > 75) {
        speed += 2;
      }
      if (speed <= target - 15) {
        speed = target - 12;
      }
      if (speed >= target + 15) {
        speed = target + 12;
      }
    }
    count = 0;
  }
  delay(1000 / 30);
}

void drawSpeed() {
  if (screen == 10 || screen == 11) {
    while (GPSRaw.available() > 0) {
      if (gps.encode(GPSRaw.read())) {
        break;
      }
    }
  }
  sprite.fillRect(0, 0, 160, 80, BLACK);
  sprite.pushImage(4, 4, 24, 12, battery);
  if (screen == 10 || screen == 11) {
    if (gps.location.isValid()) {
      speed = (int) gps.speed.kmph();
      sprite.pushImage(4, 21, 24, 18, gpson);
    } else {
      speed = 0;
      sprite.pushImage(4, 21, 24, 18, gpsoff);
    }
  }
  if (speed >= 100) {
    drawLarge(37, 7, 1);
  }
  if (speed >= 10) {
    drawLarge(59, 7, speed / 10 % 10);
  }
  drawLarge(86, 7, speed % 10);
  sprite.pushImage(114, 26, 42, 16, kmh);
  return;
}

void drawGauge() {
  sprite.pushImage(6, 56, 148, 20, gauge);
  if (abs(speed - target) <= 10) {
    int diff = (speed - target) * 7;
    sprite.fillTriangle(66 + diff, 45, 94 + diff, 45, 80 + diff, 61, BLACK);
    if (diff <= -49) {
      sprite.fillTriangle(68 + diff, 47, 92 + diff, 47, 80 + diff, 59, getColor(255, 23, 68));
    } else if (diff > -49 && diff <= -28) {
      sprite.fillTriangle(68 + diff, 47, 92 + diff, 47, 80 + diff, 59, getColor(255, 196, 0));
    } else if (diff > -28 && diff < 28) {
      sprite.fillTriangle(68 + diff, 47, 92 + diff, 47, 80 + diff, 59, getColor(118, 255, 3));
    } else if (diff >= 28 && diff < 49) {
      sprite.fillTriangle(68 + diff, 47, 92 + diff, 47, 80 + diff, 59, getColor(0, 229, 255));
    } else if (diff >= 49) {
      sprite.fillTriangle(68 + diff, 47, 92 + diff, 47, 80 + diff, 59, getColor(41, 121, 255));
    }
  }
  return;
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

uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue){
  return ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3);
}
