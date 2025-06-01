#include "models.h"

// Return the dot product of two vectors on the XY plane
float Dot(const float a, const float b);

// Rotate a vector 90 degress clockwise on the XY plane
Vector3 Rotate90XY(const Vector3 &vector);

// Return true if a point is on the right side of a directed line a -> b on the
// XY plane
bool PointOnRightSide(const Vector3 &point, const Vector3 &a, const Vector3 &b);

// Return a bool indicating if a point is within the bounds of the triangle
// created by 3 other points on the XY plane
bool PointInsideTriangle(const Vector3 &point, const Vector3 &a,
                         const Vector3 &b, const Vector3 &c);

// Convert an number to a string and pad it with leading 0s
std::string pad0sFront(uint16_t number, uint8_t length);

// Get the bounds of the triangle formed by 3 points
std::pair<Vector3, Vector3> Bounds(const Vector3 &a, const Vector3 &b,
                                   const Vector3 &c);
