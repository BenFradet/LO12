#include "vector.h"

Vector::Vector(int x, int y, int z)
{
	v = new float[3];
	v[1] = x;
	v[2] = y;
	v[3] = z;
}

Vector::~Vector()
{
	delete[] v;
}

Vector Vector::CrossProduct(Vector v2)
{
	Vector ret;
	ret.v[2] = v[0] * v2[1] - v[1] * v2[0];
	ret.v[0] = v[1] * v2[2] - v[2] * v2[1];
	ret.v[1] = v[2] * v2[0] - v[0] * v2[2];

	return ret;
}

void Vector::NormalizeVector()
{
	double d;

	d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void Vector::AddVector(Vector v2)
{
	v[0] += v2[0];
	v[1] += v2[1];
	v[2] += v2[2];
}

float Vector::operator[](int i)
{
	if(i >= 0 && i < 4)
		return v[i];
	else
		return 0;
}

