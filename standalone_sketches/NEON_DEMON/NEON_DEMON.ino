/*
D2 - split button
D3 - split color button
D4 - led data

A0 - red pot
A1 - green pot
A2 - blue pot

Connect pots to 5V pin of Arduino board!

*/


#include <Adafruit_NeoPixel.h>
#define PIN 13 // data pin
#define NUMPIXELS 60 // number of leds#include <Adafruit_NeoPixel.h>


int split_button = 3;
//int split_color_button = 3;
int split_state = LOW;
int split_color_state = LOW;
int flag = 0;
int stat = 0;
int rgb[3];
int analog_pins[3] = {A3, A1, A2};
int r1;  
int g1;
int b1;
int r2;
int g2;
int b2;
uint32_t color_1;
uint32_t color_2;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);
  //pinMode(13, OUTPUT);
  pinMode(split_button, INPUT);
  pixels.begin();
}


void readValues() {
  for (int i=0; i<3; i++) {
    rgb[i] = analogRead(analog_pins[i]) / 4;
    rgb[i] = (rgb[i] <= 4) ? 0 : rgb[i];
    delay(10);
  }
}


void loop() {
  readValues();
  Serial.println(rgb[0]);
  Serial.println(rgb[1]);
  Serial.println(rgb[2]);
  for(int i=0; i<NUMPIXELS; i++){
    color_1 = pixels.Color(rgb[0], rgb[1], rgb[2]);
    pixels.fill(color_1);
    pixels.show();
  }
}
