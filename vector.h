#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

class Vector
{
	float* v;
public:
	Vector(int x = 0, int y = 0, int z = 0);
	~Vector();
	Vector CrossProduct(Vector);
	void NormalizeVector();
	void AddVector(Vector);
	float operator[](int i);
};

#endif