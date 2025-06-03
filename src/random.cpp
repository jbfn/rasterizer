#include <random>

#include "random.h"
#include "structs.h"

std::array<unsigned int, 10> getRandomSeedData() {
  std::random_device source;
  std::array<unsigned int, 10> data;
  for (auto &elem : data) {
    elem = source();
  }
  return data;
}

double randomNumber() {
  auto seedData = getRandomSeedData();
  std::seed_seq seed(seedData.begin(), seedData.end());
  // Static ensures seed persists between function invocations
  static std::default_random_engine rng(seed);

  std::uniform_real_distribution<double> dist(0.0, 1.0);
  return dist(rng);
}

Vector3 randomVector3(const float &x, const float &y, const float &z) {
  return Vector3(randomNumber() * x, randomNumber() * y, randomNumber() * z);
}
