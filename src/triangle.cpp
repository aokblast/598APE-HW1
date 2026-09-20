#include "triangle.h"
#include <algorithm>

Triangle::Triangle(Vector c, Vector b, Vector a, Texture* t):Plane(Vector(0,0,0), t, 0., 0., 0., 0., 0.) {
   center = c;
   Vector righta = (b-c);
   textureX = righta.mag();
   right = righta/textureX;
   vect = right.cross(b-a).normalize();

   xsin = -right.z;
   if(xsin<-1.)xsin = -1;
   else if (xsin>1.)xsin=1.; 
   yaw = asin(xsin);
   xcos = sqrt(1.-xsin*xsin);

   zcos = right.x/xcos;
   zsin = -right.y/xcos;
   if(zsin<-1.)zsin = -1;
   else if (zsin>1.)zsin=1.;
   if(zcos<-1.)zcos = -1;
   else if (zcos>1.)zcos=1.;
   roll = atan2(zsin, zcos);

   ycos = vect.z/xcos;
   if(ycos<-1.)ycos = -1;
   else if (ycos>1.)ycos=1.;
   up = vect.cross(right);

   Vector np = solveScalers(right, up, vect, a-c);
   ysin = -up.z/xcos;
   pitch = atan2(ysin, ycos);
   textureY = np.y;
   thirdX = np.x;
   
   d = -vect.dot(center);
}

double Triangle::getIntersection(Ray ray) {
  Vector S = ray.point - center;
  Vector E1 = right * textureX;
  Vector E2 = right * thirdX + up * textureY;
  Vector S1 = ray.vector.cross(E2);
  Vector S2 = S.cross(E1);

  double S1E1 = S1.dot(E1);
  // Parallel
  if (S1E1 == 0)
	  return inf;
  double t = S2.dot(E2) / S1E1;
  double b1 = S1.dot(S) / S1E1;
  double b2 = S2.dot(ray.vector) / S1E1;

  if (t > 0 && b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0)
    return t;
  return inf;
}

bool Triangle::getLightIntersection(Ray ray, double* fill){
  Vector S = ray.point - center;
  Vector E1 = right * textureX;
  Vector E2 = right * thirdX + up * textureY;
  Vector S1 = ray.vector.cross(E2);
  Vector S2 = S.cross(E1);

  double S1E1 = S1.dot(E1);
  // Parallel
  if (S1E1 == 0)
	  return false;
  double t = S2.dot(E2) / S1E1;
  if (t <= 0 || t >= 1)
	  return false;
  double b1 = S1.dot(S) / S1E1;
  double b2 = S2.dot(ray.vector) / S1E1;

  if (!(b1 >= 0 && b2 >= 0 && (1 - b1 - b2) >= 0))
    return false;
  if (texture->opacity > 1 - 1E-6)
    return true;
  unsigned char temp[4];
  double amb, op, ref;
  texture->getColor(temp, &amb, &op, &ref, fix(b1 + b2 * thirdX / textureX - .5),
                    fix(b2 - .5));
  if (op > 1 - 1E-6)
    return true;
  fill[0] *= temp[0] / 255.;
  fill[1] *= temp[1] / 255.;
  fill[2] *= temp[2] / 255.;
  return false;
}

AABB Triangle::buildAABB() {
  Vector b = right * textureX + center;
  Vector a = thirdX * right + textureY * up + center;
  return {{std::min({a.x, b.x, center.x}), std::min({a.y, b.y, center.y}),
           std::min({a.z, b.z, center.z})},
          {std::max({a.x, b.x, center.x}), std::max({a.y, b.y, center.y}),
           std::max({a.z, b.z, center.z})}};
}
