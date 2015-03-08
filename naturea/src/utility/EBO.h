#pragma once

#include "VBO.h"


class EBO
{
public:
	EBO(void);
	~EBO(void);

	bool save(string filename);
	bool load(string filename);


	void linkVBO(VBO * _vbo);

	void create(GLenum _dataType, GLenum _primitiveType, int _indicesCount, const void * _data, GLenum _usage);
	void bind();
	void unbind();
	void draw(GLenum _dataType, GLenum _primitiveType, int _indicesCount, const void * offset);
	void drawInstanced(GLenum _primitiveType, int _indicesCount, GLenum _dataType, const void * _offset, int _instanceCount);
	void draw(Shader * shader);
	int getID(){
		return id;
	}
private:
	GLuint	id;
	GLuint	dataType;
	GLuint	primitiveType;
	int		indicesCount;
	const void *	data;
	GLuint	usage;
	VBO *	vbo;
};

