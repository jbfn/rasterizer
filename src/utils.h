#include "models.h"

// Compute the dot product of two 3D vectors.
float Dot(const float a, const float b);

// Convert a number to a string and front pad it.
std::string ToPaddedString(uint16_t number, uint8_t length, char pad_char);

// Get the maximum and minimum components from a set of 3D vectors.
std::pair<Vector3, Vector3> Bounds(const Vector3 &a, const Vector3 &b,
                                   const Vector3 &c);

// Compute the cross product of AB and AP. This is often used to determine
// which side of a line AB a 2D point P is on (based on the resulting sign),
// hence the name "edge function".
int EdgeFunction(int ax, int ay, int bx, int by, int px, int py);

// Determine whether a point P lies inside a triangle ABC. Checks if the signs
// of each triangle edge crossed with the vector from that edge's head to P
// are the same.
bool PointInsideTriangle(int px, int py, int ax, int ay, int bx, int by, int cx,
                         int cy);
