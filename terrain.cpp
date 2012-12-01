#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl\glut.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include "bitmap.h"
#include "terrain.h"

int Terrain::terrainGridWidth = 0;
int Terrain::terrainGridLength = 0;
float* Terrain::terrainHeights = NULL;
float* Terrain::terrainColors = NULL;
float* Terrain::terrainNormals = NULL;


float* Terrain::terrainCrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) 
{
	float *auxNormal,v1[3],v2[3];
		
	v1[0] = x2-x1; 
	v1[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
			+ terrainHeights[z2 * terrainGridWidth + x2];
	v1[2] = z2-z1; 


	v2[0] = x3-x1; 
	v2[1] = -terrainHeights[z1 * terrainGridWidth + x1] 
			+ terrainHeights[z3 * terrainGridWidth + x3];
	v2[2] = z3-z1; 

	auxNormal = (float *)malloc(sizeof(float)*3);

	auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
	auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];

	return(auxNormal);
}

void Terrain::terrainNormalize(float *v) 
{
	double d,x,y,z;

	x = v[0];
	y = v[1];
	z = v[2];
	
	d = sqrt((x*x) + (y*y) + (z*z));

	v[0] = x / d;
	v[1] = y / d;
	v[2] = z / d;
}

void Terrain::terrainAddVector(float *a, float *b) 
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void Terrain::terrainComputeNormals() 
{
	float *norm1,*norm2,*norm3,*norm4; 
	int i,j,k;
	
	if (terrainNormals == NULL)
		return;

	for(i = 0; i < terrainGridLength; i++)
		for(j = 0; j < terrainGridWidth; j++) {
			norm1 = NULL;
			norm2 = NULL;
			norm3 = NULL;
			norm4 = NULL;

			// normals for the four corners
			if (i == 0 && j == 0) {
				norm1 = terrainCrossProduct(0,0, 0,1, 1,0);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(i,j, j,i-1, j-1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == 0 && i == terrainGridLength-1) {
				norm1 = terrainCrossProduct(i,j, j,i-1, j+1,i);	
				terrainNormalize(norm1);				
			}
			else if (j == terrainGridWidth-1 && i == 0) {
				norm1 = terrainCrossProduct(i,j, j,i+1, j-1,i);	
				terrainNormalize(norm1);				
			}

			// normals for the borders
			else if (i == 0) {
				norm1 = terrainCrossProduct(j,0, j-1,0, j,1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,0,j,1,j+1,0);
				terrainNormalize(norm2);
			}
			else if (j == 0) {
				norm1 = terrainCrossProduct(0,i, 1,i, 0,i-1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(0,i, 0,i+1, 1,i);
				terrainNormalize(norm2);
			}
			else if (i == terrainGridLength) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j+1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm2);
			}
			else if (j == terrainGridWidth) {
				norm1 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm2);
			}

			// normals for the interior
			else {
				norm1 = terrainCrossProduct(j,i, j-1,i, j,i+1);
				terrainNormalize(norm1);
				norm2 = terrainCrossProduct(j,i, j,i+1, j+1,i);
				terrainNormalize(norm2);
				norm3 = terrainCrossProduct(j,i, j+1,i, j,i-1);
				terrainNormalize(norm3);
				norm4 = terrainCrossProduct(j,i, j,i-1, j-1,i);
				terrainNormalize(norm4);
			}
			if (norm2 != NULL) {
				terrainAddVector(norm1,norm2);
				free(norm2);
			}
			if (norm3 != NULL) {
				terrainAddVector(norm1,norm3);
				free(norm3);
			}
			if (norm4 != NULL) {
				terrainAddVector(norm1,norm4);
				free(norm4);
			}
			terrainNormalize(norm1);
			norm1[2] = - norm1[2];
			for (k = 0; k< 3; k++) 
				terrainNormals[3*(i*terrainGridWidth + j) + k] = norm1[k];
			free(norm1);
		}

}

int Terrain::terrainLoadFromImage(char* filename, int normals)
{
	Bitmap* bm = new Bitmap(filename, 1);
	int mode;
	float pointHeight;

	if(terrainHeights != NULL)
		terrainDestroy();

	if(!bm->loadBmp(filename, 1))
		return (TERRAIN_ERROR_LOADING_IMAGE);

	mode = bm->bpp / 8;

	if(mode != 3)
		return TERRAIN_ERROR_LOADING_IMAGE;

	terrainGridWidth = bm->width;
	terrainGridLength = bm->height;

	terrainHeights = (float*)malloc(terrainGridWidth * terrainGridLength * sizeof(float));
	if(terrainHeights == NULL)
		return (TERRAIN_ERROR_MEMORY_PROBLEM);

	if(normals)
	{
		terrainNormals = (float*)malloc(terrainGridWidth * terrainGridLength * sizeof(float) * 3);
		if(terrainNormals == NULL)
			return (TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		terrainNormals = NULL;

	for(int i = 0; i<terrainGridWidth; i++)
		for(int j = 0; j<terrainGridLength; j++)
		{
			pointHeight = (float)bm->data[i*terrainGridWidth + j] / 256.0;
			terrainHeights[i * terrainGridWidth + j] = pointHeight;
		}

	if(normals)
		terrainComputeNormals();

	//delete bm;

	return TERRAIN_OK;
}

int Terrain::terrainScale(float min,float max) 
{
	float amp,aux,min1,max1,height;
	int total,i;

	if (terrainHeights == NULL)
		return(TERRAIN_ERROR_NOT_INITIALISED);

	if (min > max) 
	{
		aux = min;
		min = max;
		max = aux;
	}

	amp = max - min;
	total = terrainGridWidth * terrainGridLength;

	min1 = terrainHeights[0];
	max1 = terrainHeights[0];
	for(i=1;i < total ; i++) 
	{
		if (terrainHeights[i] > max1)
			max1 = terrainHeights[i];
		if (terrainHeights[i] < min1)
			min1 = terrainHeights[i];
	}
	for(i=0;i < total; i++) {
		height = (terrainHeights[i] - min1) / (max1-min1);
		terrainHeights[i] = height * amp - min;
	}
	printf("%f %f %f %f %f %f\n",min,max,min1,max1,amp,height);
	if (terrainNormals != NULL)
		terrainComputeNormals();
	return(TERRAIN_OK);
}
	

int Terrain::terrainCreateDL(float xOffset, float yOffset, float zOffset) 
{
	GLuint terrainDL;
	float startW,startL;
	int i,j;

	startW = terrainGridWidth / 2.0 - terrainGridWidth;
	startL = - terrainGridLength / 2.0 + terrainGridLength;

	terrainDL = glGenLists(1);

	glNewList(terrainDL,GL_COMPILE);
	if (terrainNormals != NULL && terrainColors != NULL) 
	{
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}

	for (i = 0 ; i < terrainGridLength-1; i++) 
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0;j < terrainGridWidth; j++) 
		{
		
			if (terrainColors != NULL) 
				glColor3f(terrainColors[3*((i+1)*terrainGridWidth + j)],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+1],
						  terrainColors[3*((i+1)*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL)
				glNormal3f(terrainNormals[3*((i+1)*terrainGridWidth + j)],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+1],
						  terrainNormals[3*((i+1)*terrainGridWidth + j)+2]);
			glVertex3f(
				startW + j + xOffset,
				terrainHeights[(i+1)*terrainGridWidth + (j)] + yOffset,
				startL - (i+1) + zOffset);

			if (terrainColors != NULL) 
				glColor3f(terrainColors[3*(i*terrainGridWidth + j)],
						  terrainColors[3*(i*terrainGridWidth + j)+1],
						  terrainColors[3*(i*terrainGridWidth + j)+2]);
			if (terrainNormals != NULL)
				glNormal3f(terrainNormals[3*(i*terrainGridWidth + j)],
						  terrainNormals[3*(i*terrainGridWidth + j)+1],
						  terrainNormals[3*(i*terrainGridWidth + j)+2]);

			//glEnable(GL_TEXTURE_2D);

			glVertex3f(
				startW + j + xOffset, 
				terrainHeights[(i)*terrainGridWidth + j] + yOffset,
				startL - i + zOffset);					
		}
		glEnd();
	}
	glEndList();
	return(terrainDL);
}

float Terrain::terrainGetHeight(int x, int z) 
{
	int xt,zt;

	if (terrainHeights == NULL) 
			return(0.0);

	xt = x + terrainGridWidth /2;
	zt = terrainGridWidth - (z + terrainGridLength /2);

	if ((xt > terrainGridWidth) || (zt > terrainGridLength) || (xt < 0) || (zt < 0))
		return(0.0);

	return(terrainHeights[zt * terrainGridWidth + xt]);
}


void Terrain::terrainDestroy() 
{
	if (terrainHeights != NULL)
		free(terrainHeights);

	if (terrainColors != NULL)
		free(terrainColors);

	if (terrainNormals != NULL)
		free(terrainNormals);
}