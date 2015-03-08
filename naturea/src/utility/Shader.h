#ifndef _SHADER_H
#define _SHADER_H

#include "settings.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "IO\ioutils.h"
#include "utility\Texture.h"
#include "utility\utils.h"

struct Uniformi{
	GLint location;
	GLint value;
};
enum	UniformType{
		F1,
		F2,
		F3,
		F4,
		B1,
		I1,
		I2,
		I3,
		I4,
		M4,
		M3
	};
struct Uniform{
	void *	data;
	GLint	location;		
	UniformType type;
	string name;
};

class Shader
{
public:
	Shader(string name);
	~Shader(void);

	GLuint programID;
	bool	loadShader(	string vs_filename, string fs_filename);
	bool	loadShader(
						string vs_filename,
						string fs_filename,
						string gs_filename,
						GLint  geometry_vertices_out,
						GLenum geometry_primitive_in,
						GLenum geometry_primitive_out
					   );
	bool	linkTexture(Texture *texture);
	void	setTime(float in_time);
	void	destroy();
	void	use(bool isTurnedOn);
	int		getLocation(string var_name);
	int		getGLLocation(string var_name);
	int		getAttributeLocation(string var_name);

	int		registerUniform(string inShaderName, UniformType type, void * data);
	void	setUniform(Uniform * uni);
	
	void	setUniform1i(int locIndex, int value);
	void    setUniform2i(int locIndex, int value1, int value2);
	void	setUniform3i(int locIndex, int value1, int value2, int value3);
	void    setUniform4i(int locIndex, int value1, int value2, int value3, int value4);
	void	setUniform1f(int locIndex, float value);
	void    setUniform2f(int locIndex, float value1, float value2);
	void	setUniform3f(int locIndex, float value1, float value2, float value3);
	void    setUniform4f(int locIndex, float value1, float value2, float value3, float value4);
	void	setUniformfp(int locIndex, float* values, int count);
	void    setUniform2v(int locIndex, v2 value);
	void	setUniform3v(int locIndex, v3 value);
	void    setUniform4v(int locIndex, v4 value);
	void	setUniform1b(int locIndex, bool value);
	void	setUniformMatrix(int locIndex, m4 & matrix);
	void	setBoolean(int locIndex, bool value);
	void	setTexture(int location, int texUnitNumber);

	Uniform * getUniform(int index);

	GLint	timeLocation;
	vector<GLint>	 locations;
	vector<GLint>	 attributeLocations;
	vector<string>	 attributeNames;
private:	
	bool	isRunning;
	void   printShaderInfoLog(GLuint objectId, bool bShaderProgram);

	GLuint vertexShID;
	GLuint geometryShID;
	GLuint fragmentShID;

	vector<Uniform*> uniforms;
	string name;
	

};

#endif
