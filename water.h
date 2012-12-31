#ifndef WATER_H
#define WATER_H

#include <stdlib.h>
#include <glut.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <math.h>

#include "vector.h"
#include "bitmap.h"

#define	SIZEG 63

class Water
{
	int rainInterval;
	GLfloat viscosity;

	GLfloat position[SIZEG * SIZEG];
	GLfloat velocity[SIZEG * SIZEG];

	GLfloat vertices[SIZEG * SIZEG * 3];
	GLfloat normals[SIZEG * SIZEG * 3];

	GLfloat xAngle, yAngle;

public:
	GLuint waterTexture;
	Water();
	~Water();

	void CreateRainDrop();
	void Draw();
	GLuint LoadTexture();
	void Init();
	void Exit();
};

#endif