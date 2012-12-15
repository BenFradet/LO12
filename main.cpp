#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "terrain.h"
#include "skybox.h"
#include "billboard.h"

using namespace std;

// stuff for lighting
GLfloat lAmbient[] = {0.3,0.3,0.3,1.0};
GLfloat lDiffuse[] = {1.0,1.0,1.0,1.0};
GLfloat lSpecular[] = {1.0,1.0,1.0,1.0};

GLfloat lPosition[] = {0.0,100.0,0.0,1.0};

// materials
GLfloat mSpecular[] = {1.0,1.0,1.0,1.0};
// the smaller the larger the specular area is
GLfloat mShininess[] = {128.0};

//colors
GLfloat cBlack[] = {0.0,0.0,0.0,1.0};
GLfloat cOrange[] = {1.0,0.5,0.5,1.0}; 
GLfloat cWhite[] = {1.0,1.0,1.0,1.0}; 
GLfloat cGrey[] = {0.1,0.1,0.1,1.0};
GLfloat cLightGrey[] = {0.9,0.9,0.9,1.0};

#define FLY		1
#define WALK	2

int navigationMode = FLY;
float angle=0.0,deltaAngle = 0.0,ratio;
float x=0.0f,y=1.75f,z=5.0f;
float lx=0.0f,ly=0.0f,lz=-1.0f,deltaMove=0.0;
int h,w;
int font=(int)GLUT_BITMAP_8_BY_13;
int bitmapHeight=13;
int mode;
float angle2,angle2Y,angleY;
static int deltaX=-1000,deltaY;

int terrainDL,iterations = 0,totalIterations = 0;
char s[60];

int frame,time,timebase=0;

Skybox* skybox;
Terrain* terrain;
Billboard* billboard;
GLuint treeTex;

void init();

void changeSize(int w1, int h1)
{
	if(h1 == 0)
		h1 = 1;

	w = w1;
	h = h1;
	ratio = 1.0f * w / h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
    glViewport(0, 0, w, h);

	gluPerspective(45,ratio,0.1,1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z, 
		      x + lx,y + ly,z + lz,
			  0.0f,1.0f,0.0f);
}

void initScene() 
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	terrainDL = terrain->Create(0,0,0);
	y = terrain->GetHeight(0,0) + 5.0f;

	//
	billboard->loadTexture();
	//

	glLightfv(GL_LIGHT0,GL_AMBIENT,lAmbient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,lDiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,lSpecular);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void orientMe(float ang) 
{
	lx = sin(ang);
	lz = -cos(ang);
}


void moveMeFlat(float i) 
{
	x = x + i*lx;
	z = z + i*lz;
	y = y + i*ly;
}

void setOrthographicProjection() 
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glScalef(1, -1, 1);
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
}

void resetPerspectiveProjection() 
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void renderBitmapString(float x, float y, void *font,char *string)
{  
	char *c;
	glRasterPos2f(x, y);
	for (c=string; *c != '\0'; c++)
		glutBitmapCharacter(font, *c);
}


void renderScene(void) 
{
	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaAngle) 
	{
		angle += deltaAngle;
		orientMe(angle);
	}

	if(x < -255)
		x = -255;
	else if(x > 255)
		x = 255;
	if(z < -255)
		z = -255;
	else if(z > 255)
		z = 255;

	glLoadIdentity();
	if(navigationMode == WALK)
	{
		y = terrain->GetHeight(x, z) + 5.0f;
		gluLookAt(x, y, z, x + 10*lx,y + 10*ly,z + 10*lz, 0.0f,1.0f,0.0f);
	}
	else if(navigationMode == FLY)
	{
		if(y <= (terrain->GetHeight(x, z) + 0.5f))
			y = terrain->GetHeight(x, z) + 0.5f;
		gluLookAt(x, y, z, x + 10*lx,y + 10*ly,z + 10*lz, 0.0f,1.0f,0.0f);
	}
	
	//gluLookAt(5, 85, 10, 5, 85, 0, 0, 1, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	skybox->drawSkybox();

	glLightfv(GL_LIGHT0,GL_POSITION,lPosition);

	//Draw axis

	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(100.0, 0.0, 0.0);
	glEnd();
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 100.0, 0.0);
	glEnd();
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 100.0);
	glEnd();
	glEnable(GL_LIGHTING);

	//Draw ground

	glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS,mShininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, cWhite);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, cWhite);
	glCallList(terrainDL);

	//Draw billboard

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
	glBindTexture(GL_TEXTURE_2D, treeTex);
	glEnable(GL_TEXTURE_2D);

	billboard->billboardCylindricalBegin();

	int yTree = terrain->GetHeight(10, 10) + 10;
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(10, yTree, 10);
		glTexCoord2f(1, 0);
		glVertex3f(10, yTree + 100, 10);
		glTexCoord2f(1, 1);
		glVertex3f(110, yTree + 100, 10);
		glTexCoord2f(0, 1);
		glVertex3f(110, yTree, 10);
	glEnd();

	billboard->billboardEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	//

	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	if (time - timebase > 1000) 
	{
		sprintf(s,"FPS:%4.2f",frame*1000.0/(time-timebase));
		timebase = time;		
		frame = 0;
	}
	glDisable(GL_LIGHTING);
	glColor3f(0.0f,1.0f,1.0f);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(30,30,(void *)font,s); 
	renderBitmapString(30,40,(void *)font,"Esc - Quit");
	glPopMatrix();
	resetPerspectiveProjection();
	glEnable(GL_LIGHTING);
	glutSwapBuffers();
}

void processNormalKeys(unsigned char key, int x, int y) 
{	
	if(key == 119 && navigationMode == FLY)
		navigationMode = WALK;
	if(key == 102 && navigationMode == WALK)
		navigationMode = FLY;
	else
	{
		switch (key) 
		{
			case 113: 
				deltaAngle = -0.005f;
				break;
			case 100: 
				deltaAngle = 0.005f;
				break;
			case 122: 
				deltaMove = 1;
				break;
			case 115: 
				deltaMove = -1;
				break;
		}
	}
	if (key == 27) 
	{
		terrain->Destroy();
		exit(0);
	}
}

void releaseNormalKeys(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 113: 
			if (deltaAngle < 0.0f) 
				deltaAngle = 0.0f;
			break;
		case 100: 
			if (deltaAngle > 0.0f) 
				deltaAngle = 0.0f;
			break;
		case 122:
			if (deltaMove > 0) 
				deltaMove = 0;
			break;
		case 115:
			if (deltaMove < 0) 
				deltaMove = 0;
			break;
	}
}

void pressKey(int key, int x, int y) 
{
	switch (key) 
	{
		case GLUT_KEY_LEFT: 
			deltaAngle = -0.005f;
			break;
		case GLUT_KEY_RIGHT: 
			deltaAngle = 0.005f;
			break;
		case GLUT_KEY_UP: 
			deltaMove = 1;
			break;
		case GLUT_KEY_DOWN :
			deltaMove = -1;
			break;
	}
}

void releaseKey(int key, int x, int y) 
{
	switch (key) 
	{
		case GLUT_KEY_LEFT : if (deltaAngle < 0.0f) 
								 deltaAngle = 0.0f;
							 break;
		case GLUT_KEY_RIGHT : if (deltaAngle > 0.0f) 
								 deltaAngle = 0.0f;
							 break;
		case GLUT_KEY_UP :	 if (deltaMove > 0) 
								 deltaMove = 0;
							 break;
		case GLUT_KEY_DOWN : if (deltaMove < 0) 
								 deltaMove = 0;
							 break;
	}
}


void activeMouseMotion(int x, int y) 
{
		angle2 = angle + (x-deltaX)*0.001;
		angle2Y = angleY + (y-deltaY) * 0.001;
		if (angle2Y > 1.57)
			angle2Y = 1.57;
		else if (angle2Y < -1.57)
			angle2Y = -1.57;
		lx = cos(angle2Y)*sin(angle2);
		lz = -cos(angle2Y)*cos(angle2);
		ly = -sin(angle2Y);
}


void mousePress(int button, int state, int x, int y) 
{
	if (state == GLUT_DOWN) 
	{
		//		angle2 = 0;
		deltaX = x;
		deltaY = y;
//		angle2Y = 0;
	} 
	else if (state == GLUT_UP) 
	{
		angleY = angle2Y;
		angle = angle2;
	}
}


void init() 
{
	skybox = new Skybox();
	skybox->loadSkybox();

	terrain = new Terrain();

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(processNormalKeys);
	glutKeyboardUpFunc(releaseNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	glutMotionFunc(activeMouseMotion);
	glutMouseFunc(mousePress);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	initScene();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1200,800);
	glutCreateWindow("HeightMap");

	if (terrain->LoadFromImage("Data/heightmaps/heightmap.bmp",1) != TERRAIN_OK)
		return(-1);
	terrain->Scale(0,40);
	init();

	glutMainLoop();

	return 0;
}
