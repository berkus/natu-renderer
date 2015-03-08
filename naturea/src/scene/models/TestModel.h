#pragma once
#include "scene\scenemodel.h"
#include "models\elephant.h"
#include "../../utility/VBO.h"
#include "../../scene/vegetation/dynamic/DTree.h"
#include "../../scene/cameras/Camera.h"


class TestInstance{
public:
	TestInstance(){}
	~TestInstance(){}

	m4	transformMatrix;
	float param1;
	//float x,y,z,r;
	v3		position;
	float	rotation_y;
	float	distance;
	float	discrepacy;
	v3		eye_dir;	
};



class TestModel :
	public SceneModel
{
public:
	TestModel(void);
	~TestModel(void);

	void		 prepareForRender();
	void		 render();

	void enqueueInRenderList(TestInstance * instance);

	virtual void draw();

	virtual void drawForLOD();

	virtual void init();

	virtual void update(double time);

	GLuint				i_matricesBuffID;
	GLuint				i_paramBuffID;
	GLuint				v_positionsBuffID;
	GLuint				v_indicesBuffID;
	float				*matricesBufferData;
	vector<TestInstance*> instances;
	Shader				*shader;
	VBO					*vbo;
	EBO					*ebo;
	Terrain				*terrain;
	Camera				*camera;


	float *				type1Matrices  ;
	float *				type2Matrices  ;
	float *				type1Param1    ;
	float *				type2Param1    ;

	int type1Index ;
	int type2Index ;

	int p1Loc 	   ;
	int tmLoc 	   ;
	int tmLoc0	   ;
	int tmLoc1	   ;
	int tmLoc2	   ;
	int tmLoc3	   ;
};

