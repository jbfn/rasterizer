#include "models.h"
#include "utils.h"

bool Vector3::is_inside(const Vector3 &a, const Vector3 &b,
                        const Vector3 &c) const {
  return PointInsideTriangle(*this, a, b, c);
}

std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
  return os << "{" << v.x << ", " << v.y << ", " << v.z << "}";
}
