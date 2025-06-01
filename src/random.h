#include <cstdio>
#include <random>

#include "models.h"

// Generate random seed data for an RNG
std::array<unsigned int, 10> getRandomSeedData();

// Generate a random number between 0 and 1
double randomNumber();

Vector3 randomVector3(const float &x, const float &y, const float &z);
