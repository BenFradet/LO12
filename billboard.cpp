#include "billboard.h"

void Billboard::billboardCylindricalBegin()
{
	float modelView[16];
	int i, j;

	glPushMatrix();

	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);

	for(i = 0; i < 3; i += 2)
		for(j = 0; j < 3; j++)
		{
			if(i == j)
				modelView[i * 4 + j] = 1.0f;
			else
				modelView[i * 4 + j] = 0.0f;
		}

	glLoadMatrixf(modelView);
}

void Billboard::billboardEnd()
{
	glPopMatrix();
}

GLuint Billboard::loadTexture()
{
	GLuint texName;

	Bitmap* image = new Bitmap("Data/tree.bmp", 0);
	int w = image->width;
	int h = image->height;

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);

	return texName;
}