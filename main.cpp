#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "terrain.h"
#include "skybox.h"
#include "billboard.h"
#include "water.h"
#include "3dsloader.h"

using namespace std;

GLfloat lAmbient[] = {0.3,0.3,0.3,1.0};
GLfloat lDiffuse[] = {1.0,1.0,1.0,1.0};
GLfloat lSpecular[] = {1.0,1.0,1.0,1.0};

GLfloat lPosition[] = {0.0,255.0,0.0,0.0};
GLfloat lpAircraft[] = {0.0, 0.0, 0.0, 1.0};
GLfloat ldAircraft[] = {0.0, 0.0, -1.0};

GLfloat mSpecular[] = {1.0,1.0,1.0,1.0};
GLfloat mShininess[] = {128.0};

//colors
GLfloat Black[] = {0.0,0.0,0.0,1.0};
GLfloat Yellow[] = {0.0,0.0,1.0,1.0}; 
GLfloat White[] = {1.0,1.0,1.0,1.0}; 
GLfloat Grey[] = {0.1,0.1,0.1,1.0};
GLfloat LightGrey[] = {0.9,0.9,0.9,1.0};

#define FLY		1
#define WALK	2

#define SIZE	512

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
float* treeList;
Water* water;
int rainCount = 0;
obj_type object;
float deltaAircraftAngleX = 0, aircraftAngleX = 0;
float deltaAircraftAngleZ = 0, aircraftAngleZ = 0;
float deltaAircraftAngle = 0, aircraftAngle = 0;

void init();
float* createTreePositionsList();

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
	gluLookAt(x, y, z, x + lx, y + ly,z + lz, 0.0f,1.0f,0.0f);
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

float* createTreePositionsList()
{
	float* list = new float[3 * 15];
	int i = 0;

	while(i < 45)
	{
		float xT = float(rand()%500) - 250;
		float zT = float(rand()%500) - 250;
		float yT = terrain->GetHeight(xT, zT);

		if(yT > 12 && yT < 20)
		{
			*(list + i) = xT;
			*(list + i + 1) = yT;
			*(list + i + 2) = zT;
			i += 3;
		}		
	}

	return list;
}

void defSources()
{
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0,GL_POSITION,lPosition);

	glLightfv(GL_LIGHT1, GL_POSITION, lpAircraft);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, ldAircraft);
}

void defMaterial()
{	
	glMaterialfv(GL_FRONT, GL_SPECULAR, mSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS,mShininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, White);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, White);
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
	
	if(deltaAircraftAngle)
		aircraftAngle += deltaAircraftAngle;

	if(deltaAircraftAngleX)
		aircraftAngleX += deltaAircraftAngleX;
	if(deltaAircraftAngleZ)
		aircraftAngleZ += deltaAircraftAngleZ;
	if(aircraftAngle > 45)
		aircraftAngle = 45;
	else if(aircraftAngle < - 45)
		aircraftAngle = - 45;

	if(x < -255)
		x = -255;
	else if(x > 255)
		x = 255;
	if(z < -255)
		z = -255;
	else if(z > 255)
		z = 255;
	if(y > 255)
		y = 255;

	glLoadIdentity();
	if(navigationMode == WALK)
	{
		y = terrain->GetHeight(x, z) + 30.0f;
		gluLookAt(x, y, z, x + 10*lx,y + 10*ly,z + 10*lz, 0.0f,1.0f,0.0f);
	}
	else if(navigationMode == FLY)
	{
		if(y <= (terrain->GetHeight(x, z) + 30.0f))
			y = terrain->GetHeight(x, z) + 30.0f;
		gluLookAt(x, y, z, x + 10*lx,y + 10*ly,z + 10*lz, 0.0f,1.0f,0.0f);
	}
	
	//gluLookAt(5, 85, 10, 5, 85, 0, 0, 1, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//Draw skybox

	skybox->Draw();

	// Draw lights

	defMaterial();

	defSources();
	
	//Draw ground

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glCallList(terrainDL);

	//Draw billboard

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, treeTex);

	/*billboard->CylindricalBegin(x, y, z, 5, 10, 15);
	glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(5, 10, 15);
			glTexCoord2f(1, 0);
			glVertex3f(15, 10, 15);
			glTexCoord2f(1, 1);
			glVertex3f(15, 20, 15);
			glTexCoord2f(0, 1);
			glVertex3f(5, 20, 15);
	glEnd();
	
	billboard->End();*/

	
	for(int i = 0; i < 45; i += 3)
	{
		glPushMatrix();
		glTranslatef(treeList[i], treeList[i + 1], treeList[i + 2]);
		billboard->CylindricalBegin(x, y, z, treeList[i], treeList[i + 1], treeList[i + 2]);

		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(0, 0, 0);
			glTexCoord2f(1, 0);
			glVertex3f(10, 0, 0);
			glTexCoord2f(1, 1);
			glVertex3f(10, 10, 0);
			glTexCoord2f(0, 1);
			glVertex3f(0, 10, 0);
		glEnd();
		
		billboard->End();
		glPopMatrix();
	}

	//Draw aircraft

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, object.id_texture);
	
	glTranslatef(x + 10 * lx, y + 10 * ly, z + 10 * lz);
	glRotatef(90 , 1, 0, 0);
	glRotatef(180, 0, 1, 0);
	glRotatef(-aircraftAngle * 180, 0, 1, 0);

	if(aircraftAngle < 0)
		glRotatef(aircraftAngle * 180, 1, 0, 0);
	else
		glRotatef(-aircraftAngle * 180, 1, 0, 0);
	glRotatef(aircraftAngleX, 1, 0, 0);
	glRotatef(aircraftAngleZ, 0, 0, 1);
	glScalef(0.01, 0.01, 0.01);

    glBegin(GL_TRIANGLES);
    for (int index=0; index < object.polygons_qty; index++)
    {
        glTexCoord2f( object.mapcoord[ object.polygon[index].a ].u,
                      object.mapcoord[ object.polygon[index].a ].v);
        glVertex3f( object.vertex[ object.polygon[index].a ].x,
                    object.vertex[ object.polygon[index].a ].y,
                    object.vertex[ object.polygon[index].a ].z);

        glTexCoord2f( object.mapcoord[ object.polygon[index].b ].u,
                      object.mapcoord[ object.polygon[index].b ].v);
        glVertex3f( object.vertex[ object.polygon[index].b ].x,
                    object.vertex[ object.polygon[index].b ].y,
                    object.vertex[ object.polygon[index].b ].z);

        glTexCoord2f( object.mapcoord[ object.polygon[index].c ].u,
                      object.mapcoord[ object.polygon[index].c ].v);
        glVertex3f( object.vertex[ object.polygon[index].c ].x,
                    object.vertex[ object.polygon[index].c ].y,
                    object.vertex[ object.polygon[index].c ].z);
    }
    glEnd();
	glPopMatrix();

	//Draw water

	/*glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, water->waterTexture);
	glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(0, 60, 0);
			glTexCoord2f(1, 0);
			glVertex3f(10, 60, 0);
			glTexCoord2f(1, 1);
			glVertex3f(10, 70, 0);
			glTexCoord2f(0, 1);
			glVertex3f(0, 70, 0);
	glEnd();*/
	
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	
	water->Draw();
	water->Exit();

	//Draw axis

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_LIGHTING);
	/*glColor3f(1.0, 0.0, 0.0);
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
	glEnd();*/

	//Draw strings

	glDisable(GL_BLEND);
	frame++;
	time=glutGet(GLUT_ELAPSED_TIME);
	float fps = frame * 1000.0/(time - timebase);
	rainCount++;
	if(rainCount > fps)
		rainCount += 1000;
	if(rainCount >= 5000)
	{
		water->CreateRainDrop();
		rainCount = 0;
	}
	if (time - timebase > 1000) 
	{
		sprintf(s,"FPS:%4.2f", fps);
		timebase = time;		
		frame = 0;
	}
	glDisable(GL_LIGHTING);
	glColor3f(0.0f,1.0f,1.0f);
	setOrthographicProjection();
	glPushMatrix();
	glLoadIdentity();
	renderBitmapString(30,30,(void *)font,s); 
	renderBitmapString(30,54,(void *)font,"Esc - Quit");
	char s2[60];
	sprintf(s2, "x: %3.2f %t y: %3.2f %t z: %3.2f", x, y, z);
	renderBitmapString(30, 42, (void*) font, s2);
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
				deltaAircraftAngle = deltaAngle;
				break;
			case 100: 
				deltaAngle = 0.005f;
				deltaAircraftAngle = deltaAngle;
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
			{
				deltaAngle = 0.0f;
				deltaAircraftAngle = deltaAngle;
			}
			break;
		case 100: 
			if (deltaAngle > 0.0f)
			{
				deltaAngle = 0.0f;
				deltaAircraftAngle = deltaAngle;
			}
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
			deltaAircraftAngleZ = 1;
			break;
		case GLUT_KEY_RIGHT: 
			deltaAircraftAngleZ = -1;
			break;
		case GLUT_KEY_UP: 
			deltaAircraftAngleX = 1;
			break;
		case GLUT_KEY_DOWN :
			deltaAircraftAngleX = -1;
			break;
	}
}

void releaseKey(int key, int x, int y) 
{
	switch (key) 
	{
		case GLUT_KEY_LEFT: 
			if(deltaAircraftAngleZ > 0) 
				deltaAircraftAngleZ = 0;
				break;
		case GLUT_KEY_RIGHT: 
			if(deltaAircraftAngleZ < 0) 
				deltaAircraftAngleZ = 0;
				break;
		case GLUT_KEY_UP:	 
			if(deltaAircraftAngleX > 0) 
				deltaAircraftAngleX = 0;
				break;
		case GLUT_KEY_DOWN: 
			if(deltaAircraftAngleX < 0) 
				deltaAircraftAngleX = 0;
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
		deltaX = x;
		deltaY = y;
	} 
	else if (state == GLUT_UP) 
	{
		angleY = angle2Y;
		angle = angle2;
	}
}

void defModel()
{
	GLfloat color[4];
	color[0] = 0.5;
	color[1] = 0.5;
	color[2] = 0.5;
	color[3] = 1;
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
}

void initSources()
{
	glLightfv(GL_LIGHT1, GL_AMBIENT, Yellow);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Yellow);
	glLightfv(GL_LIGHT1, GL_SPECULAR, Yellow);

	GLfloat exponent[] = {120.0};
	glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, exponent);
	GLfloat cutoff[] = {30.0};
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, cutoff);

	glLightfv(GL_LIGHT0,GL_AMBIENT,lAmbient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,lDiffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,lSpecular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT0);
}

void init() 
{
	skybox = new Skybox();
	skybox->LoadTextures();

	terrain = new Terrain();

	water = new Water();
	water->Init();

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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);

	//
	Load3DS(&object, "Data/Aircraft/spaceship.3ds");
	object.id_texture = Bitmap::LoadTexture("Data/Aircraft/spaceshiptexture.bmp");
	//
	terrainDL = terrain->Create(0,0,0);
	y = terrain->GetHeight(0,0) + 5.0f;
	//
	treeTex = billboard->LoadTexture();
	treeList = createTreePositionsList();
	//

	defModel();

	initSources();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(1200,800);
	glutCreateWindow("HeightMap");

	if (terrain->LoadFromImage("Data/Heightmaps/heightmap.bmp",1) != TERRAIN_OK)
		return(-1);
	terrain->Scale(0,40);
	init();

	glutMainLoop();

	return 0;
}
