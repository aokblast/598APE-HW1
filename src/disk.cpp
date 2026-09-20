#include "disk.h"
Disk::Disk(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty):Plane(c, t, ya, pi, ro, tx, ty){}


double Disk::getIntersection(Ray ray){
   double time = Plane::getIntersection(ray);
   if(time==inf) 
      return time;
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*time-center);
   return (  dist.x*dist.x/(textureX*textureX)+dist.y*dist.y/(textureY*textureY)>1  )?inf:time;
}

bool Disk::getLightIntersection(Ray ray, double* fill){
   const double t = ray.vector.dot(vect);
   const double norm = vect.dot(ray.point)+d;
   const double r = -norm/t;
   if(r<=0. || r>=1.) return false;
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*r-center);
   if(  dist.x*dist.x/(textureX*textureX)+dist.y*dist.y/(textureY*textureY)>1  )return false;
   if(texture->opacity>1-1E-6) return true;   
   unsigned char temp[4];
   double amb, op, ref;
   texture->getColor(temp, &amb, &op, &ref,fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
   if(op>1-1E-6) return true;
   fill[0]*=temp[0]/255.;
   fill[1]*=temp[1]/255.;
   fill[2]*=temp[2]/255.;
   return false;
}

AABB Disk::buildAABB(){
   const double ex = sqrt(right.x*right.x*textureX*textureX + up.x*up.x*textureY*textureY);
   const double ey = sqrt(right.y*right.y*textureX*textureX + up.y*up.y*textureY*textureY);
   const double ez = sqrt(right.z*right.z*textureX*textureX + up.z*up.z*textureY*textureY);
   return {{center.x-ex, center.y-ey, center.z-ez},
           {center.x+ex, center.y+ey, center.z+ez}};
}
