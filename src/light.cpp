
#include "light.h"
#include "shape.h"
#include "camera.h"
#include <utility>
      
Light::Light(const Vector & cente, unsigned char* colo) : center(cente){
   color[0] = colo[0] / 255.;
   color[1] = colo[1] / 255.;
   color[2] = colo[2] / 255.;
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
      lightColor[0] = t->color[0];
      lightColor[1] = t->color[1];
      lightColor[2] = t->color[2];
      Vector ra = t->center-point;
      bool hit = false;
      Ray lightRay = Ray(point+ra*.01, ra);
	  for (const auto &shape : aut->shapes) {
		 hit = shape->getLightIntersection(lightRay, lightColor);
		 if (hit)
			 break;                   
      }
      if(!hit){
         double perc_dot = norm.dot(ra);
         if(flip && perc_dot<0) perc_dot=-perc_dot;
         if(perc_dot>0){
            double perc = perc_dot/(ra.mag());
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
