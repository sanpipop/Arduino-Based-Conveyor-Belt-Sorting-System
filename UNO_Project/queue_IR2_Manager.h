#pragma once
#include "objectManager.h"

// ====== คิวสำหรับ IR2 (ประกาศตอนถึงจุดแยก) ======
const uint8_t QSIZE = 16;                  // เก็บได้มากสุด 16 ชิ้น
Package qBuf[QSIZE];                       // array เก็บ Package
uint8_t qHead = 0, qTail = 0, qCount = 0;  // qHead(ตำแหน่งที่ดึงออก) qtail(ตำแหน่งที่ใส่เข้า)

bool enqueuePkg(const Package& p) {        // ใส่ของเข้าคิว
  if (qCount >= QSIZE) return false;       // คิวเต็มใส่เพิ่มไม่ได้
  qBuf[qTail] = p;
  qTail = (qTail + 1) % QSIZE;  // ขยับตำแหน่งท้ายคิวไปหนึ่งช่อง
  qCount++;
  return true;
}
bool dequeuePkg(Package& out) {  // เอาออกจากคิว
  if (qCount == 0) return false;
  out = qBuf[qHead];
  qHead = (qHead + 1) % QSIZE;
  qCount--;
  return true;
}

// ฟังก์ชัน “แอบดูหัวคิว (peek)” ดูของชิ้นถัดไปที่จะถูกปล่อยจากคิว โดยไม่ดึงออกจากคิว
bool peekPkg(Package& out) {
  if (qCount == 0) return false;  // ถ้าคิวว่าง (ไม่มีอะไรให้ดู)
  out = qBuf[qHead];              // ถ้าคิวมีของ ก็อปปี้สมาชิกตัวหน้า qBuf[qHead] ไปใส่ตัวแปรอ้างอิง out
  return true;
}