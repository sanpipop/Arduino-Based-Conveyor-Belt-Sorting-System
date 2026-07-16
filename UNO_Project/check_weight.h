#pragma once
#include "objectStruct.h"

#include "HX711.h"

#define SCK 16  // A2
#define DT 17   // A3

HX711 scale;           // ส่งพินเข้า constructor เลย
float CAL = -1045.0f;  // ค่าคาลิเบรต
const float DEADBAND = 0.5;
const uint8_t N = 10;
float w = 0.0f;

void init_weight_sensor() {
  scale.begin(DT, SCK);
  scale.set_scale(CAL);
  scale.tare();  // ตั้งศูนย์
}

Weight detectWeight() {
  w = scale.get_units(N);
  if (fabs(w) < DEADBAND) w = 0.0;

  // ตัดสินใจหนัก/เบา (เงื่อนไขปรับตามงานจริง)
  if (w >= 55.0 && w <= 65) {  // เช่น >200g ถือว่าหนัก
    return HEAVY;
  } else if (w >= 25 && w <= 35) {
    return LIGHT;
  } else {
    return INVALID;
  }
}
