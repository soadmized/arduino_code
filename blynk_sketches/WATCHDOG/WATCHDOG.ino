/*
LIST OF DIGITAL PINS
 ----------------------
| Wemos IO | pin (IDE) |
 ----------------------
| D0 (R/W) |   GPIO16  |
|    D1    |   GPIO5   | PIR
|    D2    |   GPIO4   | outer temp
|    D3    |   GPIO0   | inner temp
|    D4    |   GPIO2   | 
|    D5    |   GPIO14  | 
|    D6    |   GPIO12  | 
|    D7    |   GPIO13  | 
|    D8    |   GPIO15  |
|    D9    |   GPIO3   |
|    D10   |   GPIO1   |
|    D11   |   GPIO9   |
|    D12   |   GPIO10  |
|    A0    |   ADC     | light
 ----------------------

LIST OF VIRTUAL PINS
v0 - inner temp
v1 - outer temp
v2 - light
v3 - PIR 

*/

#define BLYNK_TEMPLATE_ID "TMPLXadWzb7w"
#define BLYNK_TEMPLATE_NAME "SENSORS CONTROLLER"
#define BLYNK_PRINT Serial

#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "creds.h" // ignored by git, should be created by yourself

int lightPin = A0;
int pirPin = 5;

OneWire oneWire1(0); // пин подключения внутреннего датчика ds18b20
OneWire oneWire2(4); // пин подключения внешнего датчика ds18b20
DallasTemperature innerTempSensor(&oneWire1);
DallasTemperature outerTempSensor(&oneWire2);

BlynkTimer sensorsTimer;
BlynkTimer connTimer;

void setup()
{
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connectWiFi(WIFI_SSID, WIFI_PASS);
  
  sensorsTimer.setInterval(600L, readSensors);
  connTimer.setInterval(10000L, checkConnection);
  
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
}

void readSensors() {
  // read light sensor
  int lightValue = analogRead(lightPin);

  // read inner temp
  innerTempSensor.requestTemperatures();
  float innerTemp = innerTempSensor.getTempCByIndex(0);

  // read outer temp
  outerTempSensor.requestTemperatures();
  float outerTemp = outerTempSensor.getTempCByIndex(0);

  // read PIR sensor
  int pirValue = digitalRead(pirPin);

  // send all data to Blynk
  Blynk.virtualWrite(V0, innerTemp);
  Blynk.virtualWrite(V1, outerTemp);
  Blynk.virtualWrite(V2, lightValue);
  Blynk.virtualWrite(V3, pirValue);
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
  sensorsTimer.run();
  
  Blynk.run();
}

