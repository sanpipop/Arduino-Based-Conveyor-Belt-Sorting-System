#pragma once
#include "ir_StateManager.h"
#include "laneManager.h"
#include "ISR_function.h"

#include <avr/sleep.h>
#include <avr/power.h>


// --- Idle timer (เริ่มจับหลัง IR2 ส่งของแล้ว + คิวว่าง) ---
const unsigned long IDLE_SLEEP_MS = 20000;  // จับเวลาว่าจะหลับ

// สำหรับโหมดใหม่นับหลังคิวว่าง
unsigned long lastNoQueueMs = 0;
bool idleTimerActive = false;


//ฟังก์ชันหลับ/ปลุก
void sleepUntilIR1() {
  // เปลี่ยนโหมด interrupt ให้ปลุกจาก Power-down ได้ (LOW level)
  detachInterrupt(digitalPinToInterrupt(2));
  attachInterrupt(digitalPinToInterrupt(2), ir1_isr, LOW);

  // --- Log ก่อนหลับ ---
  Serial.println(F("[Power] เข้าสู่โหมดหลับ (power-down) รอ IR1 ปลุก..."));  // <<< ADD
  Serial.flush();

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  noInterrupts();
  sleep_enable();
#if defined(BODS) && defined(BODSE)
  sleep_bod_disable();  // ปิด BOD เพื่อลดกินไฟ (ถ้า core รองรับ)
#endif
  interrupts();

  sleep_cpu();  // หลับจริง

  // ===== ตื่นแล้ว (เพราะ IR1 ดึง LOW) =====
  sleep_disable();

  // กลับไปใช้โหมดจับขอบตามเดิม
  detachInterrupt(digitalPinToInterrupt(2));
  attachInterrupt(digitalPinToInterrupt(2), ir1_isr, CHANGE);

  // --- Log หลังตื่น ---
  Serial.println(F("[Power] ตื่นแล้ว (ปลุกด้วย IR1)"));
}

void check_sleep_condition(){
    // ===== Idle 20s (คิวว่าง + ไม่มีงานปลายทางค้าง) -> หลับ, ปลุกด้วย IR1 =====
    if (idleTimerActive
        && (millis() - lastNoQueueMs) >= IDLE_SLEEP_MS
        && irPrev[1] == HIGH     // ไม่มีของคา IR1
        && qCount == 0         // คิว IR2 ว่างจริง
        && !lightLaneGateOpen  // เลน light ไม่ได้รอ IR3..IR5 ตรวจ
        && !lightLaneHasPackage
        && !heavyLaneGateOpen  // เลน heavy ไม่ได้รอ IR6..IR8 ตรวจ
        && !heavyLaneHasPackage) {
        Serial.println(F("[Power] Idle (queue empty) 20s -> sleep"));
        Serial.flush();
        sleepUntilIR1();
        idleTimerActive = false;  // ตื่นแล้วให้ IR1 เริ่มงานใหม่
    }
}
