#pragma once
#include "ir_StateManager.h"
#include "laneManager.h"
#include "queue_IR2_Manager.h"
#include "thaiColorForSerial.h"
#include "sleep.h"
#include "objectManager.h"
#include "servo_control.h"
#include "motor_control.h"
#include "pcf8574_read_ir.h"

bool ir2_send = false;
int lane;

void ir2_workflow_pooling() {
  // -------- IR2 (คงแบบ polling เดิม) --------
  bool ir2 = read_ir_on_pcf(2);  // LOW = เจอวัตถุ
  bool ir2Fall = (irPrev[2] == HIGH && ir2 == LOW && (millis() - irLastMs[2]) > DEBOUNCE_MS);
  bool ir2Rise = (irPrev[2] == LOW && ir2 == HIGH && (millis() - irLastMs[2]) > DEBOUNCE_MS);

  // IR2: เข้า -> ประกาศแพ็คเกจ "ตัวถัดไปในคิว"
  if (ir2Fall && irReady[2]) {
    irLastMs[2] = millis();
    irReady[2] = false;

    Package head;  // ดึงข้อมูลหัวคิว แต่ไม่ดึงออกจากคิวก่อน
    if (!peekPkg(head)) {
      Serial.println(F("!! IR2 ตรวจเจอของ แต่คิวว่าง"));
    } else {

      // ทางสำหรับ "กล่องยับ" -> ส่งลงถังขยะทันที ไม่ต้องรอ IR ปลายทาง
      if (head.damaged || head.weight == INVALID) {
        Package pkgToSend;
        dequeuePkg(pkgToSend);  // ดึงออกจริง

        send_to_trash();

        Serial.print(F("สรุป: กล่องไม่ยับ="));
        Serial.print(intact_boxes);
        Serial.print(F(" | กล่องยับ="));
        Serial.println(damaged_boxes);

        // ไม่แตะตัวแปรเลน light/heavy ใดๆ ทั้งสิ้น (ไม่มี gate/ไม่มี busy สำหรับเลนยับ)
        // ถ้าคิวว่างและ IR1 ว่าง จะเริ่ม จับเวลารอนอนได้เลย
        if (qCount == 0 && irPrev[1] == HIGH) {
          idleTimerActive = true;
          lastNoQueueMs = millis();
        }
      } else {
        // ของปกติ: ไปเลน light/heavy
        lane = laneFor(head);
        if (!laneBusy[lane]) {
          // เลนนี้ว่าง -> dequeue แล้วปล่อย
          Package pkgToSend;
          dequeuePkg(pkgToSend);  // ดึงออกจากคิวจริง

          Serial.print(F("มีกล่อง "));
          Serial.print(weightName(pkgToSend.weight));
          Serial.print(F(" สี"));
          Serial.print(colorName(pkgToSend.color));
          Serial.println(F(" มาที่จุดแยกแล้ว"));
          Serial.print(F("เตรียมส่งไปที่สายสำหรับของ "));
          Serial.println(weightName(pkgToSend.weight));


          // --- เปิดประตูเฉพาะเลนที่เกี่ยวข้อง + mark busy ---
          int expectIdx = expectedIRIndexFor(pkgToSend);  // IR ตัวไหนต้องรับของชิ้นนี้
          if (lane == LANE_LIGHT) {
            lastLightDispatched = pkgToSend;
            lightLaneHasPackage = true;
            lightLaneTargetIdx = expectIdx;  // 3-5
            lightLaneGateOpen = true;
            laneBusy[LANE_LIGHT] = true;
            send_to_light_lane();
            ir2_send = true;
          } else {
            lastHeavyDispatched = pkgToSend;
            heavyLaneHasPackage = true;
            heavyLaneTargetIdx = expectIdx;  // 6-8
            heavyLaneGateOpen = true;
            laneBusy[LANE_HEAVY] = true;
            send_to_heavy_lane();
            ir2_send = true;
          }

          // (ถ้าคิวว่างและคุณมี idle-timer ตามที่คุยไว้ ก็เริ่มจับเวลาได้ตรงนี้)
          if (qCount == 0) {
            idleTimerActive = true;
            lastNoQueueMs = millis();
          }
        } else {
          // เลนนี้ไม่ว่าง -> ยังไม่ปล่อย (ปล่อยเมื่อ IR ปลายทางของเลนนั้น detect เสร็จ)
          Serial.println(F("เลนปลายทางไม่ว่าง รอเลนเคลียร์ก่อน"));
        }
      }
    }
  }

  // IR2: ออก -> เริ่มจับเวลา re-arm
  if (ir2Rise && ir2_send) {
    irLastMs[2] = millis();
    irReleasedAt[2] = millis();
    ir2_send = false;
    Serial.println(F("จัดส่งเรียบร้อย"));
  }

  // IR2: re-arm หลังออก 0.5 วินาที
  if (!irReady[2] && ir2 == HIGH && (millis() - irReleasedAt[2]) >= READY_DELAY_MS) {
    irReady[2] = true;
  }
  irPrev[2] = ir2;
}