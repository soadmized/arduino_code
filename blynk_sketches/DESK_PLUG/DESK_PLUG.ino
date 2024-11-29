/*
 ----------------------
| Wemos IO | pin (IDE) |
 ----------------------
| D0 (R/W) |   GPIO16  | 
|    D1    |   GPIO5   | 
|    D2    |   GPIO4   | solid state relay
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
 ----------------------
 
 VIRTUAL PINS:
 v0 - app button

*/

#define BLYNK_TEMPLATE_ID "TMPL5SBwFTD9"
#define BLYNK_TEMPLATE_NAME "SSR CONTROLLER"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "creds.h"

int relayPin = 4; // pin connected to solid state relay
int appButton = LOW;  // app button
int snrButton = 13; // pin connected to sensor button
int snrState = LOW;  // flag
int connTimeout = 3000;
int connRetries = 4;

BlynkTimer buttonTimer;

// synchronize variables between app and hardware
BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
}

BLYNK_WRITE(V0){
  appButton = param.asInt();
  digitalWrite(relayPin, appButton);
}

void setup(){
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connectWiFi(WIFI_SSID, WIFI_PASS);

  buttonTimer.setInterval(100L, handleButton);

  pinMode(relayPin, OUTPUT);
  // pinMode(snrButton, INPUT);
}

void handleButton(){
  bool snrButtonVal = (digitalRead(snrButton) != 0);

  if (!snrButtonVal) {
    if (snrState) {
      appButton = !appButton;
      digitalWrite(relayPin, appButton); // switching relay
      Blynk.virtualWrite(V0, appButton);  // sending state to app
    }

    snrState = LOW;

    return;
  }

  snrState = HIGH;
}

void loop(){
  // check connection first
  if (!Blynk.connected()) {

    // if button is on, then turn it off
    if (appButton) {
      digitalWrite(relayPin, !appButton);
      Blynk.virtualWrite(V0, !appButton);
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

  Blynk.run();
}
