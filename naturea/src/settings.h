#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <cstdio>
#include <cstring>
#include <vector>
#include <stack>
#include <queue>
#include <map>
#include "GLEW/glew.h"
#include "GL/glut.h"
#include "akMath.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "..\common\coordSystem.h"
#include "globals.h"

#define BUFFER_OFFSET(i) ((char*) NULL + (i))
#define BUFFER_OFFSET2(i) ((const GLvoid *) i)

#define TIME_STEP 0.1f
#define TIME_QUERY_EXTENSION	"GL_EXT_timer_query"
#define PRIMITIVES_QUERY_EXTENSION ""

#define HEIGHTMAP_SOURCE		"textures/terrain/terrain2.png"
#define HEIGHTMAP_SCALE			0.001
#define HEIGHTMAP_INITHEIGHT	5

#define TERRAIN_TEX_COUNT		5
#define TERRAIN_SIZE_X			500.f
#define TERRAIN_SIZE_Y			500.f
#define TERRAIN_RESOLUTION_X	100
#define TERRAIN_RESOLUTION_Y	100
#define TERRAIN_TEX_NAME		"textures/terrain/terrain_tex_%02i.png"
#define TERRAIN_VS_FILENAME		"shaders/terrain/terrain_vs.glsl"
#define TERRAIN_FS_FILENAME		"shaders/terrain/terrain_fs.glsl"
#define TERRAIN_UW_FS_FILENAME  "shaders/terrain/terrain_uw_fs.glsl"
#define TERRAIN_UW_VS_FILENAME  "shaders/terrain/terrain_uw_vs.glsl"
#define HORIZON_COLOR			v4(0.7f, 0.0f, 0.0f, 1.0f)




#define GRASS_COUNT				500
//#define GRASS_TEX_NAME			"textures/grass/grass_01.png"
#define GRASS_TEX_NAME			"textures/grass/grass_multitextureVert.png"
#define GRASS_WAVE_TEX_NAME		"textures/grass/dudv03.png"
#define GRASS_VS_FILENAME		"shaders/grass/grass_vs.glsl"
#define GRASS_FS_FILENAME		"shaders/grass/grass_fs.glsl"
#define GRASS_MIN_HEIGHT		6.f
#define GRASS_MAX_HEIGHT		12.f
#define GRASS_MIN_DIST			0.005f
#define GRASS_SCALE				2.f


#define TREE1_COUNT				50
#define TREE1_TEX_NAME			"textures/tree1/tree_windbreak.png"
#define TREE1_VS_FILENAME		"shaders/tree/tree_vs.glsl"
#define TREE1_FS_FILENAME		"shaders/tree/tree_fs.glsl"
#define TREE1_MIN_HEIGHT		7.f
#define TREE1_MAX_HEIGHT		13.f
#define TREE1_MIN_DIST			5.f
#define TREE1_SCALE				5.f


#define TREE2_COUNT				50
#define TREE2_TEX_NAME			"textures/tree2/pine.png"
#define TREE2_VS_FILENAME		"shaders/tree/tree_vs.glsl"
#define TREE2_FS_FILENAME		"shaders/tree/tree_fs.glsl"
#define TREE2_MIN_HEIGHT		12.f
#define TREE2_MAX_HEIGHT		20.f
#define TREE2_MIN_DIST			5.f
#define TREE2_SCALE				6.f



#define SKYBOX_TEX_FILENAMES	"textures/skybox/512/calm_%s.png"
#define SKYBOX_SIZE				800

#define WATER_DEPTH_COLOR		v4(0.1, 0.3, 0.5, 1.0)
#define WATER_VS_FILENAME		"shaders/water/water_vs.glsl"
#define WATER_FS_FILENAME		"shaders/water/water_fs.glsl"
#define WATER_HEIGHT			0.f
//#define WATER_DUDV_MAP			"textures/water/water_dudv.png"
#define WATER_NORMAL_MAP		"textures/water/water_normal.png"
#define WATER_DUDV_MAP			"textures/water/dudv03.png"
//#define WATER_DUDV_MAP			"textures/water/water_dudv.png"

#define HUMAN_SPEED				0.3f
#define HUMAN_HEIGHT			1.8f
#define HUMAN_BREATH_FREQ		0.6f
#define HUMAN_BREATH_AMPL		0.5f
#define HUMAN_ACTIVITY_DECAY    0.995f
#define HUMAN_ACTIVITY_INCR	    0.5f
#define HUMAN_MIN_ACTIVITY	    0.5f
#define HUMAN_MAX_ACTIVITY	    1.3f
/*
#define HUMAN_POSITION          v3(-5.0, 10, 10)
#define HUMAN_DIRECTION			v3(0.5, 0.0, -0.60)
/*/
#define HUMAN_POSITION          v3(-27.37, 15.17, 20.59)
#define HUMAN_DIRECTION			v3(0.68, -0.38, -0.63)


#define PHONG_VS_FILENAME		"shaders/models/phong_vs.glsl"
#define PHONG_FS_FILENAME		"shaders/models/phong_fs.glsl"
#define PARALLAX_VS_FILENAME	"shaders/models/parallax_vs.glsl"
#define PARALLAX_FS_FILENAME	"shaders/models/parallax_fs.glsl"

#define PHONG_S_VS_FILENAME		"shaders/models/phong_vs_s.glsl"
#define PHONG_S_FS_FILENAME		"shaders/models/phong_fs_s.glsl"


static enum Attribs{
	INDEX,
	VERTEX,
	NORMAL,
	TANGENT,
	BINORMAL,
	TEXCOORD0,
	TEXCOORD1,
	TEXCOORD2,
	TEXCOORD3,
	TEXCOORD4,
	TEXCOORD5,
	TEXCOORD6,
	TEXCOORD7,
	ATTRIB0,
	ATTRIB1,
	ATTRIB2,
	ATTRIB3,
	ATTRIB4,
	FACENORMAL,
	// add new items here (must be before VBO_ATTRIB_CNT!!!)
	

	VBO_ATR_COUNT
};

#define LOD_REDUCTION			1.0


#define GODRAYS_VS_FILENAME		"shaders/godrays/godrays_vs.glsl"
#define GODRAYS_FS_FILENAME		"shaders/godrays/godrays_fs.glsl"

#define LIGHT_POSITION			v4(0.0f, 20.0f, 0.0f, 0.0f) //v4(0.f, 0.f, 1.f, 0.0f) // specific for sahara skybox... // direction
//#define LIGHT_POSITION			v3(10.f, 300.f, 0.f) 
#define LIGHT_DIRECTION			v4(-0.91f, -0.41f, 0.06f, 0.0)
#define SHADOWMAP_RESOLUTION_X  2048
#define SHADOWMAP_RESOLUTION_Y  2048


static v4 sunAmb  = v4(0.5,0.5, 0.5, 1.0);
static v4 sunDif  = v4(1.0,1.0, 1.0, 1.0);
static v4 sunSpe  = v4(0.5,0.5, 0.6, 1.0);

static GLfloat material_amd[4] = {0.6f, 0.6f, 0.6f, 1.0f};
static GLfloat material_spe[4] = {.5f, .5f, .5f, 1.0f};

// ===============================================================================
// TREE
// ===============================================================================
#define M_PI 3.14159265f
namespace DYN_TREE
{
	const static char * SHADER_BRANCH_V			= "shaders/dynamic_tree/branch_vs.glsl";
	const static char * SHADER_BRANCH_F			= "shaders/dynamic_tree/branch_fs.glsl";
	const static char * SHADER_BRANCH_SH_V		= "shaders/dynamic_tree/shadow/branch_shadow_vs.glsl";
	const static char * SHADER_BRANCH_SH_F		= "shaders/dynamic_tree/shadow/branch_shadow_fs.glsl";
	
	
	const static char * SHADER_BRANCH_VN		= "shaders/dynamic_tree/n_branch_vs.glsl";
	const static char * SHADER_BRANCH_FN		= "shaders/dynamic_tree/n_branch_fs.glsl";
	const static char * SHADER_BRANCH_GN		= "shaders/dynamic_tree/n_branch_gs.glsl";
	const static char * SHADER_LEAF_V			= "shaders/dynamic_tree/leaf_vs.glsl";
	const static char * SHADER_LEAF_F			= "shaders/dynamic_tree/leaf_fs.glsl";
	const static char * SHADER_LEAF_SH_V		= "shaders/dynamic_tree/shadow/leaf_shadow_vs.glsl";
	const static char * SHADER_LEAF_SH_F		= "shaders/dynamic_tree/shadow/leaf_shadow_fs.glsl";
	
	
	const static char * SHADER_LEAF_VN			= "shaders/dynamic_tree/n_leaf_vs.glsl";
	const static char * SHADER_LEAF_FN			= "shaders/dynamic_tree/n_leaf_fs.glsl";
	const static char * SHADER_LEAF_GN			= "shaders/dynamic_tree/n_leaf_gs.glsl";
	
	
	// prepare LOD
	const static char * SHADER_BRANCH_LOD_V		= "shaders/dynamic_tree/lod_branch_vs.glsl";
	const static char * SHADER_BRANCH_LOD_F		= "shaders/dynamic_tree/lod_branch_fs.glsl";
	const static char * SHADER_LEAF_LOD_V		= "shaders/dynamic_tree/lod_leaf_vs.glsl";
	const static char * SHADER_LEAF_LOD_F		= "shaders/dynamic_tree/lod_leaf_fs.glsl";

	const static char * SHADER_PREPROCESS_V		= "shaders/dynamic_tree/preprocess_vs.glsl";
	const static char * SHADER_PREPROCESS_F		= "shaders/dynamic_tree/preprocess_fs.glsl";

	const static char * SHADER_JOIN_V			= "shaders/dynamic_tree/join_vs.glsl";
	const static char * SHADER_JOIN_F			= "shaders/dynamic_tree/join_fs.glsl";

	// LOD
	const static char * SHADER_LOD_V		= "shaders/dynamic_tree/lod_vs.glsl";
	const static char * SHADER_LOD_F		= "shaders/dynamic_tree/lod_fs.glsl";
	const static char * SHADER_SHADOW_LOD_V	= "shaders/dynamic_tree/lod_shadow_vs.glsl";
	const static char * SHADER_SHADOW_LOD_F	= "shaders/dynamic_tree/lod_shadow_fs.glsl";



	const static int	MAX_HIERARCHY_DEPTH		= 4;
	const static float	LEAF_SIZE				= 0.04;
	//const static char	*TEX_LEAF1				= "textures/dynamic_tree/leaf2r.png";
	const static char	*TEX_WOOD1				= "textures/dynamic_tree/bark2_decal.png";
	const static char	*BRANCH_NOISE_TEXTURE	= "textures/dynamic_tree/Noise_20.png";
	const static char	*LEAF_NOISE_TEXTURE		= "textures/dynamic_tree/Noise_53.png";

	//const static char	*TEX_FDM				= "textures/dynamic_tree/leaf2.png";
	//const static char	*TEX_FNM				= "textures/dynamic_tree/leaf2normal.png";
	const static char	*TEX_FDM				=  "textures/dynamic_tree/leaf3_decal_front.png";			//"textures/dynamic_tree/habel/leaf3_decal_front.png";
	const static char	*TEX_FNM				=  "textures/dynamic_tree/leaf3_normal_front.png";		//"textures/dynamic_tree/habel/leaf3_normal_front.png";
	const static char	*TEX_FTM				=  "textures/dynamic_tree/leaf3_translucency_front.png";	//"textures/dynamic_tree/habel/leaf3_translucency_front.png";
	const static char	*TEX_FHM				=  "textures/dynamic_tree/leaf3_halflife2_front.png";		//"textures/dynamic_tree/habel/leaf3_halflife2_front.png";
	const static char	*TEX_BDM				=  "textures/dynamic_tree/leaf3_decal_back.png";			//"textures/dynamic_tree/habel/leaf3_decal_back.png";
	const static char	*TEX_BNM				=  "textures/dynamic_tree/leaf3_normal_back.png";			//"textures/dynamic_tree/habel/leaf3_normal_back.png";
	const static char	*TEX_BTM				=  "textures/dynamic_tree/leaf3_translucency_back.png";	//"textures/dynamic_tree/habel/leaf3_translucency_back.png";
	const static char	*TEX_BHM				=  "textures/dynamic_tree/leaf3_halflife2_back.png";		//"textures/dynamic_tree/habel/leaf3_halflife2_back.png";

	const static int	SLICE_RESOLUTION_X		= 1024;
	const static int	SLICE_RESOLUTION_Y		= 1024;

	const static char	*SEASON_MAP				= "textures/dynamic_tree/seasonMap2.png";

	const static char*  DATA_TEXTURE_NAME		= "data_tex";
	const static char*	LOD_DATA_TEXTURE_NAME	= "lod_data_tex";
	const static string OBJT_FILENAME			= "models/OBJT/JavorListy.objt";
	enum VERTEX_ATTRIBUTES{
		POSITION,
		NORMAL,
		TANGENT,
		TEXCOORD0,
		BRANCH_INDEX,
		WEIGHT,
		COUNT
	};
	
	const static float	MIN_HEIGHT		= -10.f;
	const static float	MAX_HEIGHT		= 10.f;

}
namespace ATTRIB_NAMES
{
	const static string POSITION	= "position";
	const static string NORMAL		= "normal";
	const static string TANGENT		= "tangent";
	const static string BITANGENT	= "bitangent";
	const static string BINORMAL	= "binormal";
	const static string TEXCOORD0	= "texCoords0";
	const static string TEXCOORD1	= "texCoords1";
	const static string TEXCOORD2	= "texCoords2";
	const static string BRANCH_ID	= "branch_index";
	const static string WEIGHT		= "x_vals";
}



using namespace std;

#endif