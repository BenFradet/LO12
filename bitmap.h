#ifndef BITMAP_H
#define BITMAP_H

#include <iostream>
#include <cstdio>
#include <string>

#include <glut.h>
#include <gl\gl.h>
#include <gl\glu.h>

using namespace std;

#define _CRT_SECURE_NO_DEPRECATE

const short BITMAP_MAGIC_NUMBER = 19778;
const int RGB_BYTE_SIZE = 3;

#pragma pack(push, bitmap_data, 1)

typedef struct tagRGBQuad
{
	char rgbBlue;
	char rgbGreen;
	char rgbRed;
	char rgbReserved;
}RGBQuad;

typedef struct tagBitmapFileHeader
{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
}BitmapFileHeader;

typedef struct tagBitmapInfoHeader
{
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
}BitmapInfoHeader;

#pragma pack(pop, bitmap_data)

class Bitmap
{
public:
	RGBQuad* colors;
	unsigned char* data;
	bool loaded;
	int width, height;
	unsigned short bpp;
	string error;
	char* filename;

	Bitmap(void);
	Bitmap(char*, int);
	~Bitmap();
	bool loadBmp(char*, int);
	void convertToGray(unsigned char*);
	bool loadTexture();
	
private:
	BitmapFileHeader bmfh;
	BitmapInfoHeader bmih;
	int byteWidth;
	int padWidth;
	unsigned int dataSize;

	void reset(void);
	bool convert24(unsigned char*);
	bool convert8(unsigned char*);
};

#endif