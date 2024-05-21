#include "functions.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "creds.h"

void setupWifiConn(String ssid, String pass){
  WiFi.begin(ssid, pass);
};

String prepareDataString(data data) {
  String preparedData = "{\"id\": \"%q\", \"temp\": %t, \"light\": %l, \"motion\": %m}";

  String q = String(data.sensorID);
  String t = String(data.temperature);
  String l = String(data.light);
  String m = String(data.movement);

  preparedData.replace("%q", q);
  preparedData.replace("%t", t);
  preparedData.replace("%l", l);
  preparedData.replace("%m", m);

  return preparedData;
}

void sendData(String host, String authStr, String preparedData) {
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient wifiConn;
    HTTPClient httpConn;
    String basicAuth = "Basic " + authStr;
    Serial.println("[HTTP] Begin HTTP ...");

    if (httpConn.begin(wifiConn, host)) {
      Serial.println("[HTTP] Sending POST ...");

      httpConn.addHeader("Content-Type", "application/json");
      httpConn.addHeader("Authorization", basicAuth);
      int httpCode = httpConn.POST(preparedData);

      if (httpCode == 200) {
        Serial.println("[HTTP] Response: 200 OK");

        // if any payload
        // String payload = httpConn.getString();
        // Serial.println(payload);
      } else {
        Serial.printf("[HTTP] Response: %d\n", httpCode);
        String payload = httpConn.getString();
        Serial.print("[HTTP] Error: " + payload);
      }
      httpConn.end();
    } else {
      Serial.println("[HTTP] Unable to connect");
    }
  }
};

float readTemperature(int digitalPin){
  OneWire oneWire1(digitalPin);
  DallasTemperature tempSensor(&oneWire1);

  tempSensor.requestTemperatures();
  float temperature = tempSensor.getTempCByIndex(0);

  return temperature;
};

int readLight(int analogPin){
  int light = analogRead(analogPin);

  return light;
};

int readPIR(int digitalPin){
  int signal = digitalRead(digitalPin);
  
  return signal;
};

String convertIntToBool(int val) {
  if (val == 1) {
    return "true";
  }

  return "false";
};
