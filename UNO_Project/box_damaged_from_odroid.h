#pragma once
#include <Arduino.h>

#define SIGNAL_PIN 14  // ต่อสายจาก PIN13 ของ Odroid เข้ามาที่ D13 ของ Arduino

// กำหนดฟังก์ชันตรวจสอบกล่องจาก Serial
// return true = กล่องยับ, false = กล่องไม่ยับ

void setup_signal_from_odroid(){
  pinMode(SIGNAL_PIN, INPUT);
  digitalWrite(A0, LOW);
}
bool isDamagedBox() {
  delay(500);
  int val = digitalRead(SIGNAL_PIN);

  if (val == 1) {
    Serial.println("damage");
    return 1;
  } else {
    Serial.println("good");
    return 0;
  }
}


