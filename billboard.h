#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <iostream>
#include "bitmap.h"
#include "vector.h"

class Billboard
{
public:
	void billboardCylindricalBegin();
	void billboardCylindricalBegin(float camX, float camY, float camZ, float objX, float objY, float objZ);
	void billboardEnd();

	GLuint loadTexture();
};

#endif