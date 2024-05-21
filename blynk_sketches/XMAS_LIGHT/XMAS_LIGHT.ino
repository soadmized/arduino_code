/*
 LIST OF VIRTUAL PINS

 --- color sliders 1 ---
 v0 - red
 v1 - green
 v2 - blue
 --- controls ----------
 v3 - on_button
 v4 - dual_mode_button
 --- color sliders 2 ---
 v5 - red
 v6 - green
 v7 - blue
 --- snake mode --------
 v8 - snake_button
 v9 - snake_speed
 --- fade mode ---------
 v10 - fade_button 
 */


#define BLYNK_TEMPLATE_ID "TMPLiXtB-8p3"
#define BLYNK_TEMPLATE_NAME "RGB CONTROLLER"

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "creds.h"

#define PIN 13 // data pin
#define NUMPIXELS 50 // number of leds


BlynkTimer ledTimer;
BlynkTimer connTimer;

int mode = 0;
int r1 = 0;
int g1 = 0;
int b1 = 0;
int r2 = 0;
int g2 = 0;
int b2 = 0;
int onButton = 0;
int dualButton = 0;
int fadeButton = 0;
int snakeButton = 0;
int snakeSize = 3;
int snakeSpeed = 10;
uint32_t color1;
uint32_t color2;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // init leds


void setup() {
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connectWiFi(WIFI_SSID, WIFI_PASS);
  
  ledTimer.setInterval(100L, runLED);
  connTimer.setInterval(10000L, checkConnection);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}


// --- SYNCHRONIZE WIDGETS AND HARDWARE VARIABLES
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
  Blynk.syncVirtual(V6);
  Blynk.syncVirtual(V7);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10);
}


// --- GETTING WIDGETS DATA FROM APP
BLYNK_WRITE(V0) {
  r1 = param.asInt(); // get red_1 value from app
}

BLYNK_WRITE(V1) {
  g1 = param.asInt(); // get green_1 value from app
}

BLYNK_WRITE(V2) {
  b1 = param.asInt(); // get blue_1 value from app
}

BLYNK_WRITE(V3) {
  onButton = param.asInt(); // get on button state from app
}

BLYNK_WRITE(V4) {
  dualButton = param.asInt(); // get dual mode button state from app
}

// get red value from app
BLYNK_WRITE(V5) {
  r2 = param.asInt();
}

// get green value from app
BLYNK_WRITE(V6) {
  g2 = param.asInt();
}

// get blue value from app
BLYNK_WRITE(V7) {
  b2 = param.asInt();
}
// ------------------------

// get snake button value from app
BLYNK_WRITE(V8) {
  snakeButton = param.asInt();
}
// ------------------------

// get snake speed value from app
BLYNK_WRITE(V9) {
  snakeSpeed = param.asInt();
}
// ------------------------

// get snake button value from app
BLYNK_WRITE(V10) {
  fadeButton = param.asInt();
}
// ------------------------


void dualFill() {
  color1 = pixels.Color(g1, r1, b1);
  color2 = pixels.Color(g2, r2, b2);
  // в цикле прохожу по всем диодам в ленте
  // четным диодам - color_1
  // нечетным диодам - color_2
  for(int i = 0; i<NUMPIXELS; i++){
    if (i % 2 == 0) {
      pixels.setPixelColor(i, color1);
      pixels.show();
      } else {
        pixels.setPixelColor(i, color2);
        pixels.show();
      }
    }
}


void fade() {
    for (int i = g1, j = r1, k = b1; i > 0, j > 0, k > 0; i--, j--, k-- ) {
      delay(30);
      pixels.fill((i, j, k));
      pixels.show();
    }
for (int i = 0, j = 0, k = 0; i < g1, j < r1, k < b1; i++, j++, k++ ) {
      delay(30);
      pixels.fill((i, j, k));
      pixels.show();
    }    
}


void snakeRun(int speed) {
  pixels.clear();
  for (int i = 0; i<NUMPIXELS; i++) {
    // color_1 = pixels.Color(random(100, 230), random(0, 100), random(0, 170));
    // color_2 = pixels.Color(random(0, 90), random(100, 240), random(0, 200));
    color1 = pixels.Color(random(0, 30), random(0, 30), random(0, 30));
    color2 = pixels.Color(random(0, 30), random(0, 30), random(0, 30));
    delay(speed);
    pixels.setPixelColor(i, color1);
    pixels.setPixelColor(i+1, color2);
    pixels.setPixelColor(i+2, color1);
    pixels.setPixelColor(i+3, color2);
    pixels.setPixelColor(i-1, 0, 0, 0);
    pixels.show();
  }
}


// основная функция, где происходит все. Замена loop
void runLED() {
  if (onButton == 1) {
    if (dualButton == 1 & snakeButton == 0) {
      mode = 1;
    }
    else if (snakeButton == 1) {
      mode = 2;
    } 
    else if (fadeButton == 1) {
      mode = 3;
    }
    else {
      mode = 0;
    }

    switch(mode) {
      case 1:
        dualFill();
        break;
      case 2:
        snakeRun(snakeSpeed);
        break;
      case 3:
        fade();
      default:
        color1 = pixels.Color(g1, r1, b1);
        pixels.fill(color1);
        pixels.show();
        break;        
    }

  } else {
    pixels.clear();
    pixels.show();
  }
}

void checkConnection() {
  bool conn = Blynk.connected();
  int timeout = 3000;

  if (!conn) {
    Blynk.connect(timeout);
  }
}

void loop() {
  connTimer.run();
  ledTimer.run();

  Blynk.run();
}
