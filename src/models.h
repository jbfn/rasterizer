#pragma once

#include <iostream>

struct Vector3 {
  float x, y, z;

  // Constructors
  Vector3() : x(0), y(0), z(0) {}
  Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

  // Helpers
  float r() const { return x; }
  void set_r(float value) { x = value; }

  float g() const { return y; }
  void set_g(float value) { y = value; }

  float b() const { return z; }
  void set_b(float value) { z = value; }

  bool is_inside(const Vector3 &a, const Vector3 &b, const Vector3 &c) const;

  // Operators
  Vector3 operator+(const Vector3 &vector2) const {
    return Vector3(vector2.x + this->x, vector2.y + this->y,
                   vector2.z + this->z);
  }
  void operator+=(const Vector3 &vector2) {
    this->x = this->x + vector2.x;
    this->y = this->y + vector2.y;
    this->z = this->z + vector2.z;
  }

  Vector3 operator-(const Vector3 &vector2) const {
    return Vector3(vector2.x - this->x, vector2.y - this->y,
                   vector2.z - this->z);
  }

  void operator-=(const Vector3 &vector2) {
    this->x = this->x - vector2.x;
    this->y = this->y - vector2.y;
    this->z = this->z - vector2.z;
  }

  Vector3 operator*(const float &multiplier) const {
    return Vector3(this->x * multiplier, this->y * multiplier,
                   this->z * multiplier);
  }

  Vector3 operator/(const float &divisor) const {
    return Vector3(this->x / divisor, this->y / divisor, this->z / divisor);
  }
};

// Vector3 operator<<
std::ostream &operator<<(std::ostream &os, const Vector3 &v);
