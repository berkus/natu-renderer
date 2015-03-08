#pragma once

#include <vector>
#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "Shader.h"

using namespace std;

class ShaderManager
{
public:
	ShaderManager(void);
	~ShaderManager(void);
	int addShader(Shader * _sh);

	vector<Shader*> shaders;

};

