#ifndef WATER_H
#define WATER_H

#include <stdlib.h>
#include <glut.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <math.h>

#include "vector.h"
#include "bitmap.h"

#define	SIZE 63

class Water
{
	GLuint waterTexture;

	int rainInterval;
	GLfloat viscosity;

	GLfloat position[SIZE * SIZE];
	GLfloat velocity[SIZE * SIZE];

	GLfloat vertices[SIZE * SIZE * 3];
	GLfloat normals[SIZE * SIZE * 3];

	GLfloat xAngle, yAngle;

public:
	Water();
	~Water();

	void CreateRainDrop();
	void Draw();
	GLuint LoadTexture();
	void Init();
	void Exit();
};

#endif