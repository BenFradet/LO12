#include "skybox.h"

int Skybox::loadSkybox()
{
	tabTextures = new GLuint[6];

	textures = new Bitmap[6];

	glGenTextures(6, tabTextures);

	textures[0].loadBmp("skybox1/XN.bmp", 0);
	/*textures[1].loadBmp("skybox1/XP.bmp", 0);
	textures[2].loadBmp("skybox1/YN.bmp", 0);
	textures[3].loadBmp("skybox1/YP.bmp", 0);
	textures[4].loadBmp("skybox1/ZN.bmp", 0);
	textures[5].loadBmp("skybox1/ZP.bmp", 0);*/

    for(int i=0;i<1;i++) 
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

void Skybox::drawSkybox()
{
	float t=20.0;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();

	/*//glLoadIdentity();

	switch(type_obs)

	{

	case 1://lookAt

		glTranslatef(lookAtParam[3], lookAtParam[4], lookAtParam[5]);

		break;

	case 2://polarView

		glTranslatef(x, y, z);

		break;

	case 3://pilotView

		glTranslatef(pilotParam[0], pilotParam[1], pilotParam[2]);

		break;

	}*/

	glBindTexture(GL_TEXTURE_2D, tabTextures[0]);
	glBegin(GL_QUADS);			// X Négatif		
		glTexCoord2f(0.0, 0.0); glVertex3f(-t,-t,-t);
		glTexCoord2f(1.0, 0.0); glVertex3f(-t,t,-t);
		glTexCoord2f(1.0, 1.0); glVertex3f(-t,t,t);
		glTexCoord2f(0.0, 1.0); glVertex3f(-t,-t,t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[1]);
	glBegin(GL_QUADS);			// X Positif
		glTexCoord2f(0.0, 0.0); glVertex3f(t,t,-t); 	
		glTexCoord2f(1.0, 0.0); glVertex3f(t,-t,-t);
		glTexCoord2f(1.0, 1.0); glVertex3f(t,-t,t);
		glTexCoord2f(0.0, 1.0); glVertex3f(t,t,t); 
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[2]);
	glBegin(GL_QUADS);			// Y Négatif	
		glTexCoord2f(0.0, 0.0); glVertex3f(t,-t,-t);
		glTexCoord2f(1.0, 0.0); glVertex3f(-t,-t,-t);
		glTexCoord2f(1.0, 1.0); glVertex3f(-t,-t,t);
		glTexCoord2f(0.0, 1.0); glVertex3f(t,-t,t); 
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[3]);
	glBegin(GL_QUADS);			// Y Positif		
		glTexCoord2f(0.0, 0.0); glVertex3f(-t,t,-t);
		glTexCoord2f(1.0, 0.0); glVertex3f(t,t,-t);
		glTexCoord2f(1.0, 1.0); glVertex3f(t,t,t);
		glTexCoord2f(0.0, 1.0); glVertex3f(-t,t,t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[4]);
	glBegin(GL_QUADS);			// Z Négatif		
		glTexCoord2f(0.0, 0.0); glVertex3f(-t,t,-t);
		glTexCoord2f(1.0, 0.0); glVertex3f(t,t,-t);
		glTexCoord2f(1.0, 1.0); glVertex3f(t,-t,-t);
		glTexCoord2f(0.0, 1.0); glVertex3f(-t,-t,-t);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tabTextures[5]);
	glBegin(GL_QUADS);			// Z Positif	
		glTexCoord2d(0.0, 0.0);	glVertex3f(t, t, t);
		glTexCoord2d(1.0, 0.0); glVertex3f(t, -t, t);
		glTexCoord2d(1.0, 1.0);	glVertex3f(-t, -t, t);
		glTexCoord2d(0.0, 1.0);	glVertex3f(-t, t, t);
	glEnd();

	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}



