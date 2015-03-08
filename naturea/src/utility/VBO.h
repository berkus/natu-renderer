#pragma once

#include "settings.h"
#include "utility/Shader.h"


class VBODataSet
{
public:
	VBODataSet(const void *_data, int _dataUnitSize, int _type, string _inShaderName, bool _positionFlag);
	~VBODataSet();
	const void *		data;
	int			offset;
	int			size;
	int			dataUnitSize;
	int			componentCount;
	int			type;
	string		inShaderName;
	bool		positionFlag;
};


class VBO
{
public:
	VBO(void);
	~VBO(void);
	
	void	bind(Shader * shader);
	void	unbind(Shader * shader);

	void	draw(Shader *shader, GLuint primitiveType=GL_TRIANGLES, int first=0);

	bool	load();
	
	bool	save();

	void	cleanUp();

	int		addVertexAttribute(VBODataSet * dataset);

	int		getReqiuredSpace();

	void	compileData(GLenum usage);
	void	compileWithShader(Shader* shader);
	
	void	setVertexCount(int count);
	int		getVertexCount();
	
	int getID(){
		return id;
	}
private:

	GLuint					id;
	int						vertexCount;
	vector<VBODataSet*>		datasets;
	vector<int>				offsets;
	vector<int>				sizes;

};

