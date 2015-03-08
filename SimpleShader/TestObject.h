#ifndef _testobj
#define _testobj
#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "../_utils/utils.h"

extern bool		g_bShadersSupported;
extern bool		g_bUseShaders;

class TestOBJ{
public:
	TestOBJ(int shader);
	~TestOBJ();
	void draw();
	void init();
	GLuint shaderProgramID;
private:
	float * data;
	GLuint textureID;
	

};

#endif