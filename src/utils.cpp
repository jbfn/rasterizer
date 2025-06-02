#include <iomanip>
#include <sstream>
#include <string>

#include "models.h"
#include "utils.h"

float Dot(const Vector3 &a, const Vector3 &b) {
  return a[0] * b[0] + a[1] * b[1];
}

std::string ToPaddedString(const uint16_t number, const uint8_t length,
                           char pad_char) {
  std::ostringstream oss;
  oss << std::setw(length) << std::setfill(pad_char) << number;
  return oss.str();
}

int EdgeFunction(int ax, int ay, int bx, int by, int px, int py) {
  return (px - ax) * (by - ay) - (py - ay) * (bx - ax);
}

bool PointInsideTriangle(int px, int py, int ax, int ay, int bx, int by, int cx,
                         int cy) {
  int w0 = EdgeFunction(ax, ay, bx, by, px, py);
  int w1 = EdgeFunction(bx, by, cx, cy, px, py);
  int w2 = EdgeFunction(cx, cy, ax, ay, px, py);
  return (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
}
