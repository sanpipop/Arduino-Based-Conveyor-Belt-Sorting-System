#pragma once
#include "ir_StateManager.h"
#include "laneManager.h"
#include "queue_IR2_Manager.h"
#include "thaiColorForSerial.h"
#include "objectManager.h"
#include "servo_control.h"
#include "motor_control.h"
#include "pcf8574_read_ir.h"


void ir3to8_workflow_pooling() {
  //  ตรวจเช็ค IR3..IR8 (one-shot + re-arm 0.5s) 
  for (uint8_t ir = 3; ir <= 8; ir++) {
    bool val = read_ir_on_pcf(ir);  // LOW = เจอวัตถุ

    // FALLING edge = พบของ
    if (irPrev[ir] == HIGH && val == LOW && (millis() - irLastMs[ir]) > DEBOUNCE_MS && irReady[ir]) {
      irLastMs[ir] = millis();
      irReady[ir] = false;

      // light lane: IR3-IR5 
      if (ir <= 5) {
        if (lightLaneGateOpen && lightLaneHasPackage && ir == lightLaneTargetIdx) {
          incrementLaneCounter(ir);  // +1 ไปที่ light_orange/green/yellow
          Serial.print(F("IR"));
          Serial.print(ir);
          Serial.println(F(": +1"));
          if (ir < 5) { // IR3,4 -> 0,1
            servoLaneFrom90To180(ir-3);
            Serial.println(F("เจอแล้วจ้า 1"));
            delay(200);
            servoLaneFrom180To90(ir-3);
            motorSTOP(1);
            motorSTOP(3);
          }else{
            delay(1000);
            motorSTOP(1);
            motorSTOP(3);
          }
          
          printSplitCounters();

          // ปิดประตูเลน light และเคลียร์ busy
          lightLaneGateOpen = false;
          lightLaneHasPackage = false;
          laneBusy[LANE_LIGHT] = false;


          // เริ่ม idle timer ได้ถ้าคิวว่างและ IR1 ว่าง
          if (qCount == 0 && irPrev[1] == HIGH) {
            idleTimerActive = true;
            lastNoQueueMs = millis();  // เริ่มจับเวลาว่าไม่มีคิวจากตอนนี้
          }
        }
      }
      // heavy lane: IR6-IR8 => index 6..8
      else {
        if (heavyLaneGateOpen && heavyLaneHasPackage && ir == heavyLaneTargetIdx) {
          incrementLaneCounter(ir);  // +1 ไปที่ heavy_orange/green/yellow
          Serial.print(F("IR"));
          Serial.print(ir);
          Serial.println(F(": +1"));
          if (ir < 8) { // IR6,7 -> 2,3
            servoLaneFrom0To90(ir - 4);
            Serial.println(F("เจอแล้วจ้า 2"));
            delay(200);
            servoLaneFrom90To0(ir - 4);
            motorSTOP(2);
            motorSTOP(3);
            servoFrom0To90(4);
            Serial.println(F("หมุนกลับแล้ว"));
          }else{
            delay(1000);
            motorSTOP(2);
            motorSTOP(3);
            servoFrom0To90(4);
            Serial.println(F("หมุนกลับแล้ว"));
          }
          
          printSplitCounters();

          heavyLaneGateOpen = false;
          heavyLaneHasPackage = false;
          laneBusy[LANE_HEAVY] = false;


          if (qCount == 0 && irPrev[1] == HIGH) {
            idleTimerActive = true;
            lastNoQueueMs = millis();  // เริ่มจับเวลาว่าไม่มีคิวจากตอนนี้
          }
        }
      }
    }


    // RISING edge = ของออกไปแล้ว เตรียมตัวเริ่มจับเวลา
    if (irPrev[ir] == LOW && val == HIGH && (millis() - irLastMs[ir]) > DEBOUNCE_MS) {
      irLastMs[ir] = millis();
      irReleasedAt[ir] = millis();
    }

    // re-IR ให้กลับมาพร้อมเช็ค หลังของออกไปแล้ว 0.5 วินาที
    if (!irReady[ir] && val == HIGH && (millis() - irReleasedAt[ir]) >= READY_DELAY_MS) {
      irReady[ir] = true;
    }

    irPrev[ir] = val;
  }
}