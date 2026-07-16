#pragma once
#include "pcf8574_read_ir.h"


// ---------- ค่าหน่วง ----------
const unsigned long DEBOUNCE_MS     = 30;
const unsigned long READY_DELAY_MS  = 500;


// ---------- สถานะ IR1..IR8 ----------
bool irPrev[10]       = { HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH };  // มีแค่index 1..8 ใช้จริง
bool irReady[10]      = { true,true,true,true,true,true,true,true,true,true };  // index 0 เป็นแค่ตัวปลอม
unsigned long irLastMs[10]      = { 0 };
unsigned long irReleasedAt[10]  = { 0 };

void setup_all_ir_pin(){
    // เริ่มระบบ IR (I2C + PCF + IR1 Arduino)
    init_ir_on_pcf();

    // อ่านค่าเริ่มต้นของ IR1..IR8 (HIGH = ว่าง, LOW = มีวัตถุ)
    for (uint8_t ir = 1; ir <= 9; ir++) {
        irPrev[ir] = read_ir_on_pcf(ir);
    }
}