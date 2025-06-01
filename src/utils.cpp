#include <iomanip>
#include <sstream>
#include <string>

#include "models.h"
#include "utils.h"

float Dot(const Vector3 &a, const Vector3 &b) { return a.x * b.x + a.y * b.y; }

Vector3 Rotate90XY(const Vector3 &vector) {
  return Vector3(vector.y, -vector.x, vector.z);
}

bool PointOnRightSide(const Vector3 &point, const Vector3 &a,
                      const Vector3 &b) {
  Vector3 ap = point - a;
  Vector3 abPerpendicular = Rotate90XY(b - a);
  return Dot(ap, abPerpendicular) >= 0;
}

bool PointInsideTriangle(const Vector3 &point, const Vector3 &a,
                         const Vector3 &b, const Vector3 &c) {
  bool ab = PointOnRightSide(point, a, b);
  bool bc = PointOnRightSide(point, b, c);
  bool ca = PointOnRightSide(point, c, a);
  return ab == bc && bc == ca;
}

std::string pad0sFront(const uint16_t number, const uint8_t length) {
  std::ostringstream oss;
  oss << std::setw(length) << std::setfill('0') << number;
  return oss.str();
}

std::pair<Vector3, Vector3> Bounds(const Vector3 &a, const Vector3 &b,
                                   const Vector3 &c) {
  Vector3 min = Vector3();
  Vector3 max = Vector3();
  min.x = std::min(std::min(a.x, b.x), c.x);
  min.y = std::min(std::min(a.y, b.y), c.y);
  min.z = std::min(std::min(a.z, b.z), c.z);
  max.x = std::max(std::max(a.x, b.x), c.x);
  max.y = std::max(std::max(a.y, b.y), c.y);
  max.z = std::max(std::max(a.z, b.z), c.z);
  return {min, max};
}
