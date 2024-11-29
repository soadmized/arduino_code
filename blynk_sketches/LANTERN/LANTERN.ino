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
#define BRIGHTNESS 90

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "creds.h"

BlynkTimer ledTimer;

int snrButton = 13;  // pin connected to sensor button
int snrState = LOW; // flag
int r1 = 0;
int g1 = 0;
int b1 = 0;
int onButton = LOW;
uint32_t color1;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // init leds
int connTimeout = 3000;
int connRetries = 4;


void setup() {
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connectWiFi(WIFI_SSID, WIFI_PASS);

  // ledTimer.setInterval(100L, runLED);
  ledTimer.setInterval(100L, runFire);

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(BRIGHTNESS);
  pixels.show(); // Initialize all pixels to 'off'

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

void runFire() {
  handleButton();

  if (onButton == HIGH) {
    int r = 226, g = 100, b = 35;

    for(int i=0; i<pixels.numPixels(); i++) {
      int flicker = random(0,55);
      int r1 = r-flicker;
      int g1 = g-flicker;
      int b1 = b-flicker;
      if(g1<0) g1=0;
      if(r1<0) r1=0;
      if(b1<0) b1=0;

      pixels.setPixelColor(i,r1,g1, b1);
      delay(1);
    }

    pixels.show();
    delay(random(70,150));

    return;
  }

  pixels.clear();
  pixels.show();
}

void loop() {
  // check connection first
  if (!Blynk.connected()) {

    // if button is on, then turn LED off
    if (onButton) {
      pixels.clear();
      pixels.show();
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

