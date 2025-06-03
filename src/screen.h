#pragma once

#include "structs.h"

class Screen {
public:
  Screen(int width, int height) : width(width), height(height) {
    data.resize(width * height);
  }

  Vector3 &operator()(int x, int y) {
    return data[y * width + x]; // row-major layout
  }
  Vector3 &operator()(int i) {
    return data[i]; // row-major layout
  }

  const Vector3 &operator()(int x, int y) const { return data[y * width + x]; }

  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int length() const { return width + height; }

  void reset() { std::fill(data.begin(), data.end(), Vector3()); }

private:
  int width, height;
  std::vector<Vector3> data;
};

void WriteScreenToFile(const Screen &screen, const std::string &name);
