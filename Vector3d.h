#ifndef VECTOR3D_H
#define VECTOR3D_H
#include <iostream>
template<typename T>
class Vector3d
{
public:
	T x, y, z;

	//Constructors
	Vector3d();
	Vector3d(T xx);
	Vector3d(T xx, T yy, T zz);

	//Norm
	Vector3d& normalize();

	//Products
	//TODO: Add Cross Product
	Vector3d<T> operator % (const Vector3d<T> &v) const;
	Vector3d<T> operator * (const T &f) const;
	Vector3d<T> operator * (const Vector3d<T> &v) const;
	T dot(const Vector3d<T> &v) const;

	//Arithmetic Operations
	Vector3d<T> operator - (const Vector3d<T> &v) const;
	Vector3d<T> operator + (const Vector3d<T> &v) const;
	Vector3d<T>& operator += (const Vector3d<T> &v);
	Vector3d<T>& operator *= (const Vector3d<T> &v);
	Vector3d<T> operator - () const;

	//Comparators
	bool operator == (const Vector3d<T> &v);

	//Lengths
	T length2() const;
	T length() const;

	//Console Print
	friend std::ostream & operator << (std::ostream &os, const Vector3d<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	};
};

#include "Vector3d.cpp"

#endif