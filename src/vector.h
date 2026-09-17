#ifndef __VECTOR_H_INCLUDED__
#define __VECTOR_H_INCLUDED__
#define _USE_MATH_DEFINES
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<cmath>
#include<limits>
#define inf std::numeric_limits<double>::infinity()

class Vector{
public:
  double x, y, z;
  Vector(double a, double b, double c) : x(a), y(b), z(c) {}
  inline void operator-=(const Vector rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
  }
  inline void operator+=(const Vector rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
  }
  inline void operator*=(const double rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }
  inline void operator*=(const float rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }
  inline void operator*=(const int rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
  }
  inline void operator/=(const double rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }
  inline void operator/=(const float rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }
  inline void operator/=(const int rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
  }

  inline Vector operator-(const Vector rhs) const {
    return Vector(x - rhs.x, y - rhs.y, z - rhs.z);
  }
  inline  Vector operator+(const Vector rhs) const {
    return Vector(x + rhs.x, y + rhs.y, z + rhs.z);
  }
  /*
  Vector Vector::operator * (const Vector a) {
     return Vector(y*a.z-z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
  }*/
  inline Vector operator*(const double rhs) const {
    return Vector(x * rhs, y * rhs, z * rhs);
  }
  inline Vector operator*(const float rhs) const {
    return Vector(x * rhs, y * rhs, z * rhs);
  }
  inline Vector operator*(const int rhs) const {
    return Vector(x * rhs, y * rhs, z * rhs);
  }
  inline Vector operator/(const double rhs) const {
    return Vector(x / rhs, y / rhs, z / rhs);
  }
  inline Vector operator/(const float rhs) const {
    return Vector(x / rhs, y / rhs, z / rhs);
  }
  inline Vector operator/(const int rhs) const {
    return Vector(x / rhs, y / rhs, z / rhs);
  }
  inline Vector cross(const Vector a) const {
    return Vector(y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x);
  }
  inline double mag2() const { return x * x + y * y + z * z; }
  inline double mag() const { return sqrt(x * x + y * y + z * z); }
  inline double dot(const Vector a) const { return x * a.x + y * a.y + z * a.z; }
  inline Vector normalize() const {
    double m = mag();
    return Vector(x / m, y / m, z / m);
  }
} ;

class Ray{
public:
  Vector point, vector;
  Ray(const Vector& po, const Vector& ve) : point(po), vector(ve) {}
};

  inline Vector operator-(const Vector b){
   return Vector(-b.x,-b.y,-b.z);
  }
  
  inline Vector operator+(const Vector b){
   return b;
  }
  
  inline Vector operator*(const int a, const Vector b){
   return Vector(a*b.x,a*b.y,a*b.z);
  }

  inline Vector operator*(const double a, const Vector b){
   return Vector(a*b.x,a*b.y,a*b.z);
  }

  inline Vector operator*(const float a, const Vector b){
   return Vector(a*b.x,a*b.y,a*b.z);
  }

  inline Vector operator/(const int a, const Vector b){
   return Vector(a/b.x,a/b.y,a/b.z);
  }

  inline Vector operator/(const double a, const Vector b){
   return Vector(a/b.x,a/b.y,a/b.z);
  }

  inline Vector operator/(const float a, const Vector b){
   return Vector(a/b.x,a/b.y,a/b.z);
  }
  
  Vector solveScalers(Vector v1, Vector v2, Vector v3, Vector solve);

int print_vector(FILE *stream, const struct printf_info *info, const void 
*const *args);

int print_vector_arginfo (const struct printf_info *info, size_t n, int 
*argtypes);

#endif
