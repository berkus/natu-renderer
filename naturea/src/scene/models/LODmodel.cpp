#include "LODmodel.h"


LODmodel::LODmodel(void)
{
	for (int i=0; i<4; i++){
		lods[i]=NULL;
	}
	LODvalue = 0;
	lastLOD	 = 0;
}


LODmodel::~LODmodel(void)
{
	glDeleteQueries(1, &queryID);
	for (int i=0; i<4; i++){
		if (lods[i]!=NULL){
			delete lods[i];
			lods[i]=NULL;
		}
	}

}

void LODmodel::setLODModel(int level, SceneModel* model)
{
	if (level>=0 && level<4){
		lods[level] = model;
		if (model!=NULL && level> lastLOD){
			lastLOD = level;
		}
	}
}

void LODmodel::drawForReflection(){
	if (lods[*actualLOD]!=NULL){
		drawLOD(lastLOD);
	}
}

void LODmodel::getLODvalue()
{
	// ==================================
	// occlusion query alg
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glMultMatrixf( transformMatrix.m );
		// draw bbox and query it...
		glBeginQuery(GL_SAMPLES_PASSED, queryID);
			lods[0]->drawForLOD();
		glEndQuery(GL_SAMPLES_PASSED);
	glPopMatrix(); 
	// decide which LOD depending on query result...
	GLuint	result = 0;
	glGetQueryObjectuiv(queryID, GL_QUERY_RESULT, &result);
	*LODvalue = result;

	/*
	// ===================================
	// distance alg
	
	// get distance to camera

	// decide which LOD depending distance...
 
	
	*/
	

}

void LODmodel::setLOD()
{
	if (*LODvalue> tresholds[0]){
		*actualLOD = 0;
	} else if (*LODvalue > tresholds[1]){
		*actualLOD = 1;
	} else if (*LODvalue > tresholds[2]){
		*actualLOD = 2;
	} else {
		*actualLOD = 3;
	}
}

void LODmodel::drawLOD(int level)
{
	if (lods[*actualLOD]!=NULL){	
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMultMatrixf( transformMatrix.m );
			lods[*actualLOD]->draw();
		glPopMatrix();
	}
}

void LODmodel::drawForShadowMapping(){
	if (lods[*actualLOD]!=NULL){
		drawLOD(lastLOD);
	}
}
void LODmodel::drawForLOD(){
	getLODvalue();
}

void LODmodel::draw()
{
	setLOD();
	drawLOD(*actualLOD);	
}

void LODmodel::init()
{
	glGenQueries(1, &queryID);
}

void LODmodel::update(double time)
{

}