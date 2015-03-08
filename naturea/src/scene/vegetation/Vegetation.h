#ifndef _VEGETATION_H
#define _VEGETATION_H

#include "scene\scenemodel.h"
#include "scene\terrain\terrain.h"

const GLfloat CROSS_VERTEX_ARRAY[] =
{
	 -0.5f,0.0f,0.0f, -0.5f,1.0f,0.0f, 0.5f,1.0f,0.0f, 0.5f,0.0f,0.0f,
	 0.0f,0.0f,-0.5f,  0.0f,1.0f,-0.5, 0.0f,1.0f,0.5f, 0.0f,0.0f,0.5f,
	 0.0, 1.0, 0.0,    0.0, 1.0, 0.0,   0.0, 1.0, 0.0,  0.0, 1.0, 0.0,
	 0.0, 1.0, 0.0,    0.0, 1.0, 0.0,   0.0, 1.0, 0.0,  0.0, 1.0, 0.0,
	 0.0, 0.0,         0.0, 1.0,	   1.0, 1.0, 	   1.0, 0.0,
	 0.0, 0.0,         0.0, 1.0,	   1.0, 1.0, 	   1.0, 0.0
};
const GLint cross_vertexCount	 = 8;
const GLint cross_NormalOffset   = cross_vertexCount*3*sizeof(GLfloat);
const GLint cross_TexCoordOffset = cross_vertexCount*3*2*sizeof(GLfloat);

struct VertexInfo
{
	v4 position;
	v4 normal;
	v3 texCoord;
	static const int sizePosition	= 3;
	static const int sizeNormal		= 3;
	static const int sizeTexCoord	= 2;
};

struct Offsets
{
	int position;
	int normal;
	int texCoord;
};


class Vegetation :
	public SceneModel
{
public:
	Vegetation(TextureManager *texManager, ShaderManager *shManager);
	virtual ~Vegetation(void);
	
	virtual Vegetation* getCopy()=0;

	virtual void draw()=0;

	virtual void drawForLOD();

	virtual void init()=0;

	virtual void update(double time)=0;

	virtual void bakeToVBO()=0;

	virtual void fixTexType()=0;
	virtual v3	 transformTexCoords(v3 &origTexCoords)=0;

	vector<VertexInfo>	vertices;
	v3					position;
	v3					scaleVector;
	float				rotationY;

	float				*pVBOdata;
	Offsets				offsets;
	int					VBOdataCount;
	int					VBOdataSize;
	int					*pEBOdata;
	int					EBOdataCount;

	GLuint				vboId;
};

#endif

