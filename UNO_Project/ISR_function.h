#pragma once
#include "pcf8574_read_ir.h"

// ====== เพิ่ม: ส่วนของ Interrupt สำหรับ IR1 ======
volatile bool ir1_state_isr = HIGH;            // สถานะล่าสุดของขา IR1 ที่อ่านใน ISR
volatile unsigned long ir1_event_time_us = 0;  // เวลาเกิดเหตุการณ์ (ไมโครวินาที)
volatile bool ir1_event_pending = false;       // มีเหตุการณ์ IR1 ค้างรอประมวลผล


void ir1_isr() {
  ir1_state_isr = read_ir_on_pcf(1);  // อ่านสถานะขาทันที
  ir1_event_time_us = micros();          // timestamp (ไม่ใช้ millis ใน ISR)
  ir1_event_pending = true;              // แค่ตั้งธง ห้าม Serial/ดีเลย์ใน ISR             
}