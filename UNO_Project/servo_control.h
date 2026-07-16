#pragma once
#include <Servo.h>

Servo servos[5];   // อ็อบเจกต์ Servo 4 ตัว + 1 ตัวแกนหมุน

// ฟังก์ชันสั่งหมุน Servo จาก 90° → 0°
void servoFrom90To0(uint8_t index) {
  if (index < 5) {
    for (int pos = 110; pos >= 6; pos--) {
      servos[index].write(pos);
      delay(15); // ปรับเวลาให้หมุนเร็ว/ช้าตามต้องการ
    }
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 0° → 90°
void servoFrom0To90(uint8_t index) {
  if (index < 5) {
    for (int pos = 6; pos <= 110; pos++) {
      servos[index].write(pos);
      delay(15);
    }
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 90° → 180°
void servoFrom90To180(uint8_t index) {
  if (index < 5) {
    for (int pos = 110; pos <= 180; pos++) {
      servos[index].write(pos);
      delay(15);
    }
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 180° → 90°
void servoFrom180To90(uint8_t index) {
  if (index < 5) {
    for (int pos = 180; pos >= 105; pos--) {
      servos[index].write(pos);
      delay(15);
    }
  }
}

// ฟังก์ชันเตรียมใช้งาน Servo
void initServo(uint8_t index, uint8_t pin) {
  if (index < 5) {
    servos[index].attach(pin);   // ระบุ index ว่าจะใช้ Servo ตัวไหน
  }
  if (index <= 1){
    servos[index].write(90);
  }
  else if (index <= 3){
    servos[index].write(0);
  }
  else if (index == 4){
    servos[index].write(110);
    servoFrom90To0(4);
    Serial.println("หมุน1");
    servoFrom0To90(4);
    Serial.println("หมุน2");
    servoFrom90To180(4);
    Serial.println("หมุน3");
    servoFrom180To90(4);
    Serial.println("หมุนเสร็จ");
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 90° → 0°
void servoLaneFrom90To0(uint8_t index) {
  if (index < 5) {
    for (int pos = 90; pos >= 0; pos--) {
      servos[index].write(pos);
      delay(1); // ปรับเวลาให้หมุนเร็ว/ช้าตามต้องการ
    }
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 0° → 90°
void servoLaneFrom0To90(uint8_t index) {
  if (index < 5) {
    for (int pos = 0; pos <= 90; pos++) {
      servos[index].write(pos);
      delay(1);
    }
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 90° → 180°
void servoLaneFrom90To180(uint8_t index) {
  if (index < 5) {
    for (int pos = 90; pos <= 180; pos++) {
      servos[index].write(pos);
      delay(1);
    }
  }
}

// ฟังก์ชันสั่งหมุน Servo จาก 180° → 90°
void servoLaneFrom180To90(uint8_t index) {
  if (index < 5) {
    for (int pos = 180; pos >= 90; pos--) {
      servos[index].write(pos);
      delay(1);
    }
  }
}
