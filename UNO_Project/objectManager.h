#pragma once
#include "thaiColorForSerial.h"
#include "objectStruct.h"

unsigned long damaged_boxes = 0;  // นับกล่องยับที่ “สร้าง” ที่ IR1
unsigned long intact_boxes = 0;   // นับกล่องไม่ยับที่ “สร้าง” ที่ IR1

// ===== ตัวแปรแยกตัวนับ light / heavy แต่ละสี =====
unsigned long light_orange = 0;
unsigned long light_green = 0;
unsigned long light_yellow = 0;
unsigned long heavy_orange = 0;
unsigned long heavy_green = 0;
unsigned long heavy_yellow = 0;


const uint8_t MAX_OBJ = 20;  // เก็บได้สูงสุด 20 ชิ้น
Package objArray[MAX_OBJ];
uint8_t objCount = 0;  // จำนวน object ที่เก็บแล้ว

// ====== ฟังก์ชันพิมพ์ Object ใน array ทั้งหมด ======
void printAllObjects() {
  Serial.println(F("=== Objects in Array ==="));
  if (objCount == 0) {
    Serial.println(F("ไม่มี object ใน array"));
  } else {
    for (uint8_t i = 0; i < objCount; i++) {
      Serial.print("#");
      Serial.print(i + 1);
      Serial.print(" -> ");
      Serial.print(weightName(objArray[i].weight));
      Serial.print(", สี");
      Serial.print(colorName(objArray[i].color));
      Serial.print(F(", สภาพ"));
      Serial.println(objArray[i].damaged ? F("ยับ") : F("ไม่ยับ"));
    }
  }
  Serial.println(F("========================"));
}

// พิมพ์ค่าตัวแปรกล่องแต่ละประเภท
void printSplitCounters() {  // NEW
  Serial.print(F("light: orange/green/yellow = "));
  Serial.print(light_orange);
  Serial.print('/');
  Serial.print(light_green);
  Serial.print('/');
  Serial.println(light_yellow);

  Serial.print(F("heavy: orange/green/yellow = "));
  Serial.print(heavy_orange);
  Serial.print('/');
  Serial.print(heavy_green);
  Serial.print('/');
  Serial.println(heavy_yellow);
}

// เพิ่มจำนวนตัวนับกล่องแต่ละประเภทตาม index ของ IR3..IR8
void incrementLaneCounter(uint8_t idx) {  // CHANGED: map ใหม่
  switch (idx) {
    case 3: /* IR3 */ light_orange++; break;
    case 4: /* IR4 */ light_green++; break;
    case 5: /* IR5 */ light_yellow++; break;
    case 6: /* IR6 */ heavy_orange++; break;
    case 7: /* IR7 */ heavy_green++; break;
    case 8: /* IR8 */ heavy_yellow++; break;
  }
}
