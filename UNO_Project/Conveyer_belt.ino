#include "work_for_isr_IR1.h"
#include "work_for_pooling_IR2.h"
#include "work_for_pooling_IR3to8.h"
#include "ir_StateManager.h"
#include "sleep.h"  
#include "check_color.h"
#include "servo_control.h"
#include "motor_control.h"
#include "pcf8574_read_ir.h"
#include "box_damaged_from_odroid.h"


void setup() {
  Serial.begin(115200);
  setup_all_ir_pin();
  initMotors();

  init_color_sensor(); // เปิด sensor สี
  init_weight_sensor(); // เปิด sensor weight

  setup_signal_from_odroid();
  // ต่อ Servo แต่ละตัวกับขา
  initServo(0, 11);   // Servo ตัวที่ 0 ที่ขา 11 ส้มเบา
  initServo(1, 12);   // Servo ตัวที่ 1 ที่ขา 12 เขียวเบา
  initServo(2, 9);   // Servo ตัวที่ 2 ที่ขา 9 ส้มหนัก
  initServo(3, 10);  // Servo ตัวที่ 3 ที่ขา 10 เขียวหนัก
  initServo(4, 3);  // Servo ตัวแกนหมุน

  randomSeed(analogRead(A0));

  // อ่านสถานะเริ่มต้น แล้วผูก interrupt ให้ IR1
  irPrev[1] = read_ir_on_pcf(1);
  attachInterrupt(digitalPinToInterrupt(IR1_PIN_ARDUINO), ir1_isr, CHANGE);

  Serial.println(F("=== Setup IR1-IR8 success!! ==="));
}

void loop() {
  ir1_workflow_isr();
  ir2_workflow_pooling();
  ir3to8_workflow_pooling();

  check_sleep_condition();
  
}

