
#include "light.h"
#include "shape.h"
#include "camera.h"
#include <utility>
      
Light::Light(const Vector & cente, unsigned char* colo) : center(cente){
   color = colo;
}

unsigned char* Light::getColor(unsigned char a, unsigned char b, unsigned char c){
   unsigned char* r = (unsigned char*)malloc(sizeof(unsigned char)*3);
   r[0] = a;
   r[1] = b;
   r[2] = c;
   return r;
}

Autonoma::Autonoma(const Camera& c): camera(c){
   depth = 10;
   skybox = BLACK;
}

Autonoma::Autonoma(const Camera& c, Texture* tex): camera(c){
   depth = 10;
   skybox = tex;
}

void Autonoma::addShape(Shape* r){
	shapes.push_back(std::unique_ptr<Shape>(r));
}

void Autonoma::addLight(Light* r){
	lights.push_back(std::unique_ptr<Light>(r));
}

void getLight(double* tColor, Autonoma* aut, Vector point, Vector norm, unsigned char flip){
   tColor[0] = tColor[1] = tColor[2] = 0.;
   for (const auto &t : aut->lights) {
      double lightColor[3];     
      lightColor[0] = t->color[0]/255.;
      lightColor[1] = t->color[1]/255.;
      lightColor[2] = t->color[2]/255.;
      Vector ra = t->center-point;
      bool hit = false;
      Ray lightRay = Ray(point+ra*.01, ra);
	  for (const auto &shape : aut->shapes) {
		 hit = shape->getLightIntersection(lightRay, lightColor);
		 if (hit)
			 break;                   
      }
      double perc = (norm.dot(ra)/(ra.mag()*norm.mag()));
      if(!hit){
      if(flip && perc<0) perc=-perc;
        if(perc>0){
      
         tColor[0]+= perc*(lightColor[0]);
         tColor[1]+= perc*(lightColor[0]);
         tColor[2]+= perc*(lightColor[0]);
         if(tColor[0]>1.) tColor[0] = 1.;
         if(tColor[1]>1.) tColor[1] = 1.;
         if(tColor[2]>1.) tColor[2] = 1.;
        }
      }
   }
}
