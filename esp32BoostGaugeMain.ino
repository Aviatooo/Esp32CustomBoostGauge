#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include "gauge.h"
#include "needless.h"
#include "wax.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite gaugeBack = TFT_eSprite(&tft);
TFT_eSprite needle = TFT_eSprite(&tft);
TFT_eSprite spr    = TFT_eSprite(&tft); // Sprite for meter reading


//----------------------------------------------------------------------------------------------------
const int sensorPin = 34;
const float alpha = 0.95; // Low Pass Filter alpha (0 - 1 )
const float aRef = 3.3; // analog reference
float filteredVal = 2048.0; // midway starting point
float sensorVal;
float voltage;
float toBar;
float bar;
const float alpha_2 = 0.95; // Low Pass Filter alpha (0 - 1 )
float filteredVal_2 = 2048.0; // midway starting point
float sensorVal_2;
float voltage_2;
float maxBar = 0.0;
const int baudRate = 115200; //constant integer to set the baud rate for serial monitor
float batteryVal;
float psiVal;

//----------------------------------------------------------------------------------------------------
float number = 0.0;
int initi = 20;
long numToAngle = number * 10;


void setup() {
  Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);
  tft.pushImage(0,0,240,240,wax);
  int i = 0;
  while(i < 100){
  sensorVal = (float)analogRead(sensorPin);
  filteredVal = (alpha * filteredVal) + ((1.0 - alpha) * sensorVal); // Low Pass Filter
  voltage = (filteredVal / 4096.0) * aRef;
  float psi = (37.5939 * voltage) - 13.5338; // y=mx+b
  psiVal = roundoff(psi, 1);
  sensorVal_2 = (float)analogRead(sensorPin);
  filteredVal_2 = (alpha_2 * filteredVal_2) + ((1.0 - alpha_2) * sensorVal_2); // Low Pass Filter
  voltage_2 = (filteredVal_2 / 4096.0) * aRef;
  float batteryVolts = (1.275 * voltage_2);
  batteryVal = roundoff(batteryVolts, 1);
  i++;
  } 
  delay(1500);
  gaugeBack.setSwapBytes(true);
  needle.setSwapBytes(true);
  createNeedle();
  plotGauge(0);
  delay(1000);

  for(int i = 0; i <= 20; i++){
    int angleMap = map(i, 0, 20, -90, 90);
    plotGauge(angleMap);
    yield();
  }
  while (initi != 0 ) {
    int angleMap = map(initi, 0, 20, -90, 90);
    plotGauge(angleMap);
    delay(20);
    yield();
    initi = initi - 1;
  }
}

void loop() {
    sensorVal = (float)analogRead(sensorPin);
  filteredVal = (alpha * filteredVal) + ((1.0 - alpha) * sensorVal); // Low Pass Filter
  voltage = (filteredVal / 4096.0) * aRef;
  float psi = (37.5939 * voltage) - 13.5338; // y=mx+b
  psiVal = roundoff(psi, 1);
  sensorVal_2 = (float)analogRead(sensorPin);
  filteredVal_2 = (alpha_2 * filteredVal_2) + ((1.0 - alpha_2) * sensorVal_2); // Low Pass Filter
  voltage_2 = (filteredVal_2 / 4096.0) * aRef;
  float batteryVolts = (1.275 * voltage_2);
  batteryVal = roundoff(batteryVolts, 1);
   toBar = 0.068947 * psiVal;
  bar = roundoff(toBar, 1);

  numToAngle = bar * 10;

  if(numToAngle > 20){
    numToAngle = 20;
  }
  if(numToAngle < 0){
    numToAngle = 0;
  }
  
  int angleMap = map(numToAngle, 0, 20, -90, 90);
  plotGauge(angleMap);
  tft.setTextColor(TFT_WHITE);
  tft.drawFloat(bar, 1, 85, 160, 6);
  if(bar >= maxBar){
    maxBar = bar;
  }
  tft.setTextColor(TFT_RED);
  tft.drawFloat(maxBar, 1, 160, 160, 2);
  yield();
  delay(50);
}

void plotGauge(int16_t angle){
  createBackground();
  needle.pushRotated(&gaugeBack, angle, TFT_TRANSPARENT);
  gaugeBack.pushSprite(0,0,TFT_TRANSPARENT);
  
}
void createBackground(){
  gaugeBack.setColorDepth(8);
  gaugeBack.createSprite(240, 240);
  gaugeBack.setPivot(120, 120);
  tft.setPivot(120, 120);
  //gaugeBack.fillSprite(TFT_TRANSPARENT);
  gaugeBack.pushImage(0,0,240,240,gauge);
}

void createNeedle(){
  needle.setColorDepth(8);
  needle.createSprite(13, 121);
  needle.pushImage(0,0,13,121,needless);
  needle.setPivot(6, 121);
}
float roundoff(float value, unsigned char prec)
{
  float pow_10 = pow(10.0f, (float)prec);
  return round(value * pow_10) / pow_10;
}
