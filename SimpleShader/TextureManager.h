#ifndef _TEXMANAGER_H
#define _TEXMANAGER_H

#include <vector>
#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "Texture.h"
#include "settings.h"

using namespace std;

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager(void);

	void bindTexture(GLuint texUnit, int texIndex);
	void unbindTexture(GLuint texUnit, int texIndex);
private:
	vector<Texture*> textures;
};

#endif // _TEXMANAGER_H
