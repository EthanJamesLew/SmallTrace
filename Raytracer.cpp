#include "Raytracer.h"
#include "Shader.h"

#include <math.h>  
#include <stdlib.h>
#include <stdio.h> 

//Unix has these defined 
#ifdef  _WIN32
#define M_PI 3.1415926535897932384626433832795
double erand48(unsigned short seed[3])
{
	return (double)rand()
		/ (double)RAND_MAX;
}
#endif

//Utilities for the radiance function
inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }
inline int toInt(double x){ return int(pow(clamp(x), 1 / 2.2) * 255 + .5); }


void Raytracer::init(int width, int height, int samples)
{
	_width = width; _height = height; _samples = samples;
	_pixels = new vec3f[width*height];
}

void Raytracer::setSamples(int samps)
{
	_samples = samps;
}

void Raytracer::setDimensions(int width, int height)
{
	_width = width; _height = height;
	_pixels = new vec3f[width*height];
}

void Raytracer::setSpheres(const std::vector<Sphere>& sphs)
{
	_spheres = sphs;
}

void Raytracer::setCam(vec3f position, vec3f direction)
{
	_cam = Ray(position, direction.normalize());
}

void Raytracer::render()
{
	vec3f cx = vec3f(_width*.5135 / _height, 0, 0), cy = (cx%_cam.direction).normalize()*.5135, r;

	/*Parallel Process through raytracer*/
#pragma omp parallel for schedule(dynamic, 1) private(r) 
	for (int y = 0; y<_height; y++){
		unsigned short Xi[3] = { 0, 0, y*y*y }; 
		for (unsigned short x = 0; x<_width; x++)

			//basic anti-aliasing. Subsample and average for pixels. 2x2 subsampling is used
			for (int sy = 0, i = (_height - y - 1)*_width + x; sy<2; sy++)
				for (int sx = 0; sx<2; sx++, r = vec3f()){
			for (int s = 0; s<_samples; s++){
				double r1 = 2 * erand48(Xi), dx = r1<1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
				double r2 = 2 * erand48(Xi), dy = r2<1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
				vec3f d = cx*(((sx + .5 + dx) / 2 + x) / _width - .5) +
					cy*(((sy + .5 + dy) / 2 + y) / _height - .5) + _cam.direction;
				r = r + _radiance(Ray(_cam.origin + d * 140, d.normalize()), 0, Xi)*(1. / _samples);
			}
			_pixels[i] = _pixels[i] + vec3f(clamp(r.x), clamp(r.y), clamp(r.z))*.25;
				}
	}
}


void Raytracer::savePPM(const std::string& imageName)
{
	FILE *f = fopen(imageName.c_str(), "w");         // Write image to PPM file.
	fprintf(f, "P3\n%d %d\n%d\n", _width, _height, 255);
	for (int i = 0; i < _width*_height; i++)
		fprintf(f, "%d %d %d ", toInt(_pixels[i].x), toInt(_pixels[i].y), toInt(_pixels [i].z));
}

void Raytracer::saveBMP(const std::string& imageName)
{
	FILE *f;
	int k = _width*_height;
	int w = _width;
	int h = _height;
	int s = 4 * k;
	int filesize = 54 + s;

	float factor = 39.375;
	int m = static_cast<int>(factor);

	int dpi = 72;
	int ppm = dpi*m;

	unsigned char bmpfileheader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
	unsigned char bmpinfoheader[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w >> 8);
	bmpinfoheader[6] = (unsigned char)(w >> 16);
	bmpinfoheader[7] = (unsigned char)(w >> 24);

	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s >> 8);
	bmpinfoheader[23] = (unsigned char)(s >> 16);
	bmpinfoheader[24] = (unsigned char)(s >> 24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm >> 8);
	bmpinfoheader[27] = (unsigned char)(ppm >> 16);
	bmpinfoheader[28] = (unsigned char)(ppm >> 24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm >> 8);
	bmpinfoheader[31] = (unsigned char)(ppm >> 16);
	bmpinfoheader[32] = (unsigned char)(ppm >> 24);

	f = fopen(imageName.c_str(), "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);

	for (int k = _height; k > 0; k--)
	{
		for (int j = 0; j < _width; j++)
		{
			int i = (k-1)*_width + j;
			float red = (_pixels[i].x) * 255;
			float green = (_pixels[i].y) * 255;
			float blue = (_pixels[i].z) * 255;

			unsigned char color[3] = { (int)floor(blue), (int)floor(green), (int)floor(red) };
			fwrite(color, 1, 3, f);
		}
	}

	fclose(f);
}

bool Raytracer::_intersect(const Ray &r, double &t, int &id){
	double n = _spheres.size(), d, inf = t = 1e20;
	for (int i = int(n); i--;) if ((d = _spheres[i].intersect(r)) && d<t){ t = d; id = i; }
	return t<inf;
}

vec3f Raytracer::_radiance(const Ray &r, int depth, unsigned short *Xi)
{
	/*Credit to Kevin Beason's SmallPT for this radiance algorithm implementation*/

	//TODO: Set Shading models to separate functions

	/*Find which object a ray of light intersects and give it an id*/
	double t;
	int id = 0;
	/*Return black if no intersection*/
	if (!_intersect(r, t, id)) return vec3f();

	/*Obtain normal angle of the intersecting ray*/
	const Sphere &obj = _spheres[id];
	vec3f x = r.origin + r.direction*t, n = (x - obj.p).normalize(), nl = n.dot(r.direction)<0 ? n : n*-1, f = obj.c;
	double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z;

	/*Set limit to recursion*/
	if (depth > 100) return obj.e;
	if (++depth>5) if (erand48(Xi)<p) f = f*(1 / p); else return obj.e;

	/*Diffuse Lambertian Shading model*/
	if (obj.refl == DIFFUSE){
		double r1 = 2 * M_PI*erand48(Xi), r2 = erand48(Xi), r2s = sqrt(r2);
		vec3f w = nl, u = ((fabs(w.x)>.1 ? vec3f(0, 1, 0) : vec3f(1, 0, 0)) % w).normalize(), v = w%u;
		vec3f d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1 - r2)).normalize();
		return obj.e + f*(_radiance(Ray(x, d), depth, Xi));
	}

	/*Specular Blinn Shading model*/
	else if (obj.refl == SPECULAR)
		return obj.e + f*(_radiance(Ray(x, r.direction - n * 2 * n.dot(r.direction)), depth, Xi));

	/*Dielectric Refraction Model*/
	Ray reflRay(x, r.direction - n * 2 * n.dot(r.direction));
	bool into = n.dot(nl)>0;
	double nc = 1, nt = 1.5, nnt = into ? nc / nt : nt / nc, ddn = r.direction.dot(nl), cos2t;

	/*Internal reflection*/
	if ((cos2t = 1 - nnt*nnt*(1 - ddn*ddn))<0)
		return obj.e + f*(_radiance(reflRay, depth, Xi));
	vec3f tdir = (r.direction*nnt - n*((into ? 1 : -1)*(ddn*nnt + sqrt(cos2t)))).normalize();

	/*Monte Carlo sample to approximate the render integral*/
	double a = nt - nc, b = nt + nc, R0 = a*a / (b*b), c = 1 - (into ? -ddn : tdir.dot(n));
	double Re = R0 + (1 - R0)*c*c*c*c*c, Tr = 1 - Re, P = .25 + .5*Re, RP = Re / P, TP = Tr / (1 - P);
	return obj.e + f*(depth>2 ? (erand48(Xi)<P ?
		_radiance(reflRay, depth, Xi)*RP : _radiance(Ray(x, tdir), depth, Xi)*TP) :
		_radiance(reflRay, depth, Xi)*Re + _radiance(Ray(x, tdir), depth, Xi)*Tr);
}