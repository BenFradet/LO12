#ifndef TERRAIN_H
#define TERRAIN_H

#define TERRAIN_ERROR_LOADING_IMAGE		-4
#define TERRAIN_ERROR_MEMORY_PROBLEM	-3
#define	TERRAIN_ERROR_NOT_SAVED			-2
#define TERRAIN_ERROR_NOT_INITIALISED	-1
#define TERRAIN_OK						 0

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <glut.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <random>

#include "bitmap.h"

class Terrain
{
public:
	static int width, length;
	static float *heights;
	static float *colors;
	static float *normals;

	int LoadFromImage(char *filename, int normals);
	int Create(float xOffset, float yOffset, float zOffset);
	void Destroy();
	int Scale(float min,float max);
	float GetHeight(int x, int z);	
	void ComputeNormals();
	void WeightsBlending(float* percent, unsigned char height);
	GLuint LoadTextures();

	float* CrossProduct(int, int, int, int, int, int);
	void NormalizeVector(float *v);
	void AddVector(float *a, float *b);
};

#endif