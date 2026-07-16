#pragma once

// ====== Object ======
enum Color { ORANGE,
             YELLOW,
             GREEN };
enum Weight { HEAVY,
              LIGHT,
              INVALID };

struct Package {
  Color color;
  Weight weight;
  bool damaged;
};