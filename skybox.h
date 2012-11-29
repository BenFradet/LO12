#include "bitmap.h"

using namespace std;

class Skybox
{
public:
	GLuint* tabTextures;
	Bitmap* textures;
	int loadSkybox();
	void drawSkybox();
};