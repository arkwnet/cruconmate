#include <M5StickC.h>
#include <TinyGPS++.h>
#include "img/splash.h"
#include "img/sample.h"

TFT_eSprite sprite(&M5.Lcd);
HardwareSerial GPSRaw(2);
TinyGPSPlus gps;

const int screenWidth = 160;
const int screenHeight = 80;
int screen = 0;
int count = 0;

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
      sprite.pushImage(0, 0, screenWidth, screenHeight, sample);
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
