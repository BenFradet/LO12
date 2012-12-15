#ifndef BILLBOARD_H
#define BILLBOARD_H

#include <iostream>
#include "bitmap.h"

class Billboard
{
public:
	void billboardCylindricalBegin();
	void billboardEnd();

	GLuint loadTexture();
};

#endif