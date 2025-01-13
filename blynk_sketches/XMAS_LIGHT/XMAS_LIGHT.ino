/*
 LIST OF VIRTUAL PINS

 --- color sliders 1 ---
 v0 - red
 v1 - green
 v2 - blue
 --- controls ----------
 v3 - on_button
 v4 - xmas_mode_button
 v5 - main_mode_button
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

int onButton = 0;
int xmasModeButton = 0;
int mainModeButton = 0;

int connTimeout = 3000;
int connRetries = 4;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // init leds

void setup() {
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connectWiFi(WIFI_SSID, WIFI_PASS);
  
  ledTimer.setInterval(100L, runLED);

  pixels.begin();
}

// --- SYNCHRONIZE WIDGETS AND HARDWARE VARIABLES
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V5);
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
  xmasModeButton = param.asInt(); // get xmas mode button state from app
}

// get red value from app
BLYNK_WRITE(V5) {
  mainModeButton = param.asInt(); // get main mode button state from app
}

void setXmasColors() {
  uint32_t redColor = pixels.Color(33, 255, 0); // !!! GREEN RED BLUE !!!
  uint32_t greenColor = pixels.Color(255, 0, 45); // !!! GREEN RED BLUE !!!
  
  for(int i = 0; i < NUMPIXELS; i++){
    if (i % 2 == 0) {
      pixels.setPixelColor(i, redColor);
      } else {
        pixels.setPixelColor(i, greenColor);
      }
    }
}

void runLED() {
  uint32_t color;

  if (onButton == 1) {
    // pixels.clear();

    if (xmasModeButton == 1) {
      if (mainModeButton == 1) {
        xmasModeButton = 0;
        // Blynk.virtualWrite(V4, xmasModeButton);
      
        color = pixels.Color(88, 255, 34); // !!! GREEN RED BLUE !!!
        pixels.fill(color);
        pixels.show();

        return;
      }

      mainModeButton = 0;
      // Blynk.virtualWrite(V5, mainModeButton);
      
      setXmasColors();
      pixels.show();

      return;
    }

    if (mainModeButton == 1) {
      if (xmasModeButton == 1) {
        mainModeButton = 0;
        // Blynk.virtualWrite(V5, mainModeButton);
      
        setXmasColors();
        pixels.show();

        return;
      }

      xmasModeButton = 0;
      // Blynk.virtualWrite(V4, xmasModeButton);
      
      color = pixels.Color(88, 255, 34); // !!! GREEN RED BLUE !!!
      pixels.fill(color);
      pixels.show();

      return;
    }

    color = pixels.Color(g1, r1, b1); // !!! GREEN RED BLUE !!!
    pixels.fill(color);
    pixels.show();

    return;
  }

  pixels.clear();
  pixels.show();
}

void loop() {
  // check connection first
  if (!Blynk.connected()) {

    // if button is on, then turn it off
    if (onButton) {
      pixels.clear();
      pixels.show();
      Blynk.virtualWrite(V3, !onButton);
    }

    // try to reconnect
    for (int i = 0; i < connRetries; i++) {
      if (Blynk.connect(connTimeout)) {
        return;
      }
    }

    // reboot controller, if cant reconnect
    ESP.restart();
  }

  ledTimer.run();
  Blynk.run();
}
