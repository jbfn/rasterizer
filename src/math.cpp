#include "math.h"
#include "structs.h"

float Dot(const Vector3 &a, const Vector3 &b) {
  return a[0] * b[0] + a[1] * b[1];
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
