#include "SceneModel.h"

SceneModel::SceneModel(){
	textureManager = NULL;
	transformMatrix.setIdentity();
	bbox = NULL;
}
SceneModel::SceneModel(TextureManager *texManager, ShaderManager *shManager){
	textureManager = texManager;
	shaderManager  = shManager;
	transformMatrix.setIdentity();
	bbox = NULL;
}



SceneModel::~SceneModel()
{

}

void SceneModel::translate(v3 &movVector)
{
	transformMatrix.translate(movVector);
}

void SceneModel::rotate(v3 &axis, float angleRad)
{
	transformMatrix.rotate( axis, angleRad);
}

void SceneModel::scale(v3 &scaleVector)
{
	transformMatrix.scale(scaleVector);
}