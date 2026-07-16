#pragma once
#include "objectStruct.h"

// ====== ช่วยแปลงชื่อสำหรับโชวใน serial เฉยๆ ======
const char* colorName(Color c) {
  switch (c) {
    case ORANGE: return "ส้ม";
    case YELLOW: return "เหลือง";
    case GREEN: return "เขียว";
  }
  return "?";
}
const char* weightName(Weight w) {
  return (w == HEAVY) ? "heavy" : "light";
}