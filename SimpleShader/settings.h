#ifndef _SETTINGS_H
#define _SETTINGS_H

#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "../_utils/Vector3.h"
#include "../_utils/Vector4.h"

const static GLenum COL1_TEX_UNIT			= GL_TEXTURE0;
const static GLint COL1_TEX_UNIT_ORDER		= 0;
const static GLenum COL2_TEX_UNIT			= GL_TEXTURE1;
const static GLint COL2_TEX_UNIT_ORDER		= 1;
const static GLenum DATA_TEX_UNIT			= 0x84C2;
const static GLint DATA_TEX_UNIT_ORDER		= 2;
const static int MAX_HIERARCHY_DEPTH		= 4;
const static float LEAF_SIZE				= 0.5;

inline float randomf(float minf, float maxf){ 
	//srand(unsigned int(clock()));
	int lastRandom = rand();
	return float((double(lastRandom)/RAND_MAX)*(maxf-minf)) + minf; 
}
const static char * TEX_LEAF1 = "textures/leaf2r.png";
const static char * TEX_WOOD1 = "textures/bark2_decal.png";
#define BUFFER_OFFSET(i) ((char*) NULL + (i))

extern v3			g_WindDirection;
extern float		g_WindStrength ;
extern v4			g_WoodAmplitude;
extern v4			g_WoodFrequency;
extern float		g_LeafAmplitude;
extern float		g_LeafFrequency;
#endif