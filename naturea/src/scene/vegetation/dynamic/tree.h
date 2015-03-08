#ifndef _TREE_H
#define _TREE_H

#include "utility/shader.h" 
#include "TreeBranch.h"
#include "TreeLeaf.h"
#include "utility/Shader.h"
#include "utility/TextureManager.h"
#include "IO/OBJTfile.h"

#include "settings.h"
#include "scene/sceneModel.h"

// #include "TreeLeaf.h"
enum VERTEX_ATTRIBUTES{
	 TREE_POSITION				,
	 TREE_NORMAL				,
	 TREE_BINORMAL				,
	 TREE_TANGENT				,
	 TREE_COLOR0				,
	 TREE_COLOR1				,
	 TREE_TEXCOORD0				,
	 TREE_TEXCOORD1				,
	 TREE_XVALS					,
	 TREE_TIME					,
	 TREE_BRANCH_INDEX			,
	 TREE_BRANCH_DATA_TEXTURE	,
	 TREE_BRANCH_TEXTURE0		,
	 TREE_BRANCH_TEXTURE1		,
	 TREE_BRANCH_TEXTURE2		,
	 TREE_BRANCH_TEXTURE3		,
	 TREE_NOISE_TEXTURE0		,
	 TREE_NOISE_TEXTURE1		,
	 TREE_NOISE_TEXTURE2		,
	 TREE_NOISE_TEXTURE3		,
	 TREE_NOISE_TEXTURE4		,
	 TREE_BRANCH_COUNT			, 
	 TREE_WIND_DIRECTION		,
	 TREE_WIND_STRENGTH			,
	 TREE_WOOD_AMPLITUDE		,
	 TREE_COLOR_TEXTURE0		,

	 TREE_VBO_VERTEX_COMPONENTS
};


using namespace std;

extern bool		g_bShadersSupported;
extern bool		g_bUseShaders;

class Tree:
	public SceneModel
{
public:
	Tree(TextureManager *texManager, ShaderManager *shManager);
	~Tree();
	
	virtual void draw();

	virtual void drawForLOD();

	virtual void init();

	virtual void update(double time);

	void translate(v3 &movVector);

	void rotate(v3 &axis, float angleRad);

	void scale(v3 &scaleVector);

	void load(string filename, TextureManager *texManager);
	void save(string filename);

	void setTime(float _time);

	

	GLuint				branchShaderID;
	GLuint				leafShaderID;

	void				createDataTexture();
	int					linearizeHierarchy();
	void				recalcCoordSystems();
	void				fillParentDataForEachBranch();
	void				createBranchesVBO();
	void				fillParentDataForEachLeaf();
	void				createLeavesVBO();

	TreeBranch			*trunk;
	vector<TreeBranch*>	branches;
	vector<TreeLeaf*>	leaves;

	Texture*			dataTexture;
	Texture*			lColorTexture;
	Texture*			bColorTexture;
	Texture*			branchNoiseTexture;
	Texture*			leafNoiseTexture;

	GLuint				branchVBOid;
	GLuint				branchEBOid;
	GLuint				branchEBOcount;

	GLuint				leafVBOid;
	GLuint				leafEBOid;
	GLuint				leafEBOcount;

	int					offsets[  TREE_VBO_VERTEX_COMPONENTS];
	int					sizes[    TREE_VBO_VERTEX_COMPONENTS];
	GLint				locations[TREE_VBO_VERTEX_COMPONENTS];

	int					leafOffsets[  TREE_VBO_VERTEX_COMPONENTS];
	int					leafSizes[    TREE_VBO_VERTEX_COMPONENTS];
	GLint				leafLocations[TREE_VBO_VERTEX_COMPONENTS];
	int					leafVerticesCount;
	
	GLfloat				*vbo_data[TREE_VBO_VERTEX_COMPONENTS];
	GLuint 				*ebo_data;
	
	//GLuint				leavesVBOid;
	//void				*leavesData;

	GLuint				dataTextureID;
	GLuint				bnoiseTextureID;
	GLuint				lnoiseTextureID;
	GLfloat				branch_count;
	

private:
	// create texture containing tree data
	
	int texDimX, texDimY;
	

	//vector<TreeLeaf*>	leaves2;
	
	GLuint hvd_shaderProgram;
	GLuint hvd_vs;
	GLuint hvd_gs;
	GLuint hvd_fs;
	float time;
	bool makeShader(const char* vsfilename,const char* gsfilename,const char* fsfilename, GLuint &programID, GLuint &vertexShader, GLuint &geometryShader,  GLuint &fragmentShader);
};



#endif