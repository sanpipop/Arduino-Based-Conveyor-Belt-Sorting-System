#pragma once
#include "servo_control.h"

// ขาของมอเตอร์แต่ละตัว (High pin)
const int motorPins[4] = {4, 5, 6, 7};  // Motor1=4, Motor2=5, Motor3=6, Motor4=7
const int in_low_Pin = 8;                  // ขา Low ร่วมกัน

// ฟังก์ชันเตรียมขา
void initMotors() {
  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
  }
  pinMode(in_low_Pin, OUTPUT);

  // ปิด (LOW) มอเตอร์ทุกตัวก่อน
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(motorPins[i], LOW);
  }
  digitalWrite(in_low_Pin, LOW);
}

// ฟังก์ชันหมุนตามเข็ม (เลือกมอเตอร์จาก index 0–3)
void motorCW(uint8_t index) {
  if (index < 4) {
    digitalWrite(motorPins[index], HIGH);
  }
}

// ฟังก์ชันหยุดมอเตอร์ (เลือกมอเตอร์จาก index 0–3)
void motorSTOP(uint8_t index) {
  if (index < 4) {
    digitalWrite(motorPins[index], LOW);
  }
}

void send_to_trash(){
  Serial.println(F("พบกล่องยับ"));
  motorSTOP(0);
  motorSTOP(3);
  servoFrom90To180(4);
  motorCW(3); // สั่งสายพานหมุนทิ้งของ
  Serial.println(F("กำลังส่งกล่องยับลงถังขยะ"));
  delay(2000);
  motorSTOP(3);
  servoFrom180To90(4);
  Serial.println(F("ทิ้งกล่องยับเสร็จสิ้นแล้ว"));
}

void send_to_light_lane(){
  motorSTOP(0);
  motorCW(1);
  Serial.println(F("กำลังจัดส่ง....")); 
}

void send_to_heavy_lane(){
  motorSTOP(0);
  motorSTOP(3);
  servoFrom90To0(4);
  motorCW(3);
  motorCW(2);
  Serial.println(F("กำลังจัดส่ง....."));
}

