#include "water.h"

Water::Water()
{
	/*position = new GLfloat[SIZEG * SIZEG]();
	velocity = new GLfloat[SIZEG * SIZEG]();

	vertices = new GLfloat[3 * SIZEG * SIZEG]();
	normals = new GLfloat[3 * SIZEG * SIZEG]();*/
}

Water::~Water()
{
	/*delete[] position;
	delete[] velocity;

	delete[] vertices;
	delete[] normals;*/
}

void Water::CreateRainDrop()
{
	//velocity[(rand()%(SIZEG - 6) + 6) * SIZEG + rand()%(SIZEG - 6) + 6] = 1060;
	velocity[10] = 1060;
}

void Water::Draw()
{
	for(int i = 2; i < SIZEG - 2; i++)
	{
		for(int j = 2; j < SIZEG - 2; j++)
		{
			velocity[i * SIZEG + j] = velocity[i * SIZEG + j] + (position[i * SIZEG + j] - 
				(4*(position[(i - 1) * SIZEG + j] + position[(i + 1) * SIZEG + j] + position[i * SIZEG + j - 1] + position[i * SIZEG + j + 1])
				+ position[(i - 1) * SIZEG + j - 1] + position[(i + 1) * SIZEG + j - 1] + position[(i - 1) * SIZEG + j + 1] + position[(i + 1) * SIZEG + j + 1]) / 25.0f) / 7.0f;
		}
	}

	for(int i = 2; i < SIZEG - 2; i++)
	{
		for(int j = 2; j < SIZEG - 2; j++)
		{
			position[i * SIZEG + j] -= velocity[i * SIZEG + j];
			velocity[i * SIZEG + j] *= viscosity;
		}
	}

	for(int i = 0; i < SIZEG; i++)
	{
		for(int j = 0; j < SIZEG; j++)
		{
			*(vertices + i * 3 * SIZEG + j * 3) = (i - SIZEG / 2.0) / (float)SIZEG * 5;
			*(vertices + i * 3 * SIZEG + j * 3 + 1) = (position[i * SIZEG + j] / 1024.0) / (float)SIZEG * 3 + 10;
			*(vertices + i * 3 * SIZEG + j * 3 + 2) = (j - SIZEG / 2.0) / (float)SIZEG * 5;
		}
	}

	for(int i = 0; i < SIZEG; i++)
	{
		for(int j = 0; j < SIZEG; j++)
		{
			float vecLength;
			if(i > 0 && j > 0 && i < SIZEG && j < SIZEG)
			{
				*(normals + i * 3 * SIZEG + j * 3) = position[(i + 1) * SIZEG + j] - position[(i - 1) * SIZEG + j];
				*(normals + i * 3 * SIZEG + j * 3 + 1) = - 2048;
				*(normals + i * 3 * SIZEG + j * 3 + 2) = position[i * SIZEG + j + 1] - position[i * SIZEG + j - 1];

				vecLength = sqrt(*(normals + i * 3 * SIZEG + j * 3 + 1) * *(normals + i * 3 * SIZEG + j * 3 + 1) + 
					*(normals + i * 3 * SIZEG + j * 3 + 2) * *(normals + i * 3 * SIZEG + j * 3) + 
					*(normals + i * 3 * SIZEG + j * 3) * *(normals + i * 3 * SIZEG + j * 3));
				if(vecLength != 0)
				{
					*(normals + i * 3 * SIZEG + j * 3) /= vecLength;
					*(normals + i * 3 * SIZEG + j * 3 + 1) /= vecLength;
					*(normals + i * 3 * SIZEG + j * 3 + 2) /= vecLength;
				}

				//Vector::normalize(&normals[i * 3 * SIZEG + j * 3], &normals[i * 3 * SIZEG + j * 3 + 1], &normals[i * 3 * SIZEG + j * 3 + 2]);
			}
			else
			{
				*(normals + i * 3 * SIZEG + j * 3) = 0;
				*(normals + i * 3 * SIZEG + j * 3 + 1) = 1;
				*(normals + i * 3 * SIZEG + j * 3 + 2) = 0;
			}
		}
	}

	glBindTexture(GL_TEXTURE_2D, waterTexture);

	for(int j = 0; j < SIZEG - 1; j++)
	{
		glBegin(GL_QUAD_STRIP);
			for(int i = 0; i < SIZEG; i++)
			{
				glNormal3f(normals[i * 3 * SIZEG + (j + 1) * 3], normals[i * 3 * SIZEG + (j + 1)* 3 + 1], normals[i * 3 * SIZEG + (j + 1) * 3 + 2]);
				glTexCoord2f(i / SIZEG, (j + 1) / SIZEG);
				glVertex3f(vertices[i * 3 * SIZEG + j * 3 + 3], vertices[i * 3 * SIZEG + j * 3 + 4], vertices[i * 3 * SIZEG + j * 3 + 5]);
				glNormal3f(normals[i * 3 * SIZEG + j * 3], normals[i * 3 * SIZEG + j * 3 + 1], normals[i * 3 * SIZEG + j * 3 + 2]);
				glTexCoord2f(i / SIZEG, j / SIZEG);
				glVertex3f(vertices[i * 3 * SIZEG + j * 3], vertices[i * 3 * SIZEG + j * 3 + 1], vertices[i * 3 * SIZEG + j * 3 + 2]);
			}
	}
}

void Water::Exit()
{
	
}

GLuint Water::LoadTexture()
{
	GLuint texName;

	Bitmap* image = new Bitmap("Data/reflection.bmp", 0);
	int w = image->width;
	int h = image->height;

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

	delete image;

	return texName;
}

void Water::Init()
{
	waterTexture = LoadTexture();

	/*glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	//glClearColor(1, 1, 1, 1);
	glClearStencil(0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_TEXTURE_2D);*/

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	viscosity = 0.96;
	rainInterval = 1000;
	
	for(int i = 0; i < SIZEG; i++)
		for(int j = 0; j < SIZEG; j++)
		{
			position[i * SIZEG + j] = 0;
			velocity[i * SIZEG + j] = 0;
		}
}