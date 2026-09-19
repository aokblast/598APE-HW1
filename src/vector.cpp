#include<string.h>
#include<stdio.h>
#include<limits>
#include<math.h>
#include<stdlib.h>
//#include <printf.h>
#include <stddef.h>
#include "vector.h"
#include <cassert>

// We assume that v1, v2, and v3 are orthonormal.
Vector solveScalers(Vector v1, Vector v2, Vector v3, Vector C){
  constexpr double THRESHOLD = 1e-9;
  assert(std::fabs(v1.dot(v2)) <= THRESHOLD && std::fabs(v2.dot(v3)) <= THRESHOLD &&
         std::fabs(v3.dot(v1)) <= THRESHOLD);
  return Vector(C.dot(v1), C.dot(v2), C.dot(v3));
}

Ray::Ray(const Vector& po, const Vector& ve): point(po), vector(ve){}
