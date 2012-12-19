#include "water.h"

Water::Water()
{
	position = new GLfloat[SIZE * SIZE]();
	velocity = new GLfloat[SIZE * SIZE]();

	vertices = new GLfloat[3 * SIZE * SIZE]();
	normals = new GLfloat[3 * SIZE * SIZE]();
}

Water::~Water()
{
	delete[] position;
	delete[] velocity;

	delete[] vertices;
	delete[] normals;
}

void Water::CreateRainDrop()
{
	velocity[(rand()%(SIZE - 6) + 6) * SIZE + rand()%(SIZE - 6) + 6] = 1060;
}

void Water::Draw()
{
	for(int i = 6; i < SIZE - 6; i++)
		for(int j = 6; j < SIZE - 6; j++)
			velocity[i * SIZE + j] = velocity[i * SIZE + j] + (position[i * SIZE + j] - 
				(4*(position[(i - 1) * SIZE + j] + position[(i + 1) * SIZE + j] + position[i * SIZE + j - 1] + position[i * SIZE + j + 1])
				+ position[(i - 1) * SIZE + j - 1] + position[(i + 1) * SIZE + j - 1] + position[(i - 1) * SIZE + j + 1] + position[(i + 1) * SIZE + j + 1]) / 25.0f) / 7.0f;

	for(int i = 6; i < SIZE - 6; i++)
		for(int j = 6; i < SIZE - 6; i++)
		{
			position[i * SIZE + j] -= velocity[i * SIZE + j];
			velocity[i * SIZE + j] *= viscosity;
		}

	for(int i = 0; i < SIZE; i++)
		for(int j = 0; j < SIZE; j++)
		{
			*(vertices + i * 3 * SIZE + j * 3) = (i - SIZE / 2) / SIZE * 5;
			*(vertices + i * 3 * SIZE + j * 3 + 1) = (position[i * SIZE + j] / 1024) / SIZE * 3;
			*(vertices + i * 3 * SIZE + j * 3 + 2) = (j - SIZE / 2) / SIZE * 5;
		}

	for(int i = 0; i < SIZE; i++)
		for(int j = 0; j < SIZE; j++)
		{
			*(normals + i * 3 * SIZE + j * 3) = position[(i + 1) * SIZE + j] - position[(i - 1) * SIZE + j];
			*(normals + i * 3 * SIZE + j * 3 + 1) = - 4 * SIZE;
			*(normals + i * 3 * SIZE + j * 3 + 2) = position[i * SIZE + j + 1] - position[i * SIZE + j - 1];

			Vector::normalize(normals + i * SIZE + j);
		}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, waterTexture);
	for(int j = 0; j < SIZE - 1; j++)
	{
		glBegin(GL_QUAD_STRIP);
			for(int i = 0; i < SIZE; i++)
			{
				glNormal3f(normals[i * 3 * SIZE + j * 3 + 3], normals[i * 3 * SIZE + j * 3 + 4], normals[i * 3 * SIZE + j * 3 + 5]);
				glTexCoord2f(i / SIZE, (j + 1) / SIZE);
				glVertex3f(vertices[i * 3 * SIZE + j * 3 + 3], vertices[i * 3 * SIZE + j * 3 + 4], vertices[i * 3 * SIZE + j * 3 + 5]);
				glNormal3f(normals[i * 3 * SIZE + j * 3], normals[i * 3 * SIZE + j * 3 + 1], normals[i * 3 * SIZE + j * 3 + 2]);
				glTexCoord2f(i / SIZE, j / SIZE);
				glVertex3f(vertices[i * 3 * SIZE + j * 3], vertices[i * 3 * SIZE + j * 3 + 1], vertices[i * 3 * SIZE + j * 3 + 2]);
			}
	}
	glDisable(GL_TEXTURE_2D);	
}

void Water::Exit()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
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
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	waterTexture = LoadTexture();

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	viscosity = 0.96;
	rainInterval = 1000;
	
	for(int i = 0; i < SIZE; i++)
		for(int j = 0; j < SIZE; j++)
		{
			position[i * SIZE + j] = 0;
			velocity[i * SIZE + j] = 0;
		}
}