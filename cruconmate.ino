#include <M5StickC.h>
#include <TinyGPS++.h>
#include "img/splash.h"
#include "img/large.h"
#include "img/small.h"
#include "img/settings.h"
#include "img/battery.h"
#include "img/charge.h"
#include "img/demo.h"
#include "img/gauge.h"
#include "img/gpsoff.h"
#include "img/gpson.h"
#include "img/kmh.h"
#include "img/tbox.h"

TFT_eSprite sprite(&M5.Lcd);
HardwareSerial GPSRaw(2);
TinyGPSPlus gps;

const int version[3] = {1, 0, 0};
const int screenWidth = 160;
const int screenHeight = 80;
const int pin = 26;
int screen = 0;
int count = 0;
int speed = 0;
int target = 60;
float vbat = 0.0f;
float ibat = 0.0f;

void setup() {
  M5.begin();
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  setCpuFrequencyMhz(20);
  M5.Lcd.setRotation(3);
  M5.Lcd.setSwapBytes(true);
  M5.Axp.ScreenBreath(11);
  sprite.createSprite(M5.Lcd.width(), M5.Lcd.height());
  GPSRaw.begin(9600, SERIAL_8N1, 33, 32);
}

void loop() {
  M5.update();
  while (GPSRaw.available() > 0) {
    if (gps.encode(GPSRaw.read())) {
      break;
    }
  }
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
        digitalWrite(pin, LOW);
        screen = 11;
        count = -1;
      }
      break;
    // Main Screen + Set Target
    case 11:
      drawSpeed();
      sprite.pushImage(4, 48, 152, 26, tbox);
      if (target >= 100) {
        drawSmall(78, 53, 1);
      }
      drawSmall(88, 53, target / 10 % 10);
      drawSmall(98, 53, 0);
      M5.Lcd.startWrite();
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      if (M5.BtnA.wasPressed()) {
        speed = target;
        screen = 12;
        count = -1;
      }
      if (M5.BtnB.wasPressed()) {
        target += 10;
        if (target >= 130) {
          target = 30;
        }
      }
      break;
    // Main Screen + Gauge (Demo Mode)
    case 12:
      drawSpeed();
      drawGauge();
      sprite.pushImage(3, 21, 28, 18, demo);
      M5.Lcd.startWrite();
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      if (M5.BtnA.wasPressed()) {
        digitalWrite(pin, LOW);
        screen = 20;
        count = -1;
      }
      break;
    // Information Screen (Version)
    case 20:
      sprite.fillRect(0, 0, 160, 80, BLACK);
      sprite.pushImage(0, 0, screenWidth, 16, settings_header_version);
      sprite.pushImage(0, 16, screenWidth, 2, settings_border);
      sprite.pushImage(15, 23, 131, 49, settings_version1);
      drawSmall(92, 57, version[0]);
      drawSmall(106, 57, version[1]);
      drawSmall(120, 57, version[2]);
      M5.Lcd.startWrite();
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      if (M5.BtnA.wasPressed()) {
        screen = 10;
        count = -1;
      }
      if (M5.BtnB.wasPressed()) {
        screen = 21;
        count = -1;
      }
      break;
    // Information Screen (License)
    case 21:
      sprite.fillRect(0, 0, 160, 80, BLACK);
      sprite.pushImage(0, 0, screenWidth, 16, settings_header_version);
      sprite.pushImage(0, 16, screenWidth, 2, settings_border);
      sprite.pushImage(4, 24, 152, 50, settings_version2);
      M5.Lcd.startWrite();
      sprite.pushSprite(0, 0);
      M5.Lcd.endWrite();
      if (M5.BtnA.wasPressed()) {
        screen = 10;
        count = -1;
      }
      if (M5.BtnB.wasPressed()) {
        screen = 20;
        count = -1;
      }
      break;
  }
  count++;
  if (count == 1) {
    vbat = M5.Axp.GetBatVoltage();
    ibat = M5.Axp.GetBatCurrent();
  }
  if (screen == 10 || screen == 12) {
    if (count == 1 || count == 11 || count == 21) {
      if (abs(speed - target) <= 10 && abs(speed - target) >= 4) {
        digitalWrite(pin, HIGH);
      }
    }
    if (count == 3 || count == 13 || count == 23) {
      digitalWrite(pin, LOW);
    }
    if (count == 6 || count == 16 || count == 26) {
      if (abs(speed - target) <= 10 && abs(speed - target) >= 7) {
        digitalWrite(pin, HIGH);
      }
    }
    if (count == 8 || count == 18 || count == 28) {
      digitalWrite(pin, LOW);
    }
  }
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
  sprite.fillRect(0, 0, 160, 80, BLACK);
  if (ibat < 0.0f) {
    sprite.pushImage(4, 4, 24, 12, battery);
    uint16_t batcol = WHITE;
    if (vbat >= 3.0f) {
      if (vbat <= 3.4f) {
        batcol = getColor(255, 23, 68);
      }
      sprite.fillRect(8, 6, 1, 8, batcol);
      if (vbat >= 3.2f) {
        sprite.fillRect(9, 5, 3, 10, batcol);
      }
      if (vbat >= 3.4f) {
        sprite.fillRect(12, 5, 3, 10, batcol);
      }
      if (vbat >= 3.6f) {
        sprite.fillRect(15, 5, 3, 10, batcol);
      }
      if (vbat >= 3.8f) {
        sprite.fillRect(18, 5, 3, 10, batcol);
      }
      if (vbat >= 4.0f) {
        sprite.fillRect(21, 5, 3, 10, batcol);
      }
      if (vbat >= 4.2f) {
        sprite.fillRect(24, 5, 2, 10, batcol);
      }
    }
  } else {
    sprite.pushImage(4, 4, 24, 12, charge);
  }
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

void drawSmall(int x, int y, int i) {
  unsigned short small[140];
  int bsize = sizeof(small);
  switch (i) {
    case 0:
      memcpy(small, small0, bsize);
      break;
    case 1:
      memcpy(small, small1, bsize);
      break;
    case 2:
      memcpy(small, small2, bsize);
      break;
    case 3:
      memcpy(small, small3, bsize);
      break;
    case 4:
      memcpy(small, small4, bsize);
      break;
    case 5:
      memcpy(small, small5, bsize);
      break;
    case 6:
      memcpy(small, small6, bsize);
      break;
    case 7:
      memcpy(small, small7, bsize);
      break;
    case 8:
      memcpy(small, small8, bsize);
      break;
    case 9:
      memcpy(small, small9, bsize);
      break;
  }
  sprite.pushImage(x, y, 10, 14, small);
  return;
}

uint16_t getColor(uint8_t red, uint8_t green, uint8_t blue){
  return ((red>>3)<<11) | ((green>>2)<<5) | (blue>>3);
}
