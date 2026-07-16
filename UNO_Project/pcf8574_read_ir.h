#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "PCF8574.h"

// ====== ตำแหน่ง Address ของ PCF8574 ======
// เปลี่ยนได้ตามการจัมพ์ A0..A2 (0x20..0x27)
#ifndef PCF8574_ADDR
#define PCF8574_ADDR 0x21
#endif

// สร้างออบเจ็กต์ PCF8574
static PCF8574 pcfIr(PCF8574_ADDR);

#define IR1_PIN_ARDUINO 2

// ใช้แมพว่า IR(2-8) แต่ละตัวต้องอ่านจาก P ไหนบน pcf8574
// index 0 ไม่ใช้ (ปล่อยไว้เพื่อให้ index = หมายเลข IR ตรงๆ)
static const int8_t IR_TO_PCF_PIN[10] = {
  -1,      // [0] not used
  -1,      // IR1  -> Arduino D2 (คงเดิม)
   3,      // IR2  -> P3 (เลือกเลน)
   4,      // IR3  -> P4 (light orange)
   5,      // IR4  -> P5 (light green)
   6,      // IR5  -> P6 (light yellow)
   0,      // IR6  -> P0 (heavy orange)
   1,      // IR7  -> P1 (heavy green)
   2,       // IR8  -> P2 (heavy yellow)
   7        // IR9(ให้สายพานแรกหยุดเช็คน้ำหนัก)
};

// ====== เตรียมฮาร์ดแวร์ IR ทั้งหมด ======
static inline void init_ir_on_pcf() {
  // เตรียม I2C + PCF8574
  Wire.begin();
  pcfIr.begin();

  // IR1: ใช้ขา Arduino โดยตรง (active-low -> ใช้ INPUT_PULLUP)
  pinMode(IR1_PIN_ARDUINO, INPUT_PULLUP);

  // ตั้ง Port บน pcf8574 ให้เป็น input (PCF8574 เขียน HIGH เพื่อปล่อยเป็นอินพุต + weak pull-up)
  for (uint8_t ir = 2; ir <= 9; ir++) {
    int8_t p = IR_TO_PCF_PIN[ir];
    if (p >= 0) {
      pcfIr.write(p, HIGH);  // ไปตั้ง port(0-7) บน pcf ให้เป็น HIGH (โหมดอินพุต)
    }
  }
}

// ====== อ่านค่า IR แบบ “ดิบ” (HIGH = ว่าง, LOW = มีวัตถุ) ======
// irIndex = 1-8 ( เขียน IR ตัวที่ต้องการอ่านมาตรงๆได้เลย )
static inline int read_ir_on_pcf(uint8_t irIndex) {
  if (irIndex == 1) {
    return digitalRead(IR1_PIN_ARDUINO);
  }
  if (irIndex >= 2 && irIndex <= 9) {
    int8_t p = IR_TO_PCF_PIN[irIndex]; // อ่าน IR ที่ต้องการจาก pcf มาใส่ใน p
    if (p >= 0) {
      return pcfIr.read(p);   // คืนค่า 0/1 เหมือน digitalRead
    }
  }
  // ถ้า index ผิด ให้คืน HIGH (ถือว่า “ว่าง”) กันพัง
  return HIGH;
}
