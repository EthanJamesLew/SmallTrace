#ifndef RAY_H
#define RAY_H

#include "Vector3d.h"
typedef Vector3d<double> vec3f;

struct Ray 
{ 
	
	vec3f origin;
	vec3f direction; 

	Ray(){};
	Ray(vec3f o_, vec3f d_) : origin(o_), direction(d_) {} 

};

#endif