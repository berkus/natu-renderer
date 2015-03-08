#ifndef __SCENEMODEL__
#define __SCENEMODEL__


#include "settings.h"
#include "models\cube.h"
#include "utility\utils.h"
#include "utility\TextureManager.h"
#include "utility\ShaderManager.h"
#include "Matrix4x4.h"
#include "scene\BBox.h"
#include "scene\Material.h"

class SceneModel
{
public:
	SceneModel();
	SceneModel(TextureManager *texManager, ShaderManager *shManager);
	~SceneModel(void);

	virtual void draw()=0;

	virtual void drawForLOD()=0;

	virtual void init()=0;

	virtual void update(double time)=0;

	void translate(v3 &movVector);

	void rotate(v3 &axis, float angleRad);

	void scale(v3 &scaleVector);


	TextureManager *textureManager;
	ShaderManager  *shaderManager;


	Material * material;
	m4	transformMatrix;
	int fastModeLoc;
	int	shadowMappingEnabledLoc;
	int LCmatrixLoc;
	int LMV_CVImatrixLoc;
	int LPmatrixLoc;
	BBox * bbox;
};
#endif

