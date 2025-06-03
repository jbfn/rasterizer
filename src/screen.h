#pragma once

#include "structs.h"

class Screen {
public:
  Screen(uint16_t width, uint16_t height) : width(width), height(height) {
    data.resize(width * height);
  }

  Vector3 &operator()(uint16_t x, uint16_t y) {
    return data[y * width + x]; // row-major layout
  }

  const Vector3 &operator()(uint16_t x, uint16_t y) const {
    return data[y * width + x];
  }

  uint16_t getWidth() const { return width; }
  uint16_t getHeight() const { return height; }
  uint16_t length() const { return width + height; }

  void reset() { std::fill(data.begin(), data.end(), Vector3()); }

private:
  uint16_t width, height;
  std::vector<Vector3> data;
};

void WriteScreenToFile(const Screen &screen, const std::string &name);
