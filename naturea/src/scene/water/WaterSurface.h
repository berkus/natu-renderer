#ifndef _WATERSURFACE_H
#define _WATERSURFACE_H

#include "scene\scenemodel.h"
#include "utility\utils.h"
#include "scene\cameras\Camera.h"

class WaterSurface :
	public SceneModel
{
public:
	WaterSurface(TextureManager *texManager, ShaderManager *shManager);
	~WaterSurface(void);

	void draw();

	virtual void drawForLOD();

	void init();

	void update(double time);

	void windowSizeChanged(int width, int height);
	void beginReflection();
	void endReflection();
	void beginRefraction();
	void endRefraction();


	void showTextures();
	GLuint	fb_refl_ID;
	GLuint	cb_refl_ID;
	GLuint	db_refl_ID;
	GLuint	fb_refr_ID;
	GLuint	cb_refr_ID;
	GLuint	db_refr_ID;
	int		dudv_ID;
	Shader* shader;
	int		water_reflection_loc;
	int		water_refraction_loc;
	int		water_depth_loc;
	int		water_dudv_loc;
	v4		viewPos;
	v4		lightPos;
	v4		waterColor;
	v4		waterDepth;
	Camera *activeCamera;
};

#endif

