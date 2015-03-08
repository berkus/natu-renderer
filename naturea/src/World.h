#ifndef _WORLD_H
#define _WORLD_H

#include "globals.h"
#include "utility\utils.h"
#include "../common/models/cube.h"
#include "utility\TextureManager.h"
#include "scene\scenemodel.h"
#include "scene\models\TestModel.h"
#include "scene\BBox.h"
#include "scene\terrain\Terrain.h"
#include "scene\cameras\Camera.h"

#include "utility\Sound.h"

#include "scene\Atmosphere\SkyBox.h"
#include "scene\Atmosphere\Light.h"
#include "scene\Atmosphere\GodRays.h"
#include "scene\Atmosphere\Fog.h"
#include "scene\Water\WaterSurface.h"


#include "scene\Vegetation\static\Grass.h"
#include "scene\Vegetation\static\Tree1.h"
#include "scene\Vegetation\static\Tree2.h"
#include "scene\Vegetation\Planter.h"
#include "scene\Vegetation\dynamic\DTree.h"
#include "scene\Vegetation\dynamic\DTreePlanter.h"

class World
{
public:
	World(void);
	~World(void);
	void draw();
	void drawUnderWater();
	void drawReflection();
	void drawForLOD();
	void windowSizeChanged(int width, int height);

	void startLODBuffer();
	void endLODBuffer();

	void init();

	void initModels();
	void drawModels();

	void deinitModels();

	void update(double time);

	void translate(v3 &movVector);

	void rotate(v3 &axis, float angleRad);

	void scale(v3 &scaleVector);

	void snapTree(v3 &dir){
		//if (p_dtree!=NULL){
		//	p_activeCamera->shoot();
		//	p_dtree->createSlices(dir, g_slice_count);
		//}
		system("PAUSE");
	}
// attributes:
	vector<SceneModel*> v_models;
	SkyBox				*p_skybox;
	Terrain				*p_terrain;
	BBox				*box;
	Camera				*p_activeCamera;
	Light				*p_activeLight;
	GodRays				*p_godRays;
	Fog					*p_fog;
	WaterSurface		*p_waterSurface;


	Grass				*p_grass_prototype;
	Vegetation			*p_grass_growth;
	Tree1				*p_tree1_prototype;
	Vegetation			*p_tree1_growth;
	Tree2				*p_tree2_prototype;
	Vegetation			*p_tree2_growth;
	DTree				*p_dtree;
	Sound				*p_backgroundSound;

	TestModel			*p_test_model;
	TestModel			*p_test_model2;

	TextureManager		textureManager;
	ShaderManager		shaderManager;

	// planters
	Planter				grass_planter;
	Planter				tree1_planter;
	Planter				tree2_planter;
	DTreePlanter		dtree_planter;
private:
	void				drawWithAlpha();
	GLuint				fb_LOD_ID;
	GLuint				cb_LOD_ID;
	GLuint				db_LOD_ID;

	VBO	* cubeVBO;
	EBO * cubeEBO;
	Shader * cubeShader;

	v4**				tree_positions;

	void show_textures();
	void drawForShadowmapping();
};

#endif
