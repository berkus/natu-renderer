#pragma once

#include <vector>
#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "../_utils/utils.h"

class Shader
{
public:
	Shader(void);
	~Shader(void);

	GLuint programID;
	GLuint vertexShID;
	GLuint geometryShID;
	GLuint fragmentShID;

	void printShaderInfoLog(GLuint objectId, bool bShaderProgram);

	void destroyShaderProgram();
	bool createShaderProgram( 
						 const char* pVertexShaderFileName, 
						 const char* pFragmentShaderFileName);

	bool createShaderProgram(
						 const char* pVertexShaderFileName, 
						 const char* pGeometryShaderFileName, 
						 const char* pFragmentShaderFileName,
						 GLint	geometry_vertices_out,
						 GLenum geometry_primitive_in,
						 GLenum geometry_primitive_out);
};

