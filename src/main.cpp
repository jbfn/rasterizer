#include <iostream>
#include <vector>

#include "math.h"
#include "random.h"
#include "screen.h"
#include "structs.h"
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

// Populate a screen with pixel colors from a list of triangle vertices and
// colors per triangle.
void Render(std::vector<Vector3> &points, std::vector<Vector3> &colours,
            Screen &screen) {
  for (int i = 0; i < points.size(); i += 3) {
    Vector3 a = points[i];
    Vector3 b = points[i + 1];
    Vector3 c = points[i + 2];

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

    // change in edge function (area) per increase in x or y pixel
    int dx_AB = (by - ay);
    int dy_AB = (ax - bx);
    int dx_BC = (cy - by);
    int dy_BC = (bx - cx);
    int dx_CA = (ay - cy);
    int dy_CA = (cx - ax);

    // signed areas of top-left pixel crossed with triangle edges
    int areaABx0 = EdgeFunction(ax, ay, bx, by, minX, minY);
    int areaBCx0 = EdgeFunction(bx, by, cx, cy, minX, minY);
    int areaCAx0 = EdgeFunction(cx, cy, ax, ay, minX, minY);
    int areaAB = areaABx0;
    int areaBC = areaBCx0;
    int areaCA = areaCAx0;

    for (int y = minY; y < maxY; y++) {
      for (int x = minX; x < maxX; x++) {
        if ((areaAB >= 0 && areaBC >= 0 && areaCA >= 0) ||
            (areaAB <= 0 && areaBC <= 0 && areaCA <= 0)) {
          screen(x, y) = colours[i / 3];
        }
        areaAB += dx_AB;
        areaBC += dx_BC;
        areaCA += dx_CA;
      }
      areaAB = areaABx0 += dy_AB;
      areaBC = areaBCx0 += dy_BC;
      areaCA = areaCAx0 += dy_CA;
    }
  }
}

// Move triangle vertices one frame forward according to their velocity. If a
// vertex will go out of the screen boundaries, its velocity is reversed.
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
static long CreateTriangleAnimation() {
  Screen screen(IMAGE_WIDTH, IMAGE_HEIGHT);
  screen.reset();
  auto [points, vels, colours] = CreateRunTemplate();

  using namespace std::chrono;

  auto start = steady_clock::now();

  for (int frame = 0; frame < FRAMES; frame++) {
    if (frame % 50 == 0) {
      std::cout << "Created " << frame << " frames" << std::endl;
    }
    Render(points, colours, screen);

    std::string name = ToPaddedString(frame, 4, '0');
    WriteScreenToFile(screen, name);
    screen.reset();

    UpdatePointPositions(points, vels);
  }

  auto end = steady_clock::now();
  auto duration = duration_cast<milliseconds>(end - start).count();
  return (long)duration;
}

int main(void) {
  std::cout << "Creating triangle frames" << std::endl;
  long ms = CreateTriangleAnimation();

  long pps = IMAGE_WIDTH * IMAGE_HEIGHT * FRAMES / ms * 1000;
  std::cout << "Pixels per second: " << pps << std::endl;

  return EXIT_SUCCESS;
}
