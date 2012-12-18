#include "billboard.h"

void Billboard::CylindricalBegin()
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

void Billboard::CylindricalBegin(float camX, float camY, float camZ, float objX, float objY, float objZ)
{
	float lookAt[3], objToCamProj[3], upAux[3];
	float modelView[16], angleCosine;

	glPushMatrix();

	objToCamProj[0] = camX - objX;
	objToCamProj[1] = 0.0f;
	objToCamProj[2] = camZ - objZ;

	lookAt[0] = 0;
	lookAt[1] = 0;
	lookAt[2] = 1;

	Vector::normalize(objToCamProj);

	Vector::crossProduct(upAux, lookAt, objToCamProj);

	angleCosine = Vector::dotProduct(lookAt, objToCamProj);

	if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
      glRotatef(acos(angleCosine)*180/3.14,upAux[0], upAux[1], upAux[2]);
}

void Billboard::End()
{
	glPopMatrix();
}

GLuint Billboard::LoadTexture()
{
	GLuint texName;

	Bitmap* image = new Bitmap("Data/tree.bmp", 0);
	int w = image->width;
	int h = image->height;

	GLubyte* tex = new GLubyte[w * h * 4];
	int j = 0;

	for(int i = 0; i < w * h * 3; i += 3)
	{
		*(tex + i + j) = *(image->data + i);
		*(tex + i + 1 + j) = *(image->data + i + 1);
		*(tex + i + 2 + j) = *(image->data + i + 2);
		j++;
		//if((*(image->data + i) - *(image->data + i + 1)) < 5 && (*(image->data + i) - *(image->data + i + 2)) < 5)
		if(*(image->data + i) > 80 && *(image->data + i + 1) > 80 && *(image->data + i + 2) > 80)
			*(tex + i + 2 + j) = 0;
		else
			*(tex + i + 2 + j) = 1;
	}

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);

	delete image;
	delete[] tex;

	return texName;
}