#include "Raytracer.h"
#include <iostream>
#include <chrono>

typedef Vector3d<double> vec3f;
typedef  std::chrono::time_point<std::chrono::system_clock> timept;
typedef  std::chrono::duration<double> durationd;
using  std::chrono::system_clock;

std::vector<Sphere> spheres = {//Scene: radius, position, emission, color, material
	Sphere(1e5, vec3f(1e5 + 1, 40.8, 81.6), vec3f(), vec3f(.75, .25, .25), DIFFUSE),//Left
	Sphere(1e5, vec3f(-1e5 + 99, 40.8, 81.6), vec3f(), vec3f(.25, .25, .75), DIFFUSE),//Rght
	Sphere(1e5, vec3f(50, 40.8, 1e5), vec3f(), vec3f(.75, .75, .75), DIFFUSE),//Back
	Sphere(1e5, vec3f(50, 40.8, -1e5 + 170), vec3f(), vec3f(), DIFFUSE),//Frnt
	Sphere(1e5, vec3f(50, 1e5, 81.6), vec3f(), vec3f(.75, .75, .75), DIFFUSE),//Botm
	Sphere(1e5, vec3f(50, -1e5 + 81.6, 81.6), vec3f(), vec3f(.75, .75, .75), DIFFUSE),//Top
	Sphere(16.5, vec3f(27, 16.5, 47), vec3f(), vec3f(1, 1, 1)*.999, SPECULAR),//Mirr
	Sphere(16.5, vec3f(73, 16.5, 78), vec3f(), vec3f(1, 1, 1)*.999, REFRACTIVE),//Glas
	Sphere(600, vec3f(50, 681.6 - .27, 81.6), vec3f(12, 12, 12), vec3f(), DIFFUSE) //Lite
};

int main(int argc, char *argv[])
{
	/*If image is noisy, increase samples. Samples will increase render time*/
	int w = 600, h = 300, samps = 4;
	std::string img = "myRender.bmp";
	
	//Timer for render
	timept begin, end;
	begin = system_clock::now();
	std::cout << "Rendering image " << img << " with width " << w << " and height " << h << " using " << samps << " samples..." << std::endl;;
	Raytracer raytrace = Raytracer();
	raytrace.init(w, h, samps);
	raytrace.setCam(vec3f(50, 52, 295.6), vec3f(0, -0.042612, -1));
	raytrace.setSpheres(spheres);
	raytrace.render();

	std::cout << "Render finished. Saving to BMP..." << std::endl;
	raytrace.saveBMP(img);
	end = system_clock::now();

	std::cout << "Done in " << (durationd(end-begin)).count() << " seconds." <<std::endl;

	return 0;
}