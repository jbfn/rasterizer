#include <chrono>
#include <iostream>
#include <vector>

#include "image.h"
#include "models.h"
#include "random.h"
#include "utils.h"

const int IMAGE_WIDTH = 320;
const int IMAGE_HEIGHT = 240;
const int FRAMES = 50;
const int TRIANGLE_COUNT = 100;
const float TRIANGLE_SIZE = 0.5;
const float TRIANGLE_SPEED = 5;

// Randomly generate triangle vertices, vertex velocities, and triangle colors.
std::tuple<std::vector<Vector3>, std::vector<Vector3>, std::vector<Vector3>>
CreateRunTemplate() {
  std::vector<Vector3> points(TRIANGLE_COUNT * 3);
  std::vector<Vector3> velocities(points.size());
  std::vector<Vector3> colours(TRIANGLE_COUNT);

  // Generate points
  for (int i = 0; i < points.size(); i++) {
    points[i] = randomVector3(IMAGE_WIDTH * TRIANGLE_SIZE,
                              IMAGE_HEIGHT * TRIANGLE_SIZE, 0);

    if (i % 3 == 2) {
      Vector3 offset = Vector3();
      offset.x = randomNumber() * IMAGE_WIDTH * (1.0 - TRIANGLE_SIZE);
      offset.y = randomNumber() * IMAGE_HEIGHT * (1.0 - TRIANGLE_SIZE);
      points[i] = points[i] + offset;
      points[i - 1] = points[i - 1] + offset;
      points[i - 2] = points[i - 2] + offset;
    }
  }

  // Generate velocities
  for (int i = 0; i < velocities.size(); i += 3) {
    Vector3 velocity = randomVector3(TRIANGLE_SPEED, TRIANGLE_SPEED, 0);
    velocity.x = velocity.x * (std::round(randomNumber()) == 0 ? 1 : -1);
    velocity.y = velocity.y * (std::round(randomNumber()) == 0 ? 1 : -1);
    velocities[i] = velocity;
    velocities[i + 1] = velocity;
    velocities[i + 2] = velocity;
  }

  // Generate colours
  for (int i = 0; i < colours.size(); i++) {
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

    // Colour triangle pixels
    auto [min, max] = Bounds(a, b, c);
    for (int y = min.y; y < max.y; y++) {
      for (int x = min.x; x < max.x; x++) {
        Vector3 pixel = Vector3(x, y, 0);
        if (!pixel.is_inside(a, b, c)) {
          continue;
        }
        image(x, y) = colours[i / 3];
      }
    }
  }
}

// Move triangle vertices one frame forward according to their velocity. If a
// vertex will go out of the image boundaries, its velocity is reversed.
void UpdatePointPositions(std::vector<Vector3> &points,
                          std::vector<Vector3> &vels) {
  for (int i = 0; i < points.size(); i++) {
    Vector3 &point = points[i];
    Vector3 &vel = vels[i];
    point += vel;
    if (point.x > IMAGE_WIDTH) {
      point.x = IMAGE_WIDTH - (point.x - IMAGE_WIDTH);
      vel.x *= -1;
    } else if (point.x < 0) {
      point.x = std::abs(point.x);
      vel.x *= -1;
    };

    if (point.y > IMAGE_HEIGHT) {
      point.y = IMAGE_HEIGHT - (point.y - IMAGE_HEIGHT);
      vel.y *= -1;
    } else if (point.y < 0) {
      point.y = std::abs(point.y);
      vel.y *= -1;
    }
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
    auto start = steady_clock::now();

    Render(points, colours, image);

    auto end = steady_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    std::cout << "Generated frame " << frame << " in " << duration << "ms"
              << std::endl;

    std::string name = pad0sFront(frame, 4);
    WriteImageToFile(image, name);
    image.reset();

    UpdatePointPositions(points, vels);
  }
}

int main(void) {
  std::cout << "Creating triangle frames" << std::endl;
  CreateTriangleAnimation();

  return EXIT_SUCCESS;
}
