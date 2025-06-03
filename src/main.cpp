#include <immintrin.h> // AVX
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
const int TRIANGLE_COUNT = 1000;
const float TRIANGLE_SIZE = 0.5;
const float TRIANGLE_SPEED = 5;
const int INTS_PER_REGISTER = 8;
std::string OUTPUT_DIR = "frames/";

// Randomly generate triangle vertices, vertex velocities, and triangle colors.
std::tuple<std::vector<Vector3>, std::vector<Vector3>, std::vector<Vector3>>
CreateRunTemplate() {
  std::vector<Vector3> points(TRIANGLE_COUNT * 3);
  std::vector<Vector3> velocities(points.size());
  std::vector<Vector3> colours(points.size());

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
  for (size_t i = 0; i < colours.size(); i+=3) {
    Vector3 randomColor = randomVector3(1.0,1.0,1.0);
    colours[i] = randomColor;
    colours[i+1] = randomColor;
    colours[i+2] = randomColor;
  }

  return {points, velocities, colours};
}

// Populate a screen with pixel colors from a list of triangle vertices and
// colors per triangle.
void Render(std::vector<Vector3> &points, std::vector<Vector3> &colours,
            Screen &screen) {
  __m256i mm_offsets = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);

  for (size_t i = 0; i < points.size(); i += 3) {
    Vector3 a = points[i];
    Vector3 b = points[i + 1];
    Vector3 c = points[i + 2];

    // triangle vertices
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
    int rowStartX = minY * IMAGE_WIDTH + minX;
    int rowEndX = rowStartX + maxX - minX;

    // change in E per increase in x or y pixel
    int AB_dx = (by - ay);
    int AB_dy = (ax - bx);
    int BC_dx = (cy - by);
    int BC_dy = (bx - cx);
    int CA_dx = (ay - cy);
    int CA_dy = (cx - ax);

    // change in E per <pixel chunk size>
    int AB_dx_chunk = (by - ay) * INTS_PER_REGISTER;
    int BC_dx_chunk = (cy - by) * INTS_PER_REGISTER;
    int CA_dx_chunk = (ay - cy) * INTS_PER_REGISTER;

    // broadcasted edge deltas per unit change in X, i.e. [dx, dx, dx, ...]
    __m256i mm_AB_dx = _mm256_set1_epi32(AB_dx);
    __m256i mm_BC_dx = _mm256_set1_epi32(BC_dx);
    __m256i mm_CA_dx = _mm256_set1_epi32(CA_dx);

    // edge deltas per unit change in (X * offset), i.e. [dx, 2dx, 3dx, ...]
    __m256i mm_AB_dx_offsets = _mm256_mullo_epi32(mm_AB_dx, mm_offsets);
    __m256i mm_BC_dx_offsets = _mm256_mullo_epi32(mm_BC_dx, mm_offsets);
    __m256i mm_CA_dx_offsets = _mm256_mullo_epi32(mm_CA_dx, mm_offsets); 

    // row-start edge functions ("E"s)
    int E_AB = (minX - ax) * AB_dx + (minY - ay) * AB_dy;
    int E_BC = (minX - bx) * BC_dx + (minY - by) * BC_dy;
    int E_CA = (minX - cx) * CA_dx + (minY - cy) * CA_dy;
    int E_ABx0 = E_AB;
    int E_BCx0 = E_BC;
    int E_CAx0 = E_CA;

    // loop variables
    __m256i mm_E_AB;
    __m256i mm_E_BC;
    __m256i mm_E_CA;
    __m256i mm_broadcast_tmp;
    __m256i XOR1;
    __m256i XOR2;
    __m256i OR;
    __m256i mask;

    for (int y = minY; y <= maxY; y ++){
      for (int x = rowStartX; x <= rowEndX; x += INTS_PER_REGISTER) {
	// current E values
	mm_broadcast_tmp = _mm256_set1_epi32(E_AB); 
	mm_E_AB = _mm256_add_epi32(mm_broadcast_tmp, mm_AB_dx_offsets); 
	mm_broadcast_tmp = _mm256_set1_epi32(E_BC);
	mm_E_BC = _mm256_add_epi32(mm_broadcast_tmp, mm_BC_dx_offsets);
	mm_broadcast_tmp = _mm256_set1_epi32(E_CA);
	mm_E_CA = _mm256_add_epi32(mm_broadcast_tmp, mm_CA_dx_offsets);

	// check if pixels are inside triangle (using signs of edge functions)
	// ((A ^ B) | (B ^ C) >= 0
	// is the same as
	// (A >= 0 && B >= 0 && C >=0) | (A <= 0 && B <= 0 && C <=0) |
	XOR1 = _mm256_xor_si256(mm_E_AB, mm_E_BC);
	XOR2 = _mm256_xor_si256(mm_E_BC, mm_E_CA);
	OR = _mm256_or_si256(XOR1, XOR2);

	// get and apply 8-bit mask of colored pixels
	mask = _mm256_cmpgt_epi32(OR, _mm256_setzero_si256());
	int mask_i32 = _mm256_movemask_ps(_mm256_castsi256_ps(mask));
	while (mask_i32 != 0) {
	  int bit = __builtin_ctz(mask_i32);   // index of lowest set bit
	  mask_i32 &= ~(1 << bit);             // clear that bit
	  screen(x + bit) = colours[i];
	}

	// advance E values for next chunk
	E_AB += AB_dx_chunk;
	E_BC += BC_dx_chunk;
	E_CA += CA_dx_chunk;
      }
      // set screen index for next iteration
      rowStartX += IMAGE_WIDTH;
      rowEndX += IMAGE_WIDTH;

      // reset E values to beginning of next row
      E_AB = E_ABx0 += AB_dy;
      E_BC = E_BCx0 += BC_dy;
      E_CA = E_CAx0 += CA_dy;
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
static double CreateTriangleAnimation() {
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
    std::string filepath = OUTPUT_DIR + name;
    WriteScreenToFile(screen, filepath);
    screen.reset();

    UpdatePointPositions(points, vels);
  }

  auto end = steady_clock::now();
  auto duration = duration_cast<milliseconds>(end - start).count();
  return (double)duration;
}

int main(void) {
  std::cout << "Creating triangle frames" << std::endl;
  double ms = CreateTriangleAnimation();

  double pps = IMAGE_WIDTH * IMAGE_HEIGHT * FRAMES / ms * 1000;
  std::cout << "Pixels per second: " << pps << std::endl;

  return EXIT_SUCCESS;
}
