#include "skybox.h"

int Skybox::LoadTextures()
{
	tabTextures = new GLuint[6];

	textures = new Bitmap[6];

	glGenTextures(6, tabTextures);

	textures[0].LoadBmp("Data/skybox1/XN.bmp", 0);
	textures[1].LoadBmp("Data/skybox1/XP.bmp", 0);
	textures[2].LoadBmp("Data/skybox1/YN.bmp", 0);
	textures[3].LoadBmp("Data/skybox1/YP.bmp", 0);
	textures[4].LoadBmp("Data/skybox1/ZN.bmp", 0);
	textures[5].LoadBmp("Data/skybox1/ZP.bmp", 0);

    for(int i=0;i<6;i++) 
	{
		glBindTexture(GL_TEXTURE_2D,tabTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );        

		glTexImage2D(GL_TEXTURE_2D, 0, 4, textures[i].width, textures[i].height, 0, GL_RGB, GL_UNSIGNED_BYTE, textures[i].data);
    }
    return 0;
}

void Skybox::Draw()
{
	float t=1.0;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);

	GLint* viewport = new GLint[4];
	GLdouble* model = new GLdouble[16];
	GLdouble* proj = new GLdouble[16];

	GLdouble posx, posy, posz;

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	gluUnProject( (viewport[2]-viewport[0])/2, (viewport[3]-viewport[1])/2, 0.0, model, proj, viewport, &posx, &posy, &posz);

	glPushMatrix();
	glTranslatef(posx, posy, posz);

	glColor3f(1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, tabTextures[0]);
	glBegin(GL_QUADS);			// X Négatif	
		glTexCoord2f(0.0, 1.0); glVertex3f(-t,t,-t);	
		glTexCoord2f(1.0, 1.0); glVertex3f(-t,t,t);
		glTexCoord2f(1.0, 0.0); glVertex3f(-t,-t,t);
		glTexCoord2f(0.0, 0.0); glVertex3f(-t,-t,-t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[1]);
	glBegin(GL_QUADS);			// X Positif
		glTexCoord2f(0.0, 1.0); glVertex3f(t,t,t); 
		glTexCoord2f(1.0, 1.0); glVertex3f(t,t,-t);	
		glTexCoord2f(1.0, 0.0); glVertex3f(t,-t,-t);
		glTexCoord2f(0.0, 0.0); glVertex3f(t,-t,t); 
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[2]);
	glBegin(GL_QUADS);			// Y Négatif	
		glTexCoord2f(0.0, 1.0); glVertex3f(t,t,-t); 
		glTexCoord2f(1.0, 1.0); glVertex3f(-t,t,-t);
		glTexCoord2f(1.0, 0.0); glVertex3f(-t,-t,-t);
		glTexCoord2f(0.0, 0.0); glVertex3f(t,-t,-t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[3]);
	glBegin(GL_QUADS);			// Y Positif		
		glTexCoord2f(0.0, 1.0); glVertex3f(-t,t,t);
		glTexCoord2f(1.0, 1.0); glVertex3f(t,t,t);
		glTexCoord2f(1.0, 0.0); glVertex3f(t,-t,t);
		glTexCoord2f(0.0, 0.0); glVertex3f(-t,-t,t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[4]);
	glBegin(GL_QUADS);			// Z Négatif		
		glTexCoord2f(0.0, 0.0); glVertex3f(-t,-t,t);
		glTexCoord2f(1.0, 0.0); glVertex3f(t,-t,t);
		glTexCoord2f(1.0, 1.0); glVertex3f(t,-t,-t);
		glTexCoord2f(0.0, 1.0); glVertex3f(-t,-t,-t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[5]);
	glBegin(GL_QUADS);			// Z Positif	
		glTexCoord2d(0.0, 1.0);	glVertex3f(-t, t, t);
		glTexCoord2d(1.0, 1.0);	glVertex3f(-t, t, -t);
		glTexCoord2d(1.0, 0.0); glVertex3f(t, t, -t);
		glTexCoord2d(0.0, 0.0);	glVertex3f(t, t, t);
	glEnd();

	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	delete[] proj;
	delete[] model;
	delete[] viewport;
}



