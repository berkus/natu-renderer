//-----------------------------------------------------------------------------
//  NATUR(E)AL
//  28/09/2011
//-----------------------------------------------------------------------------
//  Controls: 
//    [mouse]		 ... look direction / select&adjust controls
//    [w]            ... move forward
//    [s]            ... move backward
//    [a]            ... move left
//    [d]            ... move right
//    [q]            ... move up
//    [e]            ... move down
//    [x], [X]       ... decrease/increase the speed of movement
//    [spacebar]     ... switch between look-by-mouse / select&control-by-mouse 
//  
//-----------------------------------------------------------------------------
#define USE_ANTTWEAKBAR
#define TEST 0
#include "../common/Vector4.h"
#include "settings.h"

int		g_shadowMapSamples = 2;
v3		g_ShadowMapSize(SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y, 0.0);
int		g_samples = 4;
bool	g_compressToOneTexture = true;

bool	g_ParallaxMappingEnabled = true;
float	g_ParallaxScale =   0.04;
float	g_ParallaxBias  =  -0.02;
int		g_offset = 0;
float	g_cosA	 = 0.0;
float	g_cosB	 = 0.0;
float	g_cosC	 = 0.0;

#define TERRAIN_INIT_BORDER_VAL v4(13.0f, 10.0f, 5.0f, -1.0f)
#define TERRAIN_INIT_BORDER_WID v4(2.0f, 2.0f, 2.0f, 2.0f)

v4	g_terrain_border_values = TERRAIN_INIT_BORDER_VAL;
v4	g_terrain_border_widths = TERRAIN_INIT_BORDER_WID;

float tree2min = TREE2_MIN_HEIGHT;
float tree2max = TREE2_MAX_HEIGHT;
float tree1min = TREE1_MIN_HEIGHT;
float tree1max = TREE1_MAX_HEIGHT;
float grassmin = GRASS_MIN_HEIGHT;
float grassmax = GRASS_MAX_HEIGHT;
float g_dtreemin = DYN_TREE::MIN_HEIGHT;
float g_dtreemax = DYN_TREE::MAX_HEIGHT;


#include <assert.h>
#include "../common/models/cube.h"
#include "World.h"
#include "globals.h"

Sorting g_sorting			= SORT_BACK_TO_FRONT;

CameraMode g_cameraMode		= TERRAIN_RESTRICTED;
int g_WinWidth				= 1280;//800	;   // Window width
int g_WinHeight				= 720; //600;   // Window height
v3  g_window_sizes			= v3(g_WinWidth, g_WinHeight, 0.0);
double g_time				= 0.0;
float g_float_time			= 0.0f;
CTimer						timer;
Statistics					g_Statistics;
int g_Bumpmaps				= 0;
int g_Heightmaps			= 0;
int g_Specularmaps			= 0;
int g_Alphamaps				= 0;

bool tqAvailable			= false;
GLuint tqid					= 0;
bool pqAvailable			= false;
GLuint pqid					= 0;

GLint result_available		= 0;

GLuint samples_query_id1	= 0;
GLuint samples_query_id2	= 0;
double samples_lod12	= 0.0;


float	g_bloomDivide		= 0.8;
float	g_god_expo			= 0.06;
float	g_god_decay			= 1.0;
float	g_god_density		= 0.33;
float	g_god_weight		= 6.0;
float	g_illuminationDecay	= 3.27;

bool g_wind_dirty			= true;
v3	 act_wind_val			= v3(1.0, 0.0, 0.0);


v4 g_light_position			= LIGHT_POSITION;
v4 g_light_direction		= LIGHT_DIRECTION;
bool g_light_showDir		= false;
bool g_godraysEnabled		= false;
bool g_fastMode				= false;
bool g_drawingReflection	= false;
bool g_showTextures			= false;
bool g_ShadowMappingEnabled	= false;
bool g_Draw2Shadowmap		= false;
m4 g_LightMVCameraVInverseMatrix;
m4 g_LightPMatrix;
Light* g_shadowLight;
int		g_multisample_count			= 0;
GLuint	g_screen_multi_framebuffer	= 0;

v3 g_snapshot_direction		= v3(0.0, 0.0, -1.0);
int g_slice_count			= 3;

int	g_GrassCount			= GRASS_COUNT;
int	g_Tree1Count			= TREE1_COUNT;
int	g_Tree2Count			= TREE2_COUNT;
int g_TreeDCount			= 1;
// GLOBAL CONSTANTS____________________________________________________________
const GLfloat VECTOR_RENDER_SCALE = 0.20f;
// GLOBAL VARIABLES____________________________________________________________


// Scene orientation (stored as a quaternion)
GLfloat  g_SceneRot[]           = {0.0f, 0.0f, 0.0f, 1.0f};   
GLfloat  g_SceneTraZ            = 10.0f; // Scene translation along z-axis
GLfloat  g_SceneScale           = 1.0f;

GLuint cube_vbo_id				= 0;
GLuint cube_ebo_id				= 0;
GLuint plane_vbo_id				= 0;
GLuint plane_ebo_id				= 0;

bool     g_ShowVertexNormals    = false; // Show vertex normal/tangent/binormal
bool     g_FaceNormals          = false; // Show face normal
bool     g_Transparency         = false; // Draw transparent meshes
bool     g_WireMode             = false; // Wire mode enabled/disabled
bool     g_FaceCulling          = true; // Face culling enabled/disabled
GLfloat  g_AlphaThreshold       = 0.5f; // Alpha test threshold
bool	 g_MouseModeANT			= true;

bool	g_draw_dtree_lod		= true;
bool	g_draw_lod0				= true;
bool	g_draw_lod1				= true;
bool	g_draw_lod2				= true;
v4		g_lodTresholds			= v4(20, 25, 40, 50);
//v4		g_lodTresholds			= v4(15, 20, 1000, 1000);
bool	g_draw_low_vegetation	= true;
bool	g_draw_dtree			= true;
bool	g_draw_light_direction	= false;



/**************************
* DYNAMIC TREE
*/
float	g_tree_areaFactor		= 0.01;
v3		g_tree_wind_direction	= v3(1.0, 0.0, 0.0);
float	g_tree_wind_strength	= 0.0;
v4		g_tree_wood_amplitudes	= v4(1.0, 0.8, 0.2, 0.1);
v4		g_tree_wood_frequencies	= v4(0.0, 0.0, 0.0, 0.0);//v4(0.4, 1.2, 0.0, 0.0);
float	g_tree_leaf_amplitude	= 2.4;
float	g_tree_leaf_frequency	= 5.0;
int		g_tree_slice_count		= 3;
int		g_tree_show_slice		= 1;
int		g_tree_show_sliceSet	= 0;
float	g_tree_wave_amplitude	= 0.005;
float	g_tree_wave_frequency	= 0.5;
v3		g_tree_movementVectorA	= v3(0.0, 1.0, 0.0);
v3		g_tree_movementVectorB  = v3(1.0, 0.0, 0.0);
float	g_tree_wave_y_offset	= 0.0;
float	g_tree_wave_increase_factor = 1.0;
float	g_tree_time_offset_1	= 0.0;
float	g_tree_time_offset_2	= 0.5;		

const int	g_tree_gridSize			= 1;			// = SQRT(count of the trees)
float		g_tree_mean_distance	= 5.0;			// = how dense is the grid
float		g_tree_dither			= 3.0;			// = how far can be the tree placed from its' position in grid

float		g_dither				= 2.0;

int		g_treesamples = 0;

float	g_leaves_MultiplyAmbient			= 1.0;
float	g_leaves_MultiplyDiffuse			= 0.9;
float	g_leaves_MultiplySpecular			= 0.5;
float	g_leaves_MultiplyTranslucency		= 0.5;
float	g_leaves_ReduceTranslucencyInShadow	= 0.9;
float	g_leaves_shadow_intensity			= 1.0;
v3		g_leaves_LightDiffuseColor			= v3(0.2, 0.2, 0.2);


int		g_tree_lod0_count					 = 0;
int		g_tree_lod1_count					 = 0;
int		g_tree_lod2_count					 = 0;
int		g_tree_lod01_count					 = 0;
int		g_tree_lod12_count					 = 0;


bool	g_draw_lod1_method		= true;
bool	g_orbit					= false;
float	g_orbit_speed			= 0.1;
float	g_orbit_radius			= 10.0;
v3		g_center				= v3(0.0, 5.0, 0.0);
float	g_timeDiff				= 0;


v3*		g_viewer_position;
v3*		g_viewer_direction;

float	g_fog_density = 0.006, g_fog_start = 1.0, g_fog_end=100.0;

v3		g_tintColor = v3(1.0, 1.0, 1.0);
float	g_tintFactor= 1.0;
float	g_varA		= 1.0;

float	g_season = 0.58;

bool	g_debug = false;
float	g_CPU_fps;
float	CPU_render_time;

LODTransitionMethod g_lodTransition = LODTransitionMethod::SHIFTED_SOFT_FADE;
float   g_transitionShift		= 0.35;
float   g_transitionControl		= 0.0;

float	g_gauss_shift			= 0.5;
float	g_gauss_steep			= 0.1;
float	g_gauss_weight			= 0.05;


World* p_world;
Texture *	g_shadowmap1		= NULL;
m4		*	g_LightMVPmatrix	= NULL;
m4		*	g_LightMVPCameraVInverseMatrix = NULL;

float		g_ShadowNear= 0.0;
float		g_ShadowFar = 50.0;
float		g_ShadowFarMNear;
float		g_CameraNear= 0.0;
float		g_CameraFar = 50.0;

#include "../common/common.h"
// Model file name
std::string g_ModelFileName     = "models/DesertOasis/H1F.pgr2";


// FORWARD DECLARATIONS________________________________________________________
void initGUI(void);
void TW_CALL loadNewModelCB(void* clientData);
void TW_CALL copyStdStringToClient(std::string& dst, const std::string& src);

//-----------------------------------------------------------------------------
// Name: cbDisplay()
// Desc: 
//-----------------------------------------------------------------------------
void cbDisplay()
{
	g_tree_lod0_count = 0;
	g_tree_lod1_count = 0;
	g_tree_lod2_count = 0;

	if (!(g_tree_wind_direction == act_wind_val)){
		g_wind_dirty = true;
		g_tree_wind_direction = act_wind_val;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	// Setup OpenGL states according to user settings
	glAlphaFunc(GL_GEQUAL, g_AlphaThreshold);

	glPolygonMode(GL_FRONT_AND_BACK, g_WireMode ? GL_LINE : GL_FILL);

	g_time=timer.RealTime();
	g_float_time = g_time;
	p_world->update(g_time);

	// if timer query available
	if (tqAvailable){
		// measure on GPU
		glBeginQuery(GL_TIME_ELAPSED, tqid);
	} 
	
	// now draw all the things needed
	p_world->draw();
	// draw done

	if (tqAvailable){
		glEndQuery(GL_TIME_ELAPSED);
		GLuint64EXT time = 0;
		glGetQueryObjectui64vEXT(tqid, GL_QUERY_RESULT, &time); // blocking CPU
		g_Statistics.fps = 1000000000.0/ double(time);
		g_Statistics.minFps = min(g_Statistics.minFps, g_Statistics.fps);
	}
	g_timeDiff = timer.RealTime() - g_time;
	g_Statistics.cpuFps = 1.0 / (g_timeDiff);
	g_wind_dirty = false;
}

void initApp()
{
#if TEST
	// TEST START

	// do whatever u want... 

	system("PAUSE");

	exit(1);
	// TEST END
#endif
	timer.Reset();
	timer.Start();
	// set cube vbo
	initCube();
	//set plane vbo
	initPlane();
	p_world = new World();
	p_world->init();

	// timer query extension?
	if (isExtensionSupported(TIME_QUERY_EXTENSION)){
		tqAvailable = true;
		glGenQueries(1, &tqid);
	}

	glGenQueries(1, & samples_query_id1);
	glGenQueries(1, & samples_query_id2);
	//pqAvailable = true;
	//glGenQueries(1, &pqid);


}
void deinitApp()
{
	SAFE_DELETE_PTR (p_world);
	deletePlane();
	deleteCube();

	timer.Stop();

	//world.~World();
	if (tqAvailable){
		glDeleteQueries(1, &tqid);
	}
	//if (pqAvailable){
	//	glDeleteQueries(1, &pqid);
	//}
	printf("deinit done, bye\n");
	//system("PAUSE");
}

//-----------------------------------------------------------------------------
// Name: cbInitGL()
// Desc: 
//-----------------------------------------------------------------------------
void cbInitGL()
{
	g_Statistics.minFps = FLT_MAX;
	// init app
	initApp();

	// Init app GUI
	initGUI();

	// Set OpenGL state variables
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT_AND_BACK,  GL_AMBIENT, material_amd);
	glMaterialfv(GL_FRONT_AND_BACK,  GL_DIFFUSE, material_amd);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_spe);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glPointSize(1.f);
	glLineWidth(1.0f);


	glGetIntegerv(GL_MAX_SAMPLES, &g_multisample_count);
	printf("MS= %i\n", g_multisample_count);

}

void cbDeinitGL()
{
	deinitApp();
	printf("deinit GL\n");
}

void TW_CALL cbResetMinFPS(void* clientData)
{
	//p_world->snapTree(g_snapshot_direction);
	g_Statistics.minFps = FLT_MAX;
} 

void TW_CALL cbSetDTreeCount(const void *value, void *clientData)
{ 
	g_TreeDCount = *(const int*)value; // for instance
	p_world->dtree_planter.setInstanceCount(g_TreeDCount);
	//p_world->treeD_planter.plantVegetationCount(g_TreeDCount);
}
void TW_CALL cbGetDTreeCount(void *value, void *clientData)
{ 
	*(int *)value = p_world->dtree_planter.count; // for instance
}
void TW_CALL cbSetDTreeMeanDistance(const void *value, void *clientData)
{ 
	float distance = *(const float*)value; // for instance
	float dither = distance*0.4;
	int count = p_world->dtree_planter.count;
	p_world->dtree_planter.createCandidates(p_world->dtree_planter.height_min, p_world->dtree_planter.height_max, dither, distance);
	// now is p_world->dtree_planter.count=0;
	// use former count
	p_world->dtree_planter.setInstanceCount(count);
	//p_world->treeD_planter.plantVegetationCount(g_TreeDCount);
}
void TW_CALL cbGetDTreeMeanDistance(void *value, void *clientData)
{ 
	*(float *)value = p_world->dtree_planter.distance; // for instance
}

void TW_CALL cbSetDTreeAreaFactor(const void *value, void *clientData)
{ 
	int count = p_world->dtree_planter.count;
	g_tree_areaFactor = *(const float *)value;
	p_world->dtree_planter.size_factor = g_tree_areaFactor; 
	p_world->dtree_planter.createCandidates(p_world->dtree_planter.height_min, p_world->dtree_planter.height_max, p_world->dtree_planter.dither, p_world->dtree_planter.distance);
	// now is p_world->dtree_planter.count=0;
	// use former count
	p_world->dtree_planter.setInstanceCount(count);
	//p_world->treeD_planter.plantVegetationCount(g_TreeDCount);
}

void TW_CALL cbGetDTreeAreaFactor(void *value, void *clientData)
{ 
	*(float *)value = g_tree_areaFactor;
	//*(float *)value = p_world->dtree_planter.size_factor; // for instance
}

void TW_CALL cbGetDTreeMin(void *value, void *clientData)
{ 
	*(float *)value = p_world->dtree_planter.height_min; // for instance
}

void TW_CALL cbSetDTreeMin(const void *value, void *clientData)
{ 
	int count = p_world->dtree_planter.count;
	p_world->dtree_planter.height_min = *(const float*)value; 
	p_world->dtree_planter.createCandidates(p_world->dtree_planter.height_min, p_world->dtree_planter.height_max, p_world->dtree_planter.dither, p_world->dtree_planter.distance);
	// now is p_world->dtree_planter.count=0;
	// use former count
	p_world->dtree_planter.setInstanceCount(count);
	//p_world->treeD_planter.plantVegetationCount(g_TreeDCount);
}
void TW_CALL cbGetDTreeMax(void *value, void *clientData)
{ 
	*(float *)value = p_world->dtree_planter.height_max; // for instance
}

void TW_CALL cbSetDTreeMax(const void *value, void *clientData)
{ 
	int count = p_world->dtree_planter.count;
	p_world->dtree_planter.height_max = *(const float*)value; 
	p_world->dtree_planter.createCandidates(p_world->dtree_planter.height_min, p_world->dtree_planter.height_max, p_world->dtree_planter.dither, p_world->dtree_planter.distance);
	// now is p_world->dtree_planter.count=0;
	// use former count
	p_world->dtree_planter.setInstanceCount(count);
	//p_world->treeD_planter.plantVegetationCount(g_TreeDCount);
}




void TW_CALL cbSetTree2Count(const void *value, void *clientData)
{ 
	g_Tree2Count = *(const int*)value; // for instance
	p_world->tree2_planter.plantVegetationCount(g_Tree2Count);
}
void TW_CALL cbGetTree2Count(void *value, void *clientData)
{ 
	*(int *)value = p_world->tree2_planter.count; // for instance
}

void TW_CALL cbSetTree1Count(const void *value, void *clientData)
{ 
	g_Tree1Count = *(const int*)value; // for instance
	p_world->tree1_planter.plantVegetationCount(g_Tree1Count);
}
void TW_CALL cbGetTree1Count(void *value, void *clientData)
{ 
	*(int *)value = p_world->tree1_planter.count; // for instance
}

void TW_CALL cbSetGrassCount(const void *value, void *clientData)
{ 
	g_GrassCount = *(const int*)value;  // for instance
	p_world->grass_planter.plantVegetationCount(g_GrassCount);
}
void TW_CALL cbGetGrassCount(void *value, void *clientData)
{ 
	*(int *)value = p_world->grass_planter.count;  // for instance
}

//tree2
void TW_CALL cbSetTree2Min(const void *value, void *clientData)
{ 
	tree2min = *(const float*)value;  // for instance
	p_world->tree2_planter.setNewMin(tree2min);
}
void TW_CALL cbGetTree2Min(void *value, void *clientData)
{ 
	*(float *)value = p_world->tree2_planter.height_min;  // for instance
}

void TW_CALL cbSetTree2Max(const void *value, void *clientData)
{ 
	tree2max = *(const float*)value;  // for instance
	p_world->tree2_planter.setNewMax(tree2max);
}
void TW_CALL cbGetTree2Max(void *value, void *clientData)
{ 
	*(float *)value = p_world->tree2_planter.height_max;  // for instance
}

// tree1
void TW_CALL cbSetTree1Min(const void *value, void *clientData)
{ 
	tree1min = *(const float*)value;  // for instance
	p_world->tree1_planter.setNewMin(tree1min);
}
void TW_CALL cbGetTree1Min(void *value, void *clientData)
{ 
	*(float *)value = p_world->tree1_planter.height_min;  // for instance
}

void TW_CALL cbSetTree1Max(const void *value, void *clientData)
{ 
	tree1max = *(const float*)value;  // for instance
	p_world->tree1_planter.setNewMax(tree1max);
}
void TW_CALL cbGetTree1Max(void *value, void *clientData)
{ 
	*(float *)value = p_world->tree1_planter.height_max;  // for instance
}


// grass
void TW_CALL cbSetGrassMin(const void *value, void *clientData)
{ 
	grassmin = *(const float*)value;  // for instance
	p_world->grass_planter.setNewMin(grassmin);
}
void TW_CALL cbGetGrassMin(void *value, void *clientData)
{ 
	*(float *)value = p_world->grass_planter.height_min;  // for instance
}

void TW_CALL cbSetGrassMax(const void *value, void *clientData)
{ 
	grassmax = *(const float*)value;  // for instance
	p_world->grass_planter.setNewMax(grassmax);
}
void TW_CALL cbGetGrassMax(void *value, void *clientData)
{ 
	*(float *)value = p_world->grass_planter.height_max;  // for instance
}

void TW_CALL cbSetWindDir(const void *value, void *clientData)
{ 
	memcpy( g_tree_wind_direction.data, value, 3*sizeof(float));
	g_wind_dirty = true;
}
void TW_CALL cbGetWindDir(void *value, void *clientData)
{ 
	value = & g_tree_wind_direction.data;  // for instance
}


//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: 
//-----------------------------------------------------------------------------
void initGUI()
{
#ifdef USE_ANTTWEAKBAR
	// Initialize AntTweakBar GUI
	if (!TwInit(TW_OPENGL, NULL))
	{
		assert(0);
	}

	// Define the required callback function to copy a std::string 
	// (see TwCopyStdStringToClientFunc documentation)
	TwCopyStdStringToClientFunc(copyStdStringToClient); 

	TwWindowSize(g_WinWidth, g_WinHeight);
	TwBar *controlBar = TwNewBar("Controls");
	TwDefine(" Controls position='0 0' size='250 550' refresh=0.3 \
			 valueswidth=80 ");


// Visibility
	TwAddVarRW(controlBar, "sw_lod0", TW_TYPE_BOOLCPP, & g_draw_lod0, " group='Visibility' label='show LOD0' ");
	TwAddVarRW(controlBar, "sw_lod1", TW_TYPE_BOOLCPP, & g_draw_lod1, " group='Visibility' label='show LOD1' ");
	TwAddVarRW(controlBar, "sw_lod2", TW_TYPE_BOOLCPP, & g_draw_lod2, " group='Visibility' label='show LOD2' ");
	TwAddVarRW(controlBar, "sw_veg", TW_TYPE_BOOLCPP, & g_draw_low_vegetation, " group='Visibility' label='show other vegetation'  ");

//CAMERA
	TwEnumVal cam_mode[] = 
	{ 
		{ CameraMode::FREE					, "Free"					},
		{ CameraMode::TERRAIN_RESTRICTED	, "Terrain restricted"     },
		{ CameraMode::TERRAIN_CONNECTED		, "Terrain connected"      },
		{ CameraMode::WALK					, "Walk"					}

	};

	TwType transport_type = TwDefineEnum("Camera mode", cam_mode, 
		4);
	TwAddVarRW(controlBar, "camera mode", transport_type, &g_cameraMode, 
		" group='Camera' keyIncr=c \
		help='Change camera movement mode.' ");
	TwAddVarRW(controlBar, "position x", TW_TYPE_FLOAT, &(p_world->p_activeCamera->position.x), " group='Camera' ");
	TwAddVarRW(controlBar, "position y", TW_TYPE_FLOAT, &(p_world->p_activeCamera->position.y), " group='Camera' ");
	TwAddVarRW(controlBar, "position z", TW_TYPE_FLOAT, &(p_world->p_activeCamera->position.z), " group='Camera' ");
	TwAddVarRW(controlBar, "direction", TW_TYPE_DIR3F, &(p_world->p_activeCamera->direction.data), " group='Camera' ");


// STATISTICS
	//TwAddVarRO(controlBar, "LOD12_sampleCount", TW_TYPE_DOUBLE, &(samples_lod12), " group=Statistics ");
	TwAddVarRO(controlBar, "LOD0_count", TW_TYPE_INT32, &(g_tree_lod0_count), " label='# LOD0' group=Statistics ");
	TwAddVarRO(controlBar, "LOD1_count", TW_TYPE_INT32, &(g_tree_lod1_count), " label='# LOD1' group=Statistics ");
	TwAddVarRO(controlBar, "LOD2_count", TW_TYPE_INT32, &(g_tree_lod2_count), " label='# LOD2' group=Statistics ");
	TwAddVarRO(controlBar, "LOD01_count", TW_TYPE_INT32, &(g_tree_lod01_count), " label='# LOD01' group=Statistics ");
	TwAddVarRO(controlBar, "LOD12_count", TW_TYPE_INT32, &(g_tree_lod12_count), " label='# LOD12' group=Statistics ");
	//TwAddVarRO(controlBar, "min GPU_fps", TW_TYPE_FLOAT, &(g_Statistics.minFps), 
	//	" label='min GPU fps' group=Statistics help='frames per second (measured on GPU) - minimum' ");
	//TwAddButton(controlBar, "RESET MIN FPS", cbResetMinFPS, NULL, " group='Statistics' ");
	TwAddVarRO(controlBar, "GPU_fps", TW_TYPE_FLOAT, &(g_Statistics.fps), " label='fps GPU' group=Statistics help='frames per second (measured on GPU)' ");
	TwAddVarRO(controlBar, "CPU_fps", TW_TYPE_FLOAT, &(g_Statistics.cpuFps), 
		" label='fps CPU' group=Statistics help='frames per second (measured on GPU)' ");  
	
// LIGHT
	TwAddVarRW(controlBar, "g_god_expo			", TW_TYPE_FLOAT, &(g_god_expo			), " label='rays exposure'			group=Light min=0 max=5 step=0.01");  
	TwAddVarRW(controlBar, "g_god_density		", TW_TYPE_FLOAT, &(g_god_density		), " label='rays density'			group=Light min=0 max=5 step=0.01");  
	TwAddVarRW(controlBar, "shadows", TW_TYPE_BOOLCPP, &(g_ShadowMappingEnabled), 
		" label='shadow mapping enabled' group=Light help='enable/disable shadows' ");  
	TwAddVarRW(controlBar, "godrays", TW_TYPE_BOOLCPP, &(g_godraysEnabled), 
		" label='lightshafts enabled' group=Light help='enable/disable god rays' ");  
	TwAddVarRW(controlBar, "light_direction", TW_TYPE_DIR3F, &(g_light_direction), 
		" label='direction' group=Light help='adjust direction of light' ");  
	TwAddVarRW(controlBar, "light_position", TW_TYPE_DIR3F, &(g_light_position), 
		" label='position' group=Light help='adjust position of light' ");  
	TwAddVarRW(controlBar, "light_dir", TW_TYPE_BOOLCPP, &(g_light_showDir), 
		" label='show light volume' group=Light help='enable/disable showing lightdir' ");  
// LOD transition
	TwEnumVal trans_mode[] = 
	{ 
		{ LODTransitionMethod::HARD_SWITCH				, "hard switch"					},
		{ LODTransitionMethod::CROSS_FADE				, "cross fade"					},
		{ LODTransitionMethod::FADE_IN_BACKGROUND		, "fade in background"			},
		{ LODTransitionMethod::SHIFTED_CROSS_FADE		, "shifted cross fade"			},
		{ LODTransitionMethod::SHIFTED_SOFT_FADE		, "shifted soft fade"			}
	};
	TwType transition_method = TwDefineEnum("LOD_transition_method", trans_mode, 5);
	TwAddVarRW(controlBar, "method", transition_method, &g_lodTransition, " group='LOD_transition' ");
	TwAddVarRW(controlBar, "shift", TW_TYPE_FLOAT, & g_transitionShift, " group='LOD_transition' min=0 max=0.5 step=0.01");
	TwAddVarRW(controlBar, "sw_lod_tresh0", TW_TYPE_FLOAT, & g_lodTresholds.x, " group='LOD_transition' label='treshold 0'  min=0 max=10000 step=0.1");
	TwAddVarRW(controlBar, "sw_lod_tresh1", TW_TYPE_FLOAT, & g_lodTresholds.y, " group='LOD_transition' label='treshold 1'  min=0 max=10000 step=0.1");
	TwAddVarRW(controlBar, "sw_lod_tresh2", TW_TYPE_FLOAT, & g_lodTresholds.z, " group='LOD_transition' label='treshold 2'  min=0 max=10000 step=0.1");
	TwAddVarRW(controlBar, "sw_lod_tresh3", TW_TYPE_FLOAT, & g_lodTresholds.w, " group='LOD_transition' label='treshold 3'  min=0 max=10000 step=0.1");

// Leaf appearance
	TwAddVarRW(controlBar, "Season",					TW_TYPE_FLOAT,		& g_season							, " group='Leaf_appearance' min=-1 max=2 step=0.01 label='season'");
	TwAddVarRW(controlBar, "MultiplyAmbient",			TW_TYPE_FLOAT,		&g_leaves_MultiplyAmbient			, " group='Leaf_appearance' label='ambient coef.' min=0 max=5 step=0.001 ");
	TwAddVarRW(controlBar, "MultiplyDiffuse",			TW_TYPE_FLOAT,		&g_leaves_MultiplyDiffuse			, " group='Leaf_appearance' label='diffuse coef.' min=0 max=5 step=0.001");
	TwAddVarRW(controlBar, "MultiplySpecular",			TW_TYPE_FLOAT,		&g_leaves_MultiplySpecular			, " group='Leaf_appearance' label='specular coef.' min=0 max=10 step=0.001");
	TwAddVarRW(controlBar, "MultiplyTranslucency",		TW_TYPE_FLOAT,		&g_leaves_MultiplyTranslucency		, " group='Leaf_appearance' label='translucency coef.' min=0 max=5 step=0.001");
	TwAddVarRW(controlBar, "Shadow_intensity",			TW_TYPE_FLOAT,		&g_leaves_shadow_intensity			, " group='Leaf_appearance' label='shadow intensity' min=-10 max=10 step=0.001");

// Tree Dynamics	
	TwAddVarRW(controlBar, "wind_direction",  TW_TYPE_DIR3F, & act_wind_val.data,			" label='wind direction' group='Tree_Dynamics'");
	TwAddVarRW(controlBar, "wind_strength",   TW_TYPE_FLOAT, & g_tree_wind_strength,		" label='wind strength' group='Tree_Dynamics' min=0 max=5 step=0.01 ");
	TwAddSeparator(controlBar, NULL , " group='Tree_Dynamics' ");
	TwAddVarRW(controlBar, "wood0_frequency", TW_TYPE_FLOAT, & g_tree_wood_frequencies.x,	" label='frequency 0'  group='Tree_Dynamics' min=0 max=100 step=0.05 ");
	TwAddVarRW(controlBar, "wood1_frequency", TW_TYPE_FLOAT, & g_tree_wood_frequencies.y,	" label='frequency 1' group='Tree_Dynamics' min=0 max=100 step=0.05 ");
	TwAddVarRW(controlBar, "wood2_frequency", TW_TYPE_FLOAT, & g_tree_wood_frequencies.z,	" label='frequency 2' group='Tree_Dynamics' min=0 max=100 step=0.05 ");
	TwAddVarRW(controlBar, "wood3_frequency", TW_TYPE_FLOAT, & g_tree_wood_frequencies.w,	" label='frequency 3' group='Tree_Dynamics' min=0 max=100 step=0.05 ");
	TwAddVarRW(controlBar, "leaf_frequency",  TW_TYPE_FLOAT, & g_tree_leaf_frequency,		" label='frequency leaves' group='Tree_Dynamics' min=0 max=100 step=0.1 ");
	TwAddSeparator(controlBar, NULL, " group='Tree_Dynamics' ");
	TwAddVarRW(controlBar, "wood0_amplitude", TW_TYPE_FLOAT, & g_tree_wood_amplitudes.x,	" label='amplitude 0' group='Tree_Dynamics' min=0 max=10 step=0.01 ");
	TwAddVarRW(controlBar, "wood1_amplitude", TW_TYPE_FLOAT, & g_tree_wood_amplitudes.y,	" label='amplitude 1' group='Tree_Dynamics' min=0 max=10 step=0.01 ");
	TwAddVarRW(controlBar, "wood2_amplitude", TW_TYPE_FLOAT, & g_tree_wood_amplitudes.z,	" label='amplitude 2' group='Tree_Dynamics' min=0 max=10 step=0.01 ");
	TwAddVarRW(controlBar, "wood3_amplitude", TW_TYPE_FLOAT, & g_tree_wood_amplitudes.w,	" label='amplitude 3' group='Tree_Dynamics' min=0 max=10 step=0.01 ");
	TwAddVarRW(controlBar, "leaf_amplitude",  TW_TYPE_FLOAT, & g_tree_leaf_amplitude,		" label='amplitude leaves' group='Tree_Dynamics' min=0 max=10 step=0.01 ");


// VEGETATION 
	TwAddVarCB(controlBar, "count", TW_TYPE_INT32, cbSetDTreeCount, cbGetDTreeCount, NULL,						 " label='d.tree count' group='Vegetation' min=0 max=10000 step=1 ");
	TwAddVarCB(controlBar, "mean_distance", TW_TYPE_FLOAT, cbSetDTreeMeanDistance, cbGetDTreeMeanDistance, NULL, " label='d.tree distance' group='Vegetation' min=0 max=100 step=0.1 ");
	TwAddVarCB(controlBar, "area_factor", TW_TYPE_FLOAT, cbSetDTreeAreaFactor, cbGetDTreeAreaFactor, NULL,		 " label='d.tree area factor' group='Vegetation' min=0.001 max=1.0 step=0.01 ");
	TwAddVarCB(controlBar, "min_level", TW_TYPE_FLOAT, cbSetDTreeMin, cbGetDTreeMin, NULL,						 " label='d.tree min height' group='Vegetation' min=-100 max=100 step=0.1 ");
	TwAddVarCB(controlBar, "max_level", TW_TYPE_FLOAT, cbSetDTreeMax, cbGetDTreeMax, NULL,						 " label='d.tree max height' group='Vegetation' min=-100 max=100 step=0.1 ");
	TwAddSeparator(controlBar, NULL, " group='Vegetation' ");

	TwAddVarCB(controlBar, "Grass count", TW_TYPE_INT32, cbSetGrassCount, cbGetGrassCount, NULL,	" label='grass count'  group='Vegetation' min=0 max=100000 step=1 ");
	TwAddVarCB(controlBar, "Grass MIN", TW_TYPE_FLOAT, cbSetGrassMin, cbGetGrassMin, NULL,			" label='grass min height'  group='Vegetation' min=-5 max=30 step=1 ");
	TwAddVarCB(controlBar, "Grass MAX", TW_TYPE_FLOAT, cbSetGrassMax, cbGetGrassMax, NULL,			" label='grass max height'  group='Vegetation' min=-5 max=30 step=1 ");

	TwAddVarCB(controlBar, "Tree 1 count", TW_TYPE_INT32, cbSetTree1Count, cbGetTree1Count, NULL,				" label='tree1 count' group='Vegetation' min=0 max=10000 step=1 ");
	TwAddVarCB(controlBar, "Tree1 MIN", TW_TYPE_FLOAT, cbSetTree1Min, cbGetTree1Min, NULL, " label='tree1 min height' group='Vegetation' min=-5 max=30 step=1 ");
	TwAddVarCB(controlBar, "Tree1 MAX", TW_TYPE_FLOAT, cbSetTree1Max, cbGetTree1Max, NULL, " label='tree1 max height' group='Vegetation' min=-5 max=30 step=1 ");

	
	TwAddVarCB(controlBar, "Tree 2 count", TW_TYPE_INT32, cbSetTree2Count, cbGetTree2Count, NULL,				" label='tree 2 count' group='Vegetation' min=0 max=10000 step=1 ");
	TwAddVarCB(controlBar, "Tree2 MIN", TW_TYPE_FLOAT, cbSetTree2Min, cbGetTree2Min, NULL, " label='tree2 min height' group='Vegetation' min=-5 max=30 step=1 ");
	TwAddVarCB(controlBar, "Tree2 MAX", TW_TYPE_FLOAT, cbSetTree2Max, cbGetTree2Max, NULL, " label='tree2 max height' group='Vegetation' min=-5 max=30 step=1 ");

	TwDefine(" Controls/Statistics opened=false ");
	TwDefine(" Controls/Vegetation opened=false ");
	TwDefine(" Controls/Light opened=false ");
	TwDefine(" Controls/Camera opened=false ");
	TwDefine(" Controls/Leaf_appearance opened=false ");
	TwDefine(" Controls/LOD_transition opened=false ");
	TwDefine(" Controls/Tree_Dynamics opened=false ");

#endif
}


//-----------------------------------------------------------------------------
// Name: copyStdStringToClient()
// Desc: Function called to copy the content of a std::string (souceString) 
//       handled by the AntTweakBar library to destinationClientString handled 
//       by our application 
//-----------------------------------------------------------------------------
void TW_CALL copyStdStringToClient(std::string& dst, const std::string& src)
{
	dst = src;
} 


//-----------------------------------------------------------------------------
// Name: loadNewModelCB()
// Desc: Callback function to load new model
//-----------------------------------------------------------------------------
void TW_CALL loadNewModelCB(void* clientData)
{
	/*
	const std::string* file_name = &g_ModelFileName;//(const std::string *)(clientData);
	printf("RELOAD MODEL\n");
	if (!file_name->empty())
	{
	PGR2Model* pOldModel = g_pModel;
	printf("LOAD: %s\n", (*file_name).c_str());
	g_pModel = PGR2Model::loadFromFile(file_name->c_str());
	if (g_pModel != NULL)
	{
	delete pOldModel;
	}
	else
	{
	g_pModel = pOldModel;
	}
	}
	*/
} 


//-----------------------------------------------------------------------------
// Name: cbWindowSizeChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbWindowSizeChanged(int width, int height)
{
	g_WinWidth  = width;
	g_WinHeight = height;
	g_window_sizes.x = g_WinWidth;
	g_window_sizes.y = g_WinHeight;
	p_world->windowSizeChanged(width,height);
}
void activateANTMouse()
{
	glfwEnable( GLFW_MOUSE_CURSOR );
}
void activateGLFWMouse()
{
	//glfwDisable( GLFW_MOUSE_CURSOR );
	glfwSetMousePos(g_WinWidth/2, g_WinHeight/2);
}

//-----------------------------------------------------------------------------
// Name: cbKeyboardChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbKeyboardChanged(int key, int action)
{
	if (!g_MouseModeANT){
		// apply to camera first...
		if (p_world->p_activeCamera->handleKeyDown(key, action)){
			return;
		}
	}
	switch (key)
	{
		// DA use 'z' instead of 't'
		/*
		case 'z' : g_SceneTraZ  += 0.5f;                                   break;
		case 'Z' : g_SceneTraZ  -= (g_SceneTraZ > 0.5) ? 0.5f : 0.0f;      break;
		case 's' : g_SceneScale *= 1.01;                                   break;
		case 'S' : g_SceneScale *= 0.99;                                   break;
		case 'v' : g_ShowVertexNormals = !g_ShowVertexNormals;             break;
		case 'f' : g_FaceNormals != g_FaceNormals;                         break;
		case 't' : g_Transparency = !g_Transparency;                       break;
		case 'w' : g_WireMode    = !g_WireMode;                            break;
		case 'c' : g_FaceCulling = !g_FaceCulling;                         break;
		case 'a' : if(g_AlphaThreshold < 0.99f) g_AlphaThreshold += 0.01f; break;
		case 'A' : if(g_AlphaThreshold > 0.01f) g_AlphaThreshold -= 0.01f; break;
		*/
	case ' ' : 
		g_MouseModeANT = !g_MouseModeANT;
		if (g_MouseModeANT){
			activateANTMouse();
		} else {
			activateGLFWMouse();
		}
		break;
	}
}



bool g_MouseRotationEnabled = false;

//-----------------------------------------------------------------------------
// Name: cbMouseButtonChanged()
// Desc: internal
//-----------------------------------------------------------------------------
void GLFWCALL cbMouseButtonChanged(int button, int action)
{
	g_MouseRotationEnabled = ((button == GLFW_MOUSE_BUTTON_LEFT) && 
		(action == GLFW_PRESS));
}


//-----------------------------------------------------------------------------
// Name: cbMousePositionChanged()
// Desc: 
//-----------------------------------------------------------------------------
void cbMousePositionChanged(int x, int y)
{

	p_world->p_activeCamera->handleMouseMove(x,y);
	glfwSetMousePos(g_WinWidth/2, g_WinHeight/2);
}



//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
	printf(" _        _______ _________          _______  _______  _______ \n( (    /|(  ___  )\\__   __/|\\     /|(  ____ )(  ____ \\(  ___  )\n|  \\  ( || (   ) |   ) (   | )   ( || (    )|| (    \\/| (   ) |\n|   \\ | || (___) |   | |   | |   | || (____)|| (__    | (___) |\n| (\\ \\) ||  ___  |   | |   | |   | ||     __)|  __)   |  ___  |\n| | \\   || (   ) |   | |   | |   | || (\\ (   | (      | (   ) |\n| )  \\  || )   ( |   | |   | (___) || ) \\ \\__| (____/\\| )   ( |\n|/    )_)|/     \\|   )_(   (_______)|/   \\__/(_______/|/     \\|\n");
	printf("                                              Adam Kucera, 2011\n");
	printf("===============================================================\n");
	if (argc>1){
		g_samples = atoi(argv[1]); // samples requested 
	} else {
		g_samples = 1;
	}
	int output = common_main(g_WinWidth, g_WinHeight,
		"NATUREA diploma thesis project",
		cbInitGL,              // init GL callback function
		cbDeinitGL,
		cbDisplay,             // display callback function
		cbWindowSizeChanged,   // window resize callback function
		cbKeyboardChanged,     // keyboard callback function
#ifdef USE_ANTTWEAKBAR
		cbMouseButtonChanged,                  // mouse button callback function
		cbMousePositionChanged,                  // mouse motion callback function
#else
		cbMouseButtonChanged,  // mouse button callback function
		cbMousePositionChanged, // mouse motion callback function
#endif
		&g_samples
		);
	//deinitApp();

	return output;
}
