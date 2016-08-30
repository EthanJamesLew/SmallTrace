#ifndef SPHERE_H
#define SPHERE_H

#include "Vector3d.h"
#include "Ray.h"
#include "Shader.h"

typedef Vector3d<double> vec3f;

struct Sphere {
	double radius;
	
	vec3f p, e, c;
	Shader refl;
	
	Sphere(double radius_, vec3f p_, vec3f e_, vec3f c_, Shader refl_) :
		radius(radius_), p(p_), e(e_), c(c_), refl(refl_) {}
	
	double intersect(const Ray &r) const;
};
#endif