#include "ShaderManager.h"


ShaderManager::ShaderManager(void)
{
}

ShaderManager::~ShaderManager(void)
{
	int i;
	for (i=0; i<shaders.size(); i++){
		shaders[i]->destroyShaderProgram();
	}
	shaders.clear();
}

int ShaderManager::addShader(Shader * _sh){
	shaders.push_back(_sh);
	return shaders.size()-1;
}
