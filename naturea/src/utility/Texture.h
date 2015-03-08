#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "settings.h"
#include "../common/png.h"
using namespace std;

class Texture
{
public:
	Texture(void);
	Texture(string _inShaderName);
	Texture(GLuint _texType, GLuint _inFormat, GLuint _dataFormat, GLuint _dataType, GLvoid * _data, GLsizei _width, GLsizei _height, string _inShaderName);
	~Texture(void);
	
	bool load(string filename, bool buildMipmaps=true, bool makeFloat = false,  GLint wrapMode=GL_REPEAT, GLint magFilter=GL_LINEAR, GLint minFilter=GL_LINEAR_MIPMAP_LINEAR );
	void load(string filename, GLint unitNumber=0, bool buildMipmaps=false, GLint wrapMode=GL_REPEAT, GLint filterMode=GL_LINEAR, bool makeFloat=false);
	void save(string filename);
	unsigned char *  snapshot();

	void setParameterI(GLenum paramName, GLint paramValue );
	void setParameterF(GLenum paramName, GLfloat paramValue );

	void bind(GLenum texUnit);
	void unbind();
	void activate();
	void deactivate();

	void generateMipmaps();

	void show(GLint x, GLint y, GLsizei width, GLsizei height, bool depthtest = false);

	GLint		format;
	GLuint		type;
	int			samples;
	GLint		dataFormat;
	GLenum		textureUnit;
	GLint		textureUnitNumber;
	GLuint		id;
	GLsizei		width;
	GLsizei		height;
	string		inShaderName;
	string		description;
	static GLint texUnitToNumber(GLenum texUnit){
		return texUnit - GL_TEXTURE0;
	}
};

#endif
