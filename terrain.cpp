#include "terrain.h"

int Terrain::width = 0;
int Terrain::length = 0;
float* Terrain::heights = NULL;
float* Terrain::colors = NULL;
float* Terrain::normals = NULL;

float* Terrain::CrossProduct(int x1,int z1,int x2,int z2,int x3,int z3) 
{
	float *auxNormal,v1[3],v2[3];
		
	v1[0] = x2-x1; 
	v1[1] = -heights[z1 * width + x1] 
			+ heights[z2 * width + x2];
	v1[2] = z2-z1; 


	v2[0] = x3-x1; 
	v2[1] = -heights[z1 * width + x1] 
			+ heights[z3 * width + x3];
	v2[2] = z3-z1; 

	auxNormal = (float *)malloc(sizeof(float)*3);

	auxNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];
	auxNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
	auxNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];

	return(auxNormal);
}

void Terrain::NormalizeVector(float *v) 
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

void Terrain::AddVector(float *a, float *b) 
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

void Terrain::ComputeNormals() 
{
	float *norm1,*norm2,*norm3,*norm4; 
	int i,j,k;
	
	if (normals == NULL)
		return;

	for(i = 0; i < length; i++)
		for(j = 0; j < width; j++) {
			norm1 = NULL;
			norm2 = NULL;
			norm3 = NULL;
			norm4 = NULL;

			// normals for the four corners
			if (i == 0 && j == 0) {
				norm1 = CrossProduct(0,0, 0,1, 1,0);	
				NormalizeVector(norm1);				
			}
			else if (j == width-1 && i == length-1) {
				norm1 = CrossProduct(i,j, j,i-1, j-1,i);	
				NormalizeVector(norm1);				
			}
			else if (j == 0 && i == length-1) {
				norm1 = CrossProduct(i,j, j,i-1, j+1,i);	
				NormalizeVector(norm1);				
			}
			else if (j == width-1 && i == 0) {
				norm1 = CrossProduct(i,j, j,i+1, j-1,i);	
				NormalizeVector(norm1);				
			}

			// normals for the borders
			else if (i == 0) {
				norm1 = CrossProduct(j,0, j-1,0, j,1);
				NormalizeVector(norm1);
				norm2 = CrossProduct(j,0,j,1,j+1,0);
				NormalizeVector(norm2);
			}
			else if (j == 0) {
				norm1 = CrossProduct(0,i, 1,i, 0,i-1);
				NormalizeVector(norm1);
				norm2 = CrossProduct(0,i, 0,i+1, 1,i);
				NormalizeVector(norm2);
			}
			else if (i == length) {
				norm1 = CrossProduct(j,i, j,i-1, j+1,i);
				NormalizeVector(norm1);
				norm2 = CrossProduct(j,i, j+1,i, j,i-1);
				NormalizeVector(norm2);
			}
			else if (j == width) {
				norm1 = CrossProduct(j,i, j,i-1, j-1,i);
				NormalizeVector(norm1);
				norm2 = CrossProduct(j,i, j-1,i, j,i+1);
				NormalizeVector(norm2);
			}

			// normals for the interior
			else {
				norm1 = CrossProduct(j,i, j-1,i, j,i+1);
				NormalizeVector(norm1);
				norm2 = CrossProduct(j,i, j,i+1, j+1,i);
				NormalizeVector(norm2);
				norm3 = CrossProduct(j,i, j+1,i, j,i-1);
				NormalizeVector(norm3);
				norm4 = CrossProduct(j,i, j,i-1, j-1,i);
				NormalizeVector(norm4);
			}
			if (norm2 != NULL) {
				AddVector(norm1,norm2);
				free(norm2);
			}
			if (norm3 != NULL) {
				AddVector(norm1,norm3);
				free(norm3);
			}
			if (norm4 != NULL) {
				AddVector(norm1,norm4);
				free(norm4);
			}
			NormalizeVector(norm1);
			norm1[2] = - norm1[2];
			for (k = 0; k< 3; k++) 
				normals[3*(i*width + j) + k] = norm1[k];
			free(norm1);
		}

}

int Terrain::LoadFromImage(char* filename, int normal)
{
	Bitmap* bm = new Bitmap(filename, 1);
	int mode;
	float pointHeight;

	if(heights != NULL)
		Destroy();

	if(!bm->loadBmp(filename, 1))
		return (TERRAIN_ERROR_LOADING_IMAGE);

	mode = bm->bpp / 8;

	if(mode != 3)
		return TERRAIN_ERROR_LOADING_IMAGE;

	width = bm->width;
	length = bm->height;

	heights = (float*)malloc(width * length * sizeof(float));
	if(heights == NULL)
		return (TERRAIN_ERROR_MEMORY_PROBLEM);

	if(normal)
	{
		normals = (float*)malloc(width * length * sizeof(float) * 3);
		if(normals == NULL)
			return (TERRAIN_ERROR_MEMORY_PROBLEM);
	}
	else
		normals = NULL;

	for(int i = 0; i<width; i++)
		for(int j = 0; j<length; j++)
		{
			pointHeight = (float)bm->data[i*width + j] / 256.0;
			heights[i * width + j] = pointHeight;
		}

	if(normals)
		ComputeNormals();

	return TERRAIN_OK;
}

int Terrain::Scale(float min,float max) 
{
	float amp,aux,min1,max1,height;
	int total,i;

	if (heights == NULL)
		return(TERRAIN_ERROR_NOT_INITIALISED);

	if (min > max) 
	{
		aux = min;
		min = max;
		max = aux;
	}

	amp = max - min;
	total = width * length;

	min1 = heights[0];
	max1 = heights[0];
	for(i=1;i < total ; i++) 
	{
		if (heights[i] > max1)
			max1 = heights[i];
		if (heights[i] < min1)
			min1 = heights[i];
	}
	for(i=0;i < total; i++) 
	{
		height = (heights[i] - min1) / (max1-min1);
		heights[i] = height * amp - min;
	}
	printf("%f %f %f %f %f %f\n",min,max,min1,max1,amp,height);
	if (normals != NULL)
		ComputeNormals();
	return(TERRAIN_OK);
}
	

int Terrain::Create(float xOffset, float yOffset, float zOffset) 
{
	GLuint terrainDL;
	float startW,startL;

	GLuint texName = LoadTextures();

	startW = width / 2.0 - width;
	startL = - length / 2.0 + length;

	terrainDL = glGenLists(1);

	glNewList(terrainDL,GL_COMPILE);
	if (normals != NULL && colors != NULL) 
	{
		glColorMaterial(GL_FRONT, GL_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);
	}

	for (int i = 0 ; i < length-1; i++) 
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texName);

		glBegin(GL_QUADS);
		glTexCoord2f(1, 0);
		glVertex3f(10, 80, 0);
		glTexCoord2f(1, 1);
		glVertex3f(10, 90, 0);
		glTexCoord2f(0, 1);
		glVertex3f(0, 90, 0);
		glTexCoord2f(0, 0);
		glVertex3f(0, 80, 0);
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0;j < width; j++) 
		{
		
			if (colors != NULL) 
				glColor3f(colors[3*((i+1)*width + j)], colors[3*((i+1)*width + j)+1], colors[3*((i+1)*width + j)+2]);
			if (normals != NULL)
				glNormal3f(normals[3*((i+1)*width + j)], normals[3*((i+1)*width + j)+1], normals[3*((i+1)*width + j)+2]);

			if (colors != NULL) 
				glColor3f(colors[3*(i*width + j)], colors[3*(i*width + j)+1], colors[3*(i*width + j)+2]);
			if (normals != NULL)
				glNormal3f(normals[3*(i*width + j)], normals[3*(i*width + j)+1], normals[3*(i*width + j)+2]);

			glTexCoord2f((i+1), j);
			glVertex3f(startW + j + xOffset, heights[(i+1)*width + (j)] + yOffset, startL - (i+1) + zOffset);

			glTexCoord2f(i, j);
			glVertex3f(startW + j + xOffset, heights[(i)*width + j] + yOffset, startL - i + zOffset);
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	glEndList();
	return(terrainDL);
}

GLuint Terrain::LoadTextures()
{
	float* percent = new float[4];
	double r, g, b;

	GLuint texName;

	int max = 0, min = 512;
	
	Bitmap* dirt = new Bitmap("terrainTextures/dirt.bmp", 0);
	Bitmap* grass = new Bitmap("terrainTextures/grass.bmp", 0);
	Bitmap* rock = new Bitmap("terrainTextures/rock.bmp", 0);
	Bitmap* snow = new Bitmap("terrainTextures/snow.bmp", 0);

	int w = dirt->width, h = dirt->height;

	GLubyte* tex = new GLubyte[w * h * 3];

	if(dirt == NULL || grass == NULL || rock == NULL || snow == NULL)
		cout<<"Probleme avec l'allocation des textures"<<endl;
	for(int i = 0; i < h; i++)
		for(int j = 0; j < w; j++)
		{
			WeightsBlending(percent, (int)heights[j*w + i]);

			if(heights[j*w + i] < min)
				min = heights[j*w + i];
			if(heights[j*w + i] > max)
				max = heights[j*w + i];

			r = percent[0] * dirt->data[j * 3 * w + i*3];
			g = percent[0] * dirt->data[j * 3 * w + i*3 + 1];
			b = percent[0] * dirt->data[j * 3 * w + i*3 + 2];

			r += percent[1] * grass->data[j * 3 * w + i * 3];
			g += percent[1] * grass->data[j * 3 * w + i * 3 + 1];
			b += percent[1] * grass->data[j * 3 * w + i * 3 + 2];

			r += percent[2] * rock->data[j * 3 * w + i * 3];
			g += percent[2] * rock->data[j * 3 * w + i * 3 + 1];
			b += percent[2] * rock->data[j * 3 * w + i * 3 + 2];

			r += percent[3] * snow->data[j * 3 * w + i * 3];
			g += percent[3] * snow->data[j * 3 * w + i * 3 + 1];
			b += percent[3] * snow->data[j * 3 * w + i * 3 + 2];

			tex[j * 3 * w + i * 3] = r;
			tex[j * 3 * w + i * 3 + 1] = g;
			tex[j * 3 * w + i * 3 + 2] = b;
		}

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, length, 0, GL_RGB, GL_UNSIGNED_BYTE, tex);

	cout<<"MAXH="<<max<<" MINH="<<min<<endl;

	delete dirt;
	delete grass;
	delete rock;
	delete snow;

	return texName;
}

float Terrain::GetHeight(int x, int z) 
{
	int xt,zt;

	if (heights == NULL) 
			return(0.0);

	xt = x + width /2;
	zt = width - (z + length /2);

	if ((xt > width) || (zt > length) || (xt < 0) || (zt < 0))
		return(0.0);

	return(heights[zt * width + xt]);
}


void Terrain::Destroy() 
{
	if (heights != NULL)
		free(heights);

	if (colors != NULL)
		free(colors);

	if (normals != NULL)
		free(normals);
}

void Terrain::WeightsBlending(float* percent, unsigned char height)
{
	/*int add = height + (rand()%30) - 15;

	if(add < 0)
		add = 0;
	if(add > 255)
		add = 255;

	height = add;*/

	//all dirt
	if(height < 4)
	{
		percent[0] = 1.0f;
		percent[1] = 0.0f;
		percent[2] = 0.0f;
		percent[3] = 0.0f;
	}
	/*else if(height < 10)
	{
		percent[0] = 1.0f - (height - 2.5f)/1.5f;
		percent[1] = (height - 2.5f)/1.5f;
		percent[2] = 0.0f;
		percent[3] = 0.0f;
	}//all grass*/
	else if(height < 12)
	{
		percent[0] = 0.0f;
		percent[1] = 1.0f;
		percent[2] = 0.0f;
		percent[3] = 0.0f;
	}/*
	else if(height < 18)
	{
		percent[0] = 0.0f;
		percent[1] = 1.0f - (height - 9.0f)/7.0f;
		percent[2] = (height - 9.0f)/7.0f;
		percent[3] = 0.0f;
	}//all rock*/
	else if(height < 20)
	{
		percent[0] = 0.0f;
		percent[1] = 0.0f;
		percent[2] = 1.0f;
		percent[3] = 0.0f;
	}/*
	else if(height < 27)
	{
		percent[0] = 0.0f;
		percent[1] = 0.0f;
		percent[2] = 1.0f - (height - 15.0f)/12.0f;
		percent[3] = (height - 15.0f)/12.0f;
	}//all snow*/
	else
	{
		percent[0] = 0.0f;
		percent[1] = 0.0f;
		percent[2] = 0.0f;
		percent[3] = 1.0f;
	}
}