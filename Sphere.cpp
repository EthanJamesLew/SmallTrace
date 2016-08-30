#include "Sphere.h"

double Sphere::intersect(const Ray &r) const
{
	vec3f op = p - r.origin;
	double t, eps = 1e-4, b = op.dot(r.direction), det = b*b - op.dot(op) + radius*radius;
	if (det<0) return 0; else det = sqrt(det);
	return (t = b - det)>eps ? t : ((t = b + det)>eps ? t : 0);
}