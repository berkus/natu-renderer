#pragma once
#include "..\Vegetation.h"
#include "..\..\..\IO\OBJTfile.h"
#include "DTreeBranch.h"
#include "DTreeLeaf.h"
#include "settings.h"
#include "../../../utility/Texture.h"
#include "../../../utility/EBO.h"
#include "../../../utility/VBO.h"

class DTreeSlice{
public:
	DTreeSlice(){
		colormap	= NULL;
		normalmap	= NULL;
		depthmap	= NULL;
		branchmap	= NULL;
		datamap		= NULL;
	}
	~DTreeSlice(){
		printf("deleting slice\n");
		SAFE_DELETE_PTR(	colormap	);
		SAFE_DELETE_PTR(	normalmap	);
		SAFE_DELETE_PTR(	depthmap	);
		SAFE_DELETE_PTR(	branchmap	);
		SAFE_DELETE_PTR(	datamap		);
		printf("deleted slice\n");
	}

	Texture *	colormap;
	Texture *	normalmap;
	Texture *	depthmap;
	Texture *	branchmap;
	Texture *	datamap;
	
	float		getSortValue(v3 &viewer_position) {
					return (controlPoint-viewer_position).length();
				}

	//			transformed direction of the slice normal
	
private:
	v3			controlPoint;


};

class DTreeSliceSet{
public :
	DTreeSliceSet(){}
	~DTreeSliceSet(){
		for (int i=0; i<slices.size(); i++){
			SAFE_DELETE_PTR( slices[i]	);
		}
	}
	v3			getNormal(){
					v3 n = v3(0.0, 0.0, -1.0);
					n.rotateY((90+rotation_y)*DEG_TO_RAD);
					return n;
				}
	void		setRotMatrix(m3 &_rotMatrix){
					rotMatrix = _rotMatrix;
				}
	void		setSlices(vector<DTreeSlice*> _slices){
					slices = _slices;
				}
	int			size(){
					return slices.size();
				}
	DTreeSlice*	getSlice(int id){
					return slices[id];
				}
	int			getSliceCnt(){
					return slices.size();
				}
	float		rotation_y; // degrees
private:
	vector<DTreeSlice*> slices;
	v3			normal;
	m3			rotMatrix;
};

struct DTreeInstanceData
{
	v3		position;
	float	rotation_y;
	m4		transformMatrix;
	v3		wind_dir;
	v3		dirA;
	v3		dirB;
	float	distance;
	float	discrepacy;
	v3		eye_dir;	
	int		index;
	float	alpha;
	int		offset;
	float   time_offset;
	v3		colorVariance;
};
struct DTreeInstanceDrawData
{
	int		index;
	float	alpha;
};

class DTree :
	public Vegetation
{
public:
	DTree(TextureManager *texManager, ShaderManager *shManager);
	DTree(DTree* copy);
	~DTree(void);

	bool loadOBJT(string filename);

	void createVBOs();
	void createDataTexture();

	Texture* createLODdataTexture(vector<Matrix4x4*> &MVPs);
	
	bool loadDataTexture(string filename);
	bool saveDataTexture(string filename);

	bool loadVBO(string filename);
	bool saveVBO(string filename);

	//Texture * renderToTexture()

	Vegetation* getCopy();
	void draw();
	void draw_instance_LOD0(DTreeInstanceData * instance, float alpha);
	void draw_instance_LOD1(DTreeInstanceData * instance, float alpha);
	void draw_all_instances_LOD1();
	void draw_instance_LOD2(DTreeInstanceData * instance, float alpha);
	void draw_all_instances_LOD2();
	void draw_instance_LOD2b(DTreeInstanceData * instance, float alpha);
	void draw_all_instances_LOD2b();

	
	void drawForLOD();
	/*
	void drawLOD0();
	void drawLOD1();
	void drawLOD1b();
	void drawLOD2();
	void draw2Shadowmap();
	*/
	void init();
	void initInstances(vector<v4> &positions_rotations);

	void setInstances(v4 ** positions_rotations, int count);
	void initLOD0();
	void initLOD1();
	void initLOD2b();
	void initLOD2();

	void update(double time);

	void bakeToVBO(void);

	void fixTexType(void);
	v3	 transformTexCoords(v3 &origTexCoords);


	BBox * getBBox(void);
	
	
	vector<DTreeSlice*>	slices;
	v3					position;
	v3*					viewer_position;
	v3*					viewer_direction;

	vector<DTreeInstanceData*>			tree_instances;
	vector<DTreeInstanceData*>			tree_instances_lod1;

private:
	void	joinSliceSetsTextures(
					Texture ** jColorMap,
					Texture ** jDataMap,
					Texture ** jDepthMap,
					Texture ** jNormalMap,
					vector<DTreeSliceSet*>* sliceSets,
					int sliceset_count,
					int slice_count,
					int resolution_x,
					int resolution_y
					);
	void	createSlices(vector<DTreeSlice*>* slices, int count,int resX, int resY, v3 & direction, v3 & right,  Matrix4x4 *mvp, bool half=true);

	void	makeTransition(float control, bool maskOld, DTreeInstanceData* instance, void (DTree::*drawLODA)(DTreeInstanceData*, float),  void (DTree::*drawLODB)(DTreeInstanceData*, float));
	void	enqueueInRenderList(DTreeInstanceData * instance);
	void	prepareForRender();
	void	render();
	int		linearizeHierarchy();
	void	recalcCoordSystems();
	void	fillParentDataForEachBranch();
	void	createBranchesVBO();
	void	createLeavesVBO();

	int swapCnt;
	/*
	// NORMALS
	VBO *				n_branchesVBO;
	VBO	*				n_leavesVBO;
	Shader *			n_branchShader;
	Shader *			n_leafShader;
	//void				initNormals();
	void				drawNormals(DTreeInstanceData * instance);
	*/

	// INSTANCES
	bool				instancesLocked;
	//v4**				positions_rotations;
	int					count;
	vector<DTreeInstanceData**> instancesInRenderQueues;
	vector<int>			countRenderQueues;
	GLuint				i_matricesBuffID;
	v3					wind_relative_direction;
	int					instanceFloatCount;
	// INSTANCES END

	
	DTreeBranch			*trunk;
	vector<DTreeBranch*> branches;
	vector<DTreeLeaf*>	 leaves;

	Shader *			branchShader;
	Shader *			leafShader;

	Shader *			branchShader_sh;
	Shader *			leafShader_sh;

	Shader*				bLODShader;
	Shader*				lLODShader;

	Texture*			seasonMap;

	Texture*			dataTexture;
	Texture*			lColorTexture;
	Texture*			frontDecalMap;
	Texture*			frontNormalMap;
	Texture*			frontTranslucencyMap;
	Texture*			frontHalfLife2Map;
	Texture*			backDecalMap;
	Texture*			backNormalMap;
	Texture*			backTranslucencyMap;
	Texture*			backHalfLife2Map;
	
	Texture*			bColorTexture;
	Texture*			bNormalTexture;

	Texture*			branchNoiseTexture;
	Texture*			leafNoiseTexture;

	int					texDimX, texDimY;
	int					branchCount;
	float				branchCountF;

	VBO *				branchesVBO;
	VBO	*				leavesVBO;
	EBO *				branchesEBO;

	GLfloat				*vbo_data[DYN_TREE::COUNT];
	GLuint 				*ebo_data;



	// LOD 1
	float					tree_time_offset;
	Uniform*				u_time_offset;

	int						l_color	  ;
	int						l_displ	  ;
	int						l_displ2  ;
	int						l_data	  ;
	int						l_normal  ;
	
	
	

	Texture*				colorMap;
	Texture*				colorMap2;
	Texture*				lod1_dataTexture;
	Texture*				lod2_dataTexture;
	Texture*				weightMap;
	Texture*				dataMap;
	vector<DTreeSliceSet*>	sliceSets;
	Shader	*				lod1shader;
	//VBO		*				lod1vbo;


	int						ctr;
	float					alpha_c;

	GLenum					eBranchmapInternalFormat;
	GLenum					eBranchmapFormat;
	GLenum					eBranchmapDataType;

	/***************************************
	* LOD 1 data
	*/
	v2						lod1_win_resolution;
	float					lod1_sliceCount;
	float					lod1_sliceSetCount;
	Shader	*				lod1_shader_shadow;
	vector<float *>			lod1_instanceMatrices;
	vector<int>				lod1_typeIndices;
	Texture*				lod1_jColorMap;
	Texture*				lod1_jDataMap;
	Texture*				lod1_jNormalMap;
	Texture*				lod1_jDepthMap;
	

	
	//GLuint					i_paramBuffID;
	//GLuint					v_ordAttribsBuffID;
	//GLuint					v_indicesBuffID;
	float					time_offset;

	GLint					tmLoc0;
	GLint					iaLoc1;
	GLint					iaLoc1_shadow;
	GLint					tmLoc0_shadow;
	GLint					iu1Loc1;
	GLint					iu0Loc1;
	int						isInstancingEnabled;

	// int						l2_color	;
	// int						l2_displ	;
	// int						l2_displ2	;
	// int						l2_data		;
	// int						l2_normal	;
	// int						l2_depth	;
	// int						l2_season  ;
	vector<DTreeSliceSet*>	lod1_sliceSets	;
	Shader	*				lod1_shader	;
	
	VBO		*				lod1_vbo	;
	EBO		*				lod1_ebo	;
	


	

	/****************************************************
	* LOD 2 data
	*/
	v2					lod2_win_resolution;
	Texture*			lod2_jColorMap;
	Texture*			lod2_jDataMap;
	Texture*			lod2_jNormalMap;
	Texture*			lod2_jDepthMap;
	float				lod2_sliceCount;
	float				lod2_sliceSetCount;
	vector<DTreeSliceSet*> lod2_sliceSets;
	Shader	*			lod2_shader;
	Shader	*			lod2_shader_shadow;

	VBO		*			lod2_vbo;
	EBO		*			lod2_ebo;
	vector<float *>		lod2_instanceMatrices;
	vector<int>			lod2_typeIndices;


	GLint				lod2_tmLoc0;
	GLint				lod2_iaLoc1;
	GLint				lod2_iaLoc1_shadow;
	GLint				lod2_tmLoc0_shadow;
	GLint				lod2_iu1Loc1;
	GLint				lod2_iu0Loc1;

	GLint				tm2Loc0;
	GLint				tm2Loc0_shadow;
	GLint				ia2Loc1;
	GLint				ia2Loc1_shadow;
	GLint				iu1_2Loc1;
	GLint				iu0_2Loc1;

	Texture	*			lod2color1;
	Texture *			lod2color2;
	Texture	*			lod2normal1;
	Texture *			lod2normal2;
	Texture	*			lod2branch1;
	Texture *			lod2branch2;
	Texture	*			lod2depth1;
	Texture *			lod2depth2;
	Texture *			lod2dataTexture;

	int					lod2loc_season_tex;
	int					lod2loc_colorVariance;
	int					lod2loc_leaf_tex;
	int					lod2loc_branch_tex;

	GLint				lod2loc_color_tex_1;
	GLint				lod2loc_color_tex_2;
	GLint				lod2loc_normal_tex_1;
	GLint				lod2loc_normal_tex_2;
	GLint				lod2loc_branch_tex_1;
	GLint				lod2loc_branch_tex_2;

};

