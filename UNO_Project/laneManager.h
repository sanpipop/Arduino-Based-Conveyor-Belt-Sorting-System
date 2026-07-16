#pragma once
#include "objectManager.h"

// --- lane state ---
enum { LANE_LIGHT = 0,
       LANE_HEAVY = 1 };
bool laneBusy[2] = { false, false };

// light lane gate
bool lightLaneGateOpen = false;    // เลน light พร้อมเปิดรับของมา detect ไหม
bool lightLaneHasPackage = false;  // เลน light กำลังจัดส่ง
int lightLaneTargetIdx = -1;       // กำหนด IR ในเลน light ที่จะรับชิ้นนี้ 0..2 (IR3..IR5)
Package lastLightDispatched;       // object ที่เก็บข้อมูลของชิ้นที่เพิ่งปล่อยล่าสุดในเลน light (สี/น้ำหนัก)

// heavy lane gate
bool heavyLaneGateOpen = false;    // เลน heavy พร้อมเปิดรับของมา detect ไหม
bool heavyLaneHasPackage = false;  // เลน heavy กำลังจัดส่ง
int heavyLaneTargetIdx = -1;       // กำหนด IR ในเลน heavy ที่จะรับชิ้นนี้ 3..5 (IR6..IR8)
Package lastHeavyDispatched;       // object ที่เก็บข้อมูลของชิ้นที่เพิ่งปล่อยล่าสุดในเลน heavy (สี/น้ำหนัก)

// เช็คว่าจะให้ไปเลน light หรือ heavy
int laneFor(const Package& p) {
  return (p.weight == LIGHT) ? LANE_LIGHT : LANE_HEAVY;
}
// เช็คว่ากล่องนี้จะให้ IR ตัวไหน detect
int expectedIRIndexFor(const Package& p) {  // 3-8 = IR3..IR8
  int colorIdx = (p.color == ORANGE) ? 3 : (p.color == GREEN ? 4 : 5);
  return (p.weight == LIGHT) ? colorIdx : (3 + colorIdx);
}