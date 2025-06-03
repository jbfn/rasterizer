#pragma once

#include <iostream>

struct Vector3 {
  float data[3];

  // Constructors
  Vector3() : data{0, 0, 0} {}
  Vector3(float x, float y, float z) : data{x, y, z} {}

  inline float &x() { return data[0]; }
  inline float &y() { return data[1]; }
  inline float &z() { return data[2]; }

  inline float x() const { return data[0]; }
  inline float y() const { return data[1]; }
  inline float z() const { return data[2]; }

  // Operators
  inline float &operator[](size_t i) { return data[i]; }
  inline const float &operator[](size_t i) const { return data[i]; }

  inline Vector3 operator+(const Vector3 &rhs) const {
    return Vector3(data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2]);
  }
  inline void operator+=(const Vector3 &rhs) {
    data[0] += rhs[0];
    data[1] += rhs[1];
    data[2] += rhs[2];
  }
  inline Vector3 operator-(const Vector3 &rhs) const {
    return Vector3(data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2]);
  }
  inline void operator-=(const Vector3 &rhs) {
    data[0] -= rhs[0];
    data[1] -= rhs[1];
    data[2] -= rhs[2];
  }
  inline Vector3 operator*(const float &rhs) const {
    return Vector3(data[0] * rhs, data[1] * rhs, data[2] * rhs);
  }
  inline Vector3 operator/(const float &rhs) const {
    return Vector3(data[0] / rhs, data[1] / rhs, data[2] / rhs);
  }
};

// Vector3 operator<<
inline std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
  return os << "{" << (int)v[0] << ", " << (int)v[1] << ", " << (int)v[2]
            << "}";
}
