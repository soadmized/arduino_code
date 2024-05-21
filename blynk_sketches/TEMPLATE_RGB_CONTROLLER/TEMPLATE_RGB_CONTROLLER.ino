/*
LIST OF DIGITAL PINS
 ----------------------
| Wemos IO | pin (IDE) |
 ----------------------
| D0 (R/W) |   GPIO16  |
|    D1    |   GPIO5   | 
|    D2    |   GPIO4   | rgb data
|    D3    |   GPIO0   | 
|    D4    |   GPIO2   | 
|    D5    |   GPIO14  | 
|    D6    |   GPIO12  | 
|    D7    |   GPIO13  | sensor button
|    D8    |   GPIO15  |
|    D9    |   GPIO3   |
|    D10   |   GPIO1   |
|    D11   |   GPIO9   |
|    D12   |   GPIO10  |
|    A0    |   ADC     |
 ----------------------

 LIST OF VIRTUAL PINS

 --- color sliders 1 ---
 v0 - red
 v1 - green
 v2 - blue
 --- controls ----------
 v3 - on_button
 */

#define BLYNK_TEMPLATE_ID "TMPLiXtB-8p3"
#define BLYNK_TEMPLATE_NAME "RGB CONTROLLER"

#define PIN 4 // data pin
#define NUMPIXELS 70 // number of leds

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "creds.h"

BlynkTimer ledTimer;
BlynkTimer connTimer;

int snrButton = 13;  // pin connected to sensor button
int snrState = LOW; // flag
int r1 = 0;
int g1 = 0;
int b1 = 0;
int onButton = LOW;
uint32_t color1;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // init leds


void setup() {
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connectWiFi(WIFI_SSID, WIFI_PASS);

  ledTimer.setInterval(100L, runLED);
  connTimer.setInterval(10000L, checkConnection);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pinMode(snrButton, INPUT);
}


// --- SYNCHRONIZE WIDGETS AND HARDWARE VARIABLES
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
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

void handleButton(){
  if (digitalRead(snrButton) == LOW){
    if (snrState != LOW){
      onButton = !onButton;
      Blynk.virtualWrite(V0, onButton); // sending state to app
    }
    snrState = LOW;
  } else {
    snrState = HIGH;
  }
}

// основная функция, где происходит все. Замена loop
void runLED() {
  color1 = pixels.Color(g1, r1, b1);
  handleButton();

  if (onButton == HIGH) {
    pixels.fill(color1);
    pixels.show();

    return;
  }

  pixels.clear();
  pixels.show();
}

void checkConnection() {
  bool conn = Blynk.connected();
  int timeout = 3000;

  if (!conn) {
    Blynk.connect(timeout);
  }
}

void loop() {
  ledTimer.run();
  connTimer.run();

  Blynk.run();
}
