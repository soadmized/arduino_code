/*
 ----------------------
| Wemos IO | pin (IDE) |
 ----------------------
| D0 (R/W) |   GPIO16  | 
|    D1    |   GPIO5   | motion sensor
|    D2    |   GPIO4   | temp sensor
|    D3    |   GPIO0   | 
|    D4    |   GPIO2   |
|    D5    |   GPIO14  | 
|    D6    |   GPIO12  | 
|    D7    |   GPIO13  |
|    D8    |   GPIO15  |
|    D9    |   GPIO3   |
|    D10   |   GPIO1   |
|    D11   |   GPIO9   |
|    D12   |   GPIO10  |
 ----------------------
*/

#include "functions.h"
#include <Arduino.h>
#include <arduino-timer.h>
#include "creds.h"

auto timer = timer_create_default(); // create a timer with default settings
const int timerInterval = 3000;

const int temperaturePin = 4;
const int PIRSensorPin = 5;
const int lightPin = A0;
const int relayPin = 2;

bool sendToServer(void *) {
  Serial.println("[Main] Sending to server");

  data sensorData;
  sensorData.sensorID = SENSOR_ID;
  sensorData.temperature = readTemperature(temperaturePin);
  sensorData.light = readLight(lightPin);
  sensorData.movement = readPIR(PIRSensorPin);

  String preparedData = prepareDataString(sensorData);
  sendData(SERVER_URL, BASIC_AUTH_STR, preparedData);

  Serial.println("[Main]" + preparedData);
  Serial.printf("[Main] Delay %dms \n-----------\n", timerInterval);
  
  return true; // true if need repeat
}

void setup() {
  setupWifiConn(WIFI_SSID, WIFI_PASS);

  timer.every(timerInterval, sendToServer);

  pinMode(PIRSensorPin, INPUT);
  pinMode(relayPin, OUTPUT);

  Serial.begin(115200);
}

void loop() {
  timer.tick();
}
