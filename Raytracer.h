#ifndef RAYTRACER_H
#define RAYTRACER
#include "Vector3d.h"
#include "Sphere.h"
#include <string>
#include "Ray.h"
#include <vector>

typedef Vector3d<double> vec3f;

class Raytracer
{
	/*
	The Raytracer class is a forward raytracer that uses Monte Carlo
	stepping to approximate the rendering integral. Credit to Kevin Beason's
	SmallPT for the radiance and render algorithms
	*/
public:
	/*Two step initialization*/
	Raytracer():_width(1024), _height(768), _samples(2){}
	
	/*Initializers*/
	void init(int width, int height, int samples);
	void setSamples(int samps);
	void setDimensions(int width, int height);
	void setSpheres(const std::vector<Sphere>&  sphs);
	void setCam(vec3f position, vec3f direction);

	/*Render to _pixels*/
	void render();

	/*Save render to image*/
	void savePPM(const std::string& imageName);
	void saveBMP(const std::string& imageName);
	
private:
	int _width;
	int _height;
	int _samples;


	Ray _cam;
	std::vector<Sphere>  _spheres;
	vec3f* _pixels;

	//Private Methods
	vec3f _radiance(const Ray &r, int depth, unsigned short *Xi);
	bool _intersect(const Ray &r, double &t, int &id);


};
#endif