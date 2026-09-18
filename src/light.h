#ifndef __LIGHT_H__
#define __LIGHT_H__
#include "vector.h"
#include "camera.h"
#include "Textures/texture.h"
#include "Textures/colortexture.h"
#include <vector>
#include <memory>

class Light{
  public:
   unsigned char color[3];
   unsigned char* getColor(unsigned char a, unsigned char b, unsigned char c);
   Vector center;
   Light(const Vector & cente, unsigned char* colo);
};

class Shape;

class Autonoma{
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
};

void getLight(double* toFill, Autonoma* aut, Vector point, Vector norm, unsigned char r);

#endif
