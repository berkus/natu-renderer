#ifndef _SHADERMANAGER_H
#define _SHADERMANAGER_H

#include "settings.h"
#include "Shader.h"
#include "utils.h"

class ShaderManager
{
public:
	ShaderManager(void);
	~ShaderManager(void);

	int loadShader( string shname, string vs_filename, string fs_filename);
	int loadShader(	string shname,
					string vs_filename,
					string fs_filename,
					string gs_filename,
					GLint  geometry_vertices_out,
					GLenum geometry_primitive_in,
					GLenum geometry_primitive_out);
	void init();
	void use(int shaderId, bool turnOn);
	Shader* getShader(int shaderId);

	Shader* getBumpMapShader();
	Shader* getPhongShader();
	Shader* getParallaxShader();
	
private:

	Shader* bumpmapShader;
	Shader* phongShader;
	Shader* parallaxShader;
	vector<Shader*> shaders;
};

#endif
