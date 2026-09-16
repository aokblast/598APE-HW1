#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "vector.h"
#include "camera.h"
#include "Textures/texture.h"
#include "Textures/colortexture.h"
#include <vector>
#include <memory>
#include <algorithm>
#include <array>

class Light{
  public:
   unsigned char* color;
   unsigned char* getColor(unsigned char a, unsigned char b, unsigned char c);
   Vector center;
   Light(const Vector & cente, unsigned char* colo);
};

class Shape;
class BVHTree;

struct AABB {
  std::array<double, 3> minbounds;
  std::array<double, 3> maxbounds;
  inline void Union(const struct AABB &rhs) {
    for (int i = 0; i < 3; ++i) {
      minbounds[i] = std::min(minbounds[i], rhs.minbounds[i]);
      maxbounds[i] = std::max(maxbounds[i], rhs.maxbounds[i]);
    }
  }
  inline double calcSurfaceAreea() const {
    double a = maxbounds[0] - minbounds[0], b = maxbounds[1] - minbounds[1],
           c = maxbounds[2] - minbounds[2];
    return 2 * (a * b + b * c + a * c);
  }
  inline bool isFinite() const {
    for (int i = 0; i < 3; ++i)
      if (!std::isfinite(minbounds[i]) ||
          !std::isfinite(maxbounds[i]))
        return false;
    return true;
  }
  AABB() {
    for (int i = 0; i < 3; ++i) {
      minbounds[i] = std::numeric_limits<double>().max();
      maxbounds[i] = std::numeric_limits<double>().lowest();
    }
  }
  AABB(const std::array<double, 3> &mn, const std::array<double, 3> &mx)
      : minbounds(mn), maxbounds(mx) {}
};

class Autonoma{
private:
  BVHTree *bvh = nullptr;
public:
   Camera camera;
   Texture* skybox;
   unsigned int depth;
   std::vector<std::unique_ptr<Shape>> shapes;
   std::vector<std::unique_ptr<Light>> lights;
   Autonoma(const Camera &c);
   Autonoma(const Camera &c, Texture* tex);
   void addShape(Shape* s);
   void addLight(Light* s);
   void buildBVHTree();
   void intersect(const Ray ray, Shape *&shape, double &time);
   ~Autonoma();
};

void getLight(double* toFill, Autonoma* aut, Vector point, Vector norm, unsigned char r);

#endif
