#pragma once
#include "ir_StateManager.h"
#include "queue_IR2_Manager.h"
#include "sleep.h"
#include "objectManager.h"
#include "ISR_function.h"
#include "check_color.h"
#include "check_weight.h"
#include "motor_control.h"
#include "box_damaged_from_odroid.h"


// object ปัจจุบันที่ IR1 เจอ (ใช้เก็บ/โชว์ + push เข้า queue ด้วย)
Package currentPkg;
bool hasCurrent = false;  // ของอยู่หน้า IR มั้ย


void ir1_check_item() {
  while(read_ir_on_pcf(9) != LOW){
    delay(5);
  }

  motorSTOP(0);                              // หยุดเช็ค
  currentPkg.color = detectColor();          // ส้ม/เหลือง/เขียว
  currentPkg.weight = detectWeight();        // heavy/light
  currentPkg.damaged = isDamagedBox();  // 0=ไม่ยับ, 1=ยับ (อยากปรับโอกาสค่อยว่ากัน)
  if (currentPkg.damaged) damaged_boxes++;   // ถ้าสุ่มได้1ก็เพิ่มจำนวนกล่องที่นับ
  else intact_boxes++;

  hasCurrent = true;
  motorCW(0);  // สั่งสายพานแรกหมุน
  motorCW(3);  // สั่งสายพานตัวแยกหมุน
  
}

void ir1_workflow_isr() {
  // -------- เหตุการณ์ของ IR1 (มาจาก ISR) ที่จะต้องทำ--------
  if (ir1_event_pending) {
    noInterrupts();
    bool ir1_state = ir1_state_isr;
    unsigned long t_us = ir1_event_time_us;
    ir1_event_pending = false;
    interrupts();

    unsigned long event_ms = t_us / 1000UL;
    if (event_ms - irLastMs[1] > DEBOUNCE_MS) {

      // FALLING: HIGH -> LOW (เจอวัตถุ)
      if (irPrev[1] == HIGH && ir1_state == LOW && irReady[1]) {
        irLastMs[1] = event_ms;
        irReady[1] = false;
        idleTimerActive = false;  // มีงานเข้ามาแล้ว ยกเลิกการนับ idle

        motorCW(0);  // สั่งสายพานเดิน

        ir1_check_item();

        // เก็บลง array สำหรับเก็บ object ทั้งหมดที่สร้าง
        if (objCount < MAX_OBJ) {
          objArray[objCount++] = currentPkg;
        } else {
          Serial.println(F("!! Array เต็ม เก็บ object ใหม่ไม่ได้แล้ว"));
        }

        // เช็คการเข้าคิวสำหรับ IR2 ว่าเข้าได้ไหม
        if (!enqueuePkg(currentPkg)) {
          Serial.println(F("!! Queue เต็ม: IR2 อาจช้าเกินไป"));
        }

        // พิมพ์ array ทั้งหมดทันทีเมื่อสร้าง object ใหม่
        printAllObjects();
      }

      // RISING: LOW -> HIGH (ของออกจาก IR1) -> เริ่มจับเวลา 0.5วิ สำหรับพร้อม detect รอบถัดไป
      if (irPrev[1] == LOW && ir1_state == HIGH) {
        irLastMs[1] = event_ms;
        irReleasedAt[1] = event_ms;
        hasCurrent = false;
      }

      irPrev[1] = ir1_state;  // อัปเดตสถานะล่าสุดของ IR1
    }
  }

  // Re-IR1 ให้พร้อมตรวจกล่องใหม่ หลังกล่องเก่าออกไปแล้ว 0.5 วินาที
  if (!irReady[1] && irPrev[1] == HIGH && (millis() - irReleasedAt[1]) >= READY_DELAY_MS) {
    irReady[1] = true;
  }
}
