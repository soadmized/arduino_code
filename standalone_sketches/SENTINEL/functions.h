#pragma once
#include <Arduino.h>

struct data {
  String  sensorID;
  float   temperature;
  int     light;
  int     movement;
};

void   setupWifiConn(String ssid, String pass);
String prepareDataString(data data);
void   sendData(String host, String authStr, String preparedData);
float  readTemperature(int digitalPin);
int    readLight(int analogPin);
int    readPIR(int digitalPin);
String convertIntToBool(int val);