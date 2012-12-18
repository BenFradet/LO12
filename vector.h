#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

class Vector
{
	float* v;
	float v1, v2, v3;
public:
	Vector(int x = 0, int y = 0, int z = 0);
	~Vector();
	Vector CrossProduct(Vector);
	void NormalizeVector();
	void AddVector(Vector);
	float operator[](int i);

	static void normalize(float*);
	static void normalize(float&, float&, float&);
	static void crossProduct(float*, float*, float*);
	static float dotProduct(float*, float*);
};

#endif