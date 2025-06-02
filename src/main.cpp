#include <iostream>
#include <vector>

#include "image.h"
#include "models.h"
#include "random.h"
#include "utils.h"

const int IMAGE_WIDTH = 800;
const int IMAGE_HEIGHT = 600;
const int FRAMES = 1000;
const int TRIANGLE_COUNT = 200;
const float TRIANGLE_SIZE = 0.5;
const float TRIANGLE_SPEED = 5;

// Randomly generate triangle vertices, vertex velocities, and triangle colors.
std::tuple<std::vector<Vector3>, std::vector<Vector3>, std::vector<Vector3>>
CreateRunTemplate() {
  std::vector<Vector3> points(TRIANGLE_COUNT * 3);
  std::vector<Vector3> velocities(points.size());
  std::vector<Vector3> colours(TRIANGLE_COUNT);

  // Generate points
  for (size_t i = 0; i < points.size(); i++) {
    points[i] = randomVector3(IMAGE_WIDTH * TRIANGLE_SIZE,
                              IMAGE_HEIGHT * TRIANGLE_SIZE, 0);

    if (i % 3 == 2) {
      Vector3 offset = Vector3();
      offset[0] = randomNumber() * IMAGE_WIDTH * (1.0 - TRIANGLE_SIZE);
      offset[1] = randomNumber() * IMAGE_HEIGHT * (1.0 - TRIANGLE_SIZE);
      points[i] = points[i] + offset;
      points[i - 1] = points[i - 1] + offset;
      points[i - 2] = points[i - 2] + offset;
    }
  }

  // Generate velocities
  for (size_t i = 0; i < velocities.size(); i += 3) {
    Vector3 velocity = randomVector3(TRIANGLE_SPEED, TRIANGLE_SPEED, 0);
    velocity[0] = velocity[0] * (std::round(randomNumber()) == 0 ? 1 : -1);
    velocity[1] = velocity[1] * (std::round(randomNumber()) == 0 ? 1 : -1);
    velocities[i] = velocity;
    velocities[i + 1] = velocity;
    velocities[i + 2] = velocity;
  }

  // Generate colours
  for (size_t i = 0; i < colours.size(); i++) {
    colours[i] = randomVector3(1.0, 1.0, 1.0);
  }

  return {points, velocities, colours};
}

// Populate an image with pixel colors from a list of triangle vertices and
// colors per triangle.
void Render(std::vector<Vector3> &points, std::vector<Vector3> &colours,
            Image &image) {
  for (int i = 0; i < points.size(); i += 3) {
    Vector3 a = points[i];
    Vector3 b = points[i + 1];
    Vector3 c = points[i + 2];
    // std::cout << "triangle " << a << b << c << std::endl;

    int ax = static_cast<int>(a[0]);
    int ay = static_cast<int>(a[1]);
    int bx = static_cast<int>(b[0]);
    int by = static_cast<int>(b[1]);
    int cx = static_cast<int>(c[0]);
    int cy = static_cast<int>(c[1]);

    // bounding box
    int minX = std::min(std::min(ax, bx), cx);
    int maxX = std::max(std::max(ax, bx), cx);
    int minY = std::min(std::min(ay, by), cy);
    int maxY = std::max(std::max(ay, by), cy);

    // change in edge function per change in x or y pixel
    int deltaXABAP = (by - ay);
    int deltaYABAP = (ax - bx);
    int deltaXBCBP = (cy - by);
    int deltaYBCBP = (bx - cx);
    int deltaXCACP = (ay - cy);
    int deltaYCACP = (cx - ax);

    // signed areas of top-left pixel (-1x) crossed with triangle edges
    int areaABAP = EdgeFunction(ax, ay, bx, by, minX - 1, minY);
    int areaBCBP = EdgeFunction(bx, by, cx, cy, minX - 1, minY);
    int areaCACP = EdgeFunction(cx, cy, ax, ay, minX - 1, minY);

    auto sameSign = [](int i1, int i2, int i3) {
      bool sign =
          (i1 >= 0 && i2 >= 0 && i3 >= 0) || (i1 <= 0 && i2 <= 0 && i3 <= 0);
      // std::cout << "sign " << sign << std::endl;
      return sign;
    };

    // snake down through pixels in image
    bool flipX = minY % 2 == 1;
    for (int y = minY; y < maxY; y++) {
      if (flipX ^ (y % 2 == 0)) {
        for (int x = minX; x < maxX; x++) {
          areaABAP += deltaXABAP;
          areaBCBP += deltaXBCBP;
          areaCACP += deltaXCACP;
          // std::cout << "edges " << x << " " << y << " " << areaABAP << " "
          //          << areaBCBP << " " << areaCACP << std::endl;
          if (sameSign(areaABAP, areaBCBP, areaCACP)) {
            // std::cout << "filling " << x << " " << y << std::endl;
            image(x, y) = colours[i / 3];
          }
        }
      } else {
        for (int x = maxX - 1; x >= minX; x--) {
          if (sameSign(areaABAP, areaBCBP, areaCACP)) {
            image(x, y) = colours[i / 3];
          }
          areaABAP -= deltaXABAP;
          areaBCBP -= deltaXBCBP;
          areaCACP -= deltaXCACP;
          // std::cout << "edges " << x << " " << y << " " << areaABAP << " "
          //           << areaBCBP << " " << areaCACP << std::endl;
        }
      }
      areaABAP += deltaYABAP;
      areaBCBP += deltaYBCBP;
      areaCACP += deltaYCACP;
    }
  }
}

// Move triangle vertices one frame forward according to their velocity. If a
// vertex will go out of the image boundaries, its velocity is reversed.
void UpdatePointPositions(std::vector<Vector3> &points,
                          std::vector<Vector3> &vels) {
  auto bounce = [](float &pos, float &vel, float max) {
    if (pos > max) {
      pos = max - (pos - max);
      vel *= -1;
    } else if (pos < 0) {
      pos = -pos;
      vel *= -1;
    }
  };

  for (size_t i = 0; i < points.size(); ++i) {
    points[i] += vels[i];
    bounce(points[i][0], vels[i][0], IMAGE_WIDTH);
    bounce(points[i][1], vels[i][1], IMAGE_HEIGHT);
  }
}

// Generate a series of BMP images that depict triangles bouncing around the
// screen.
static void CreateTriangleAnimation() {
  Image image(IMAGE_WIDTH, IMAGE_HEIGHT);
  image.reset();
  auto [points, vels, colours] = CreateRunTemplate();

  using namespace std::chrono;

  for (int frame = 0; frame < FRAMES; frame++) {
    Render(points, colours, image);

    WriteImageToFile(image, ToPaddedString(frame, 4, '0'));
    image.reset();

    UpdatePointPositions(points, vels);
  }
}

int main(void) {
  std::cout << "Creating triangle frames" << std::endl;
  CreateTriangleAnimation();

  return EXIT_SUCCESS;
}
