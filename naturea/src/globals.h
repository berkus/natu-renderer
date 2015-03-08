#ifndef _GLOBALS_H
#define _GLOBALS_H
#include "GLEW/glew.h"
#include "GL/glut.h"
#include "timer.h"

#include "Vector3.h"
#include "Vector4.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "utility\Texture.h"

class Texture;

enum CameraMode{
	FREE,
	TERRAIN_RESTRICTED,
	TERRAIN_CONNECTED,
	WALK
};
enum Sorting{
		SORT_FRONT_TO_BACK,
		SORT_BACK_TO_FRONT,
		DO_NOT_SORT
};


enum LODTransitionMethod{
	HARD_SWITCH,
	CROSS_FADE,
	FADE_IN_BACKGROUND,  // paper by Markus Giegl & Michael Wimmer
	SHIFTED_CROSS_FADE,	 // new ?
	SHIFTED_SOFT_FADE	 // new ?
};

struct Statistics{
	float	fps;
	int		primitives;
	float	minFps;
	float	cpuFps;
};

static int g_nula = 0;

extern	GLuint					samples_query_id1;
extern	GLuint					samples_query_id2;
extern  double					samples_lod12;

extern	v3						g_ShadowMapSize;
extern	int						g_shadowMapSamples;
extern  bool					g_wind_dirty;

extern	int						g_samples;
extern	int						g_offset;
extern	float					g_cosA	;
extern	float					g_cosB	;
extern	float					g_cosC	;

extern	GLint					g_WinWidth;   // Window width
extern	GLint					g_WinHeight;   // Window height
extern	v3						g_window_sizes;	
extern	double					g_time;
extern	float					g_float_time;
extern	bool					g_godraysEnabled;
extern	bool					g_fastMode;
extern	v4						g_light_position;
extern	v4						g_light_direction;
extern	bool					g_light_showDir;
extern	bool					g_drawingReflection;
extern	bool					g_showTextures;
extern  bool					g_ParallaxMappingEnabled;
extern  float					g_ParallaxScale;
extern  float					g_ParallaxBias;

extern	float					g_god_expo			  ;
extern	float					g_god_decay			  ;
extern	float					g_god_density		  ;
extern	float					g_god_weight		  ;
extern	float					g_illuminationDecay	  ;
extern  float					g_bloomDivide;


extern	bool					g_draw_lod0				;
extern	bool					g_draw_lod1				;
extern	bool					g_draw_lod2				;
extern	v4						g_lodTresholds			;
extern	bool					g_draw_dtree_lod		;
extern	bool					g_draw_low_vegetation	;
extern	bool					g_draw_dtree			;
extern	bool					g_draw_light_direction  ;

extern	bool					g_ShadowMappingEnabled;
extern	bool					g_Draw2Shadowmap;
//extern	m4						g_LightMVPCameraVInverseMatrix;
extern	m4						g_LightMVCameraVInverseMatrix;
extern	m4						g_LightPMatrix;

extern	int						g_multisample_count;
extern	GLuint					g_screen_multi_framebuffer;

extern	CameraMode				g_cameraMode;

extern	Statistics				g_Statistics;
extern	v4						g_terrain_border_values;
extern	v4						g_terrain_border_widths;

extern int						g_GrassCount;
extern int						g_Tree1Count;
extern int						g_Tree2Count;
extern int						g_TreeDCount;

extern int						g_Bumpmaps	   ;
extern int						g_Heightmaps   ;
extern int						g_Specularmaps ;
extern int						g_Alphamaps	   ;


extern bool						g_draw_lod1_method;
extern bool						g_orbit			;
extern float					g_orbit_speed	;
extern float					g_orbit_radius	;
extern v3						g_center		;
extern float					g_timeDiff		;

/**********************************************
* DYNAMIC TREE
*/
extern float	g_tree_areaFactor;
extern v3		g_tree_wind_direction;
extern float	g_tree_wind_strength;
extern v4		g_tree_wood_amplitudes;
extern v4		g_tree_wood_frequencies;
extern float	g_tree_leaf_amplitude;
extern float	g_tree_leaf_frequency;
extern float	g_dtreemin;
extern float	g_dtreemax;

/**********************************************
* LOD TREE
*/
extern int		g_tree_slice_count;
extern int		g_tree_show_slice;
extern int		g_tree_show_sliceSet;
extern float	g_tree_wave_amplitude;
extern float	g_tree_wave_frequency;
extern v3		g_tree_movementVectorA;
extern v3		g_tree_movementVectorB;
extern float	g_tree_wave_y_offset;
extern float	g_tree_wave_increase_factor;
extern float	g_tree_time_offset_1;
extern float	g_tree_time_offset_2;

extern float	g_leaves_MultiplyAmbient				;
extern float	g_leaves_MultiplyDiffuse				;
extern float	g_leaves_MultiplySpecular				;
extern float	g_leaves_MultiplyTranslucency			;
extern float	g_leaves_ReduceTranslucencyInShadow		;
extern float	g_leaves_shadow_intensity				;
extern v3		g_leaves_LightDiffuseColor				;

extern v3		g_snapshot_direction;
extern int		g_slice_count;

extern const int g_tree_gridSize			;
extern float	g_tree_mean_distance	;
extern float	g_tree_dither			;

extern int		g_tree_lod0_count;
extern int		g_tree_lod1_count;
extern int		g_tree_lod2_count;
extern int		g_tree_lod01_count;
extern int		g_tree_lod12_count;

extern bool		g_compressToOneTexture;

extern v3*		g_viewer_position;
extern v3*		g_viewer_direction;

extern float	g_fog_density, g_fog_start, g_fog_end;
extern	bool	g_debug;

extern v3		g_tintColor ;
extern float	g_tintFactor;

extern float	g_varA;
extern float	g_transitionShift;
extern float	g_transitionControl;

extern float	g_gauss_shift;
extern float	g_gauss_steep;
extern float	g_gauss_weight;


extern float	g_season;
extern LODTransitionMethod g_lodTransition;
extern Texture* g_shadowmap1;
extern m4	  * g_LightMVPmatrix;
extern m4	  * g_LightMVPCameraVInverseMatrix;
extern float	g_ShadowNear;
extern float	g_ShadowFar;
extern float	g_ShadowFarMNear;
extern float	g_CameraNear;
extern float	g_CameraFar ;

extern float	g_dither;

extern Sorting	g_sorting;
#endif