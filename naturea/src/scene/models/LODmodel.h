#ifndef _LODMODEL_H
#define _LODMODEL_H

#include "scene\scenemodel.h"


class LODmodel :
	public SceneModel
{
public:
	LODmodel(void);
	~LODmodel(void);

	void getLODvalue();

	void setLODModel(int level, SceneModel* model);

	virtual void draw();

	virtual void drawForLOD();

	void drawLOD(int level);

	void drawForShadowMapping();

	void drawForReflection();

	virtual void init();

	virtual void update(double time);

	v3			tresholds;

	int*		actualLOD;
	int*		LODvalue;
private:
	GLuint		queryID;
	SceneModel* lods[4];
	void		setLOD();
	int			lastLOD;

};

#endif
