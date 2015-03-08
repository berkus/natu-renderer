#pragma once

#include <vector>
#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "settings.h"
#include <string>
#include "LODEpng\lodepng.h"
using namespace LodePNG;

using namespace std;

class Texture
{
public:
	Texture();
	Texture(GLuint _texType, GLuint _inFormat, GLuint _dataFormat, GLuint _dataType, GLvoid * _data, GLsizei _width, GLsizei _height);
	~Texture(void);

	void bindTexture(GLuint texUnit);
	void unbindTexture();

	bool loadTexture(const char * filename, bool makeFloat = false, GLint borderMode = GL_CLAMP, GLint filterMode = GL_LINEAR);

	GLuint	glID;
	void *	data;
	GLenum	type;
	GLenum	dataFormat;
	GLsizei width;
	GLsizei height;
	GLuint  unitOffset;
	GLuint  unitId;

};

