#pragma once
#include "objectStruct.h"

#include <Wire.h>
#include "Adafruit_TCS34725.h"

// ใช้ Integration Time 154ms และ Gain 4X
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_154MS,
  TCS34725_GAIN_4X);

// ต้องเรียกใน setup() 
void init_color_sensor() {
  tcs.begin();
}

Color detectColor() {
  delay(1000);
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Normalize RGB 0–255
  float Rn = (float)r / c * 255.0;
  float Gn = (float)g / c * 255.0;
  float Bn = (float)b / c * 255.0;


  // ตัดสินใจแยกสี 
  if (Rn > 130 && Gn > 70 && Bn < 90 && (Rn - Gn) > 30) {
    return ORANGE;
  } else if (Gn > Rn && Gn > Bn && Gn > 80) {
    return GREEN;
  } else if (Rn > 80 && Gn > 80 && Bn < 100 && abs(Rn - Gn) < 60) {
    return YELLOW;
  } 
  return GREEN;

}