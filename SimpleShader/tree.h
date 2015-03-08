#ifndef _TREE_H
#define _TREE_H

#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h" 
#include "../_utils/shaders.h" 
//#include "branch.h"
#include "TreeBranch.h"
#include "TreeLeaf.h"
#include "Shader.h"
#include "TextureManager.h"
#include "OBJTfile.h"

#include "settings.h"

// #include "TreeLeaf.h"
enum VERTEX_ATTRIBUTES{
	 POSITION             ,
	 NORMAL		          ,
	 BINORMAL	          ,
	 TANGENT	          ,
	 COLOR0		          ,
	 COLOR1		          ,
	 TEXCOORD0	          ,
	 TEXCOORD1	          ,
	 XVALS		          ,
	 TIME				  ,
	 BRANCH_INDEX         ,
	 BRANCH_DATA_TEXTURE  ,
	 BRANCH_TEXTURE0	  ,
	 BRANCH_TEXTURE1	  ,
	 BRANCH_TEXTURE2	  ,
	 BRANCH_TEXTURE3	  ,
	 NOISE_TEXTURE0		  ,
	 NOISE_TEXTURE1		  ,
	 NOISE_TEXTURE2		  ,
	 NOISE_TEXTURE3		  ,
	 NOISE_TEXTURE4		  ,
	 BRANCH_COUNT		  , 
	 WIND_DIRECTION		  ,
	 WIND_STRENGTH		  ,
	 WOOD_AMPLITUDE 	  ,
	 WOOD_FREQUENCY	 	  ,
	 LEAF_AMPLITUDE	 	  ,
	 LEAF_FREQUENCY	 	  ,
	 COLOR_TEXTURE0		  ,

	 VBO_VERTEX_COMPONENTS
};


using namespace std;

extern bool		g_bShadersSupported;
extern bool		g_bUseShaders;

class Tree{
public:
	Tree();
	~Tree();
	
	// create texture & set-up for drawing...
	void init();
	
	void draw();

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

	int					offsets[VBO_VERTEX_COMPONENTS];
	int					sizes[VBO_VERTEX_COMPONENTS];
	GLint				locations[VBO_VERTEX_COMPONENTS];

	int					leafOffsets[VBO_VERTEX_COMPONENTS];
	int					leafSizes[VBO_VERTEX_COMPONENTS];
	GLint				leafLocations[VBO_VERTEX_COMPONENTS];
	int					leafVerticesCount;
	
	GLfloat				*vbo_data[VBO_VERTEX_COMPONENTS];
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