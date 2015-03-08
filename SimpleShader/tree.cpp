#include "tree.h"
#include <stack>

/* Constructor */
Tree::Tree(){
	dataTexture		= NULL;
	lColorTexture	= NULL;
	bColorTexture	= NULL;

	trunk			= NULL;

	hvd_shaderProgram = 0;
	hvd_vs			= 0;
	hvd_gs			= 0;
	hvd_fs			= 0;

	branchVBOid		= 0;
	texDimX			= 0;
	texDimY			= 0;

}

/* Destructor */
Tree::~Tree(){
	if (hvd_shaderProgram!=NULL){
		destroyShaderProgram(hvd_shaderProgram, &hvd_vs, &hvd_gs, &hvd_fs);
	}
			
	if (dataTexture!=NULL){
		delete dataTexture;
	}
	if (branchNoiseTexture!=NULL){
		delete branchNoiseTexture;
	}
	if (leafNoiseTexture!=NULL){
		delete leafNoiseTexture;
	}
	if (lColorTexture!=NULL){
		delete lColorTexture;
	}
	if (bColorTexture!=NULL){
		delete bColorTexture;
	}
}

void Tree::init(){
	// get loactions of proper attributes
	locations[NORMAL]  = glGetAttribLocation(branchShaderID, "normal");
	locations[TANGENT] = glGetAttribLocation(branchShaderID, "tangent");
	locations[XVALS]   = glGetAttribLocation(branchShaderID, "x_vals");
	locations[BRANCH_INDEX]        = glGetAttribLocation(branchShaderID, "branch_index");
	locations[BRANCH_DATA_TEXTURE] = glGetUniformLocation(branchShaderID, "data_tex");
	locations[TIME]	   = glGetUniformLocation(branchShaderID, "time");
	locations[BRANCH_COUNT]		   = glGetUniformLocation(branchShaderID, "branch_count");
	locations[NOISE_TEXTURE0]	   = glGetUniformLocation(branchShaderID, "branch_noise_tex");
	locations[NOISE_TEXTURE1]	   = glGetUniformLocation(branchShaderID, "leaf_noise_tex");
	locations[WIND_DIRECTION]	   = glGetUniformLocation(branchShaderID, "wind_direction");
	locations[WIND_STRENGTH]	   = glGetUniformLocation(branchShaderID, "wind_strength");
	locations[WOOD_AMPLITUDE]	   = glGetUniformLocation(branchShaderID, "wood_amplitudes");
	locations[WOOD_FREQUENCY]	   = glGetUniformLocation(branchShaderID, "wood_frequencies");
	locations[COLOR_TEXTURE0]		= glGetUniformLocation(branchShaderID, "color_texture");
	locations[TEXCOORD0]			= glGetAttribLocation(branchShaderID, "texCoords0");

	leafLocations[NORMAL]				= glGetAttribLocation(leafShaderID, "normal");
	leafLocations[TANGENT]				= glGetAttribLocation(leafShaderID, "tangent");
	leafLocations[XVALS]				= glGetAttribLocation(leafShaderID, "x_vals");
	leafLocations[BRANCH_INDEX]			= glGetAttribLocation(leafShaderID, "branch_index");
	leafLocations[TEXCOORD0]			= glGetAttribLocation(leafShaderID, "texCoords0");
	leafLocations[BRANCH_DATA_TEXTURE]	= glGetUniformLocation(leafShaderID, "data_tex");
	leafLocations[TIME]					= glGetUniformLocation(leafShaderID, "time");
	leafLocations[BRANCH_COUNT]			= glGetUniformLocation(leafShaderID, "branch_count");
	leafLocations[NOISE_TEXTURE0]		= glGetUniformLocation(leafShaderID, "branch_noise_tex");
	leafLocations[NOISE_TEXTURE1]		= glGetUniformLocation(leafShaderID, "leaf_noise_tex");
	leafLocations[WIND_DIRECTION]		= glGetUniformLocation(leafShaderID, "wind_direction");
	leafLocations[WIND_STRENGTH]		= glGetUniformLocation(leafShaderID, "wind_strength");
	leafLocations[WOOD_AMPLITUDE]		= glGetUniformLocation(leafShaderID, "wood_amplitudes");
	leafLocations[WOOD_FREQUENCY]		= glGetUniformLocation(leafShaderID, "wood_frequencies");
	leafLocations[LEAF_FREQUENCY]		= glGetUniformLocation(leafShaderID, "leaf_frequency");
	leafLocations[COLOR_TEXTURE0]		= glGetUniformLocation(leafShaderID, "color_texture");

	// create branch data texture
	createDataTexture();

	// create VBO for branch vertices
	createBranchesVBO();

	// create VBO for leaf vertices
	createLeavesVBO();

	// load noise texture for branches
	// TODO: via texture manager
	branchNoiseTexture = new Texture();
	branchNoiseTexture->loadTexture("textures/habel/Noise_20.png", true, GL_REPEAT, GL_LINEAR);
	// add branch color texture
	bColorTexture = new Texture();
	bColorTexture->loadTexture(TEX_WOOD1, false, GL_REPEAT, GL_LINEAR);
	
	leafNoiseTexture = new Texture();
	leafNoiseTexture->loadTexture("textures/habel/Noise_53.png", true, GL_REPEAT, GL_LINEAR);

	// add leaf color texture
	lColorTexture = new Texture();
	lColorTexture->loadTexture(TEX_LEAF1, false, GL_REPEAT, GL_LINEAR);



}

void Tree::createLeavesVBO(){
	// recalc coord systems...
	int i, sizeL = leaves.size(), size;
	TreeLeaf * leaf;
	for (i=0; i<sizeL; i++){
		leaf = leaves[i];
		leaf->init();
		if (leaf->parent!=NULL){
			leaf->cs = leaf->parent->originalCS.getSystemInThisSystem(leaf->originalCS);
		}
	}

	int vCnt = 0;
	// count total vertices & indices
	for (i=0; i<sizeL; i++){
		vCnt += leaves[i]->getVertexCount();
	}
	leafVerticesCount = vCnt;
	// get sizes
	for (i = 0; i < VBO_VERTEX_COMPONENTS; i++){
		leafSizes[i]=0;
	}
	leafSizes[POSITION] = 3;
	leafSizes[NORMAL]	= 3;
	leafSizes[TANGENT]	= 3;
	leafSizes[TEXCOORD0]= 2;
	leafSizes[BRANCH_INDEX]=1;
	leafSizes[XVALS]	= 4;
	int totalVertexSize = 0;
	for (i = 0; i < VBO_VERTEX_COMPONENTS; i++){
		vbo_data[i]= new GLfloat[vCnt*leafSizes[i]];
		totalVertexSize+=leafSizes[i];
	}

	// get offsets
	leafOffsets[POSITION] = 0;
	leafOffsets[NORMAL]   = leafOffsets[POSITION]  + vCnt * leafSizes[POSITION]	* sizeof(GLfloat);
	leafOffsets[TANGENT]  = leafOffsets[NORMAL]    + vCnt * leafSizes[NORMAL]	* sizeof(GLfloat);
	leafOffsets[TEXCOORD0]= leafOffsets[TANGENT]   + vCnt * leafSizes[TANGENT]	* sizeof(GLfloat);
	leafOffsets[XVALS]	  =	leafOffsets[TEXCOORD0] + vCnt * leafSizes[TEXCOORD0]* sizeof(GLfloat);
	leafOffsets[BRANCH_INDEX]= leafOffsets[XVALS]  + vCnt * leafSizes[XVALS]	* sizeof(GLfloat);
	
	// fill arrays
	int k,l, indicesCnt, vertexCnt, offset, ch;
	int indexPtr = 0, dataPtr=0, v=0, id=0;
	int indexOffset = 0;
	Vertex *vertex;
	for (i=0; i<sizeL; i++)// each leaf 
	{
		leaf = leaves[i];
		id = leaf->parentID;
		for (k=0; k<leaf->vertices.size(); k++){
			vertex = leaf->vertices[k];
			// position
			vbo_data[POSITION][dataPtr*leafSizes[POSITION] + 0] = vertex->bPos.data[ 0 ];
			vbo_data[POSITION][dataPtr*leafSizes[POSITION] + 1] = vertex->bPos.data[ 1 ];
			vbo_data[POSITION][dataPtr*leafSizes[POSITION] + 2] = vertex->bPos.data[ 2 ];

			// normal
			vbo_data[NORMAL][dataPtr*leafSizes[NORMAL] + 0] = vertex->normal.data[ 0 ];
			vbo_data[NORMAL][dataPtr*leafSizes[NORMAL] + 1] = vertex->normal.data[ 1 ];
			vbo_data[NORMAL][dataPtr*leafSizes[NORMAL] + 2] = vertex->normal.data[ 2 ];

			// tangent
			vbo_data[TANGENT][dataPtr*leafSizes[TANGENT] + 0] = vertex->tangent.data[ 0 ];
			vbo_data[TANGENT][dataPtr*leafSizes[TANGENT] + 1] = vertex->tangent.data[ 1 ];
			vbo_data[TANGENT][dataPtr*leafSizes[TANGENT] + 2] = vertex->tangent.data[ 2 ];

			// texcoord0
			vbo_data[TEXCOORD0][dataPtr*leafSizes[TEXCOORD0] + 0] = vertex->textureCoords.data[ 0 ];
			vbo_data[TEXCOORD0][dataPtr*leafSizes[TEXCOORD0] + 1] = vertex->textureCoords.data[ 1 ];

			// xvals
			vbo_data[XVALS][dataPtr*leafSizes[XVALS] + 0] = vertex->x[ 0 ];
			vbo_data[XVALS][dataPtr*leafSizes[XVALS] + 1] = vertex->x[ 1 ];
			vbo_data[XVALS][dataPtr*leafSizes[XVALS] + 2] = vertex->x[ 2 ];
			vbo_data[XVALS][dataPtr*leafSizes[XVALS] + 3] = vertex->x[ 3 ];

			// branch index
			vbo_data[BRANCH_INDEX][dataPtr + 0] = id+0.5f;

			dataPtr++;
		}
	}
	// create vbos...
	glGenBuffers(1, &leafVBOid);
	glBindBuffer(GL_ARRAY_BUFFER, leafVBOid);
		// empty init
		int vboSize = totalVertexSize * vCnt * sizeof(GLfloat);
		glBufferData(GL_ARRAY_BUFFER, vboSize, 0, GL_STATIC_DRAW);
		
		// fill by parts
		// position
		glBufferSubData(GL_ARRAY_BUFFER,
						leafOffsets   [POSITION],
						vCnt*leafSizes[POSITION]*sizeof(GLfloat),
						vbo_data  [POSITION]
						);
		// normal
		glBufferSubData(GL_ARRAY_BUFFER,
						leafOffsets   [NORMAL],
						vCnt*leafSizes[NORMAL]*sizeof(GLfloat),
						vbo_data  [NORMAL]
						);
		// tangent
		glBufferSubData(GL_ARRAY_BUFFER,
						leafOffsets   [TANGENT],
						vCnt*leafSizes[TANGENT]*sizeof(GLfloat),
						vbo_data  [TANGENT]
						);
		// xvals
		glBufferSubData(GL_ARRAY_BUFFER,
						leafOffsets   [XVALS],
						vCnt*leafSizes[XVALS]*sizeof(GLfloat),
						vbo_data  [XVALS]
						);
		// branch index
		glBufferSubData(GL_ARRAY_BUFFER,
						leafOffsets   [BRANCH_INDEX],
						vCnt*leafSizes[BRANCH_INDEX]*sizeof(GLfloat),
						vbo_data  [BRANCH_INDEX]
						);
		// texcoord0
		glBufferSubData(GL_ARRAY_BUFFER,
						leafOffsets   [TEXCOORD0],
						vCnt*leafSizes[TEXCOORD0]*sizeof(GLfloat),
						vbo_data  [TEXCOORD0]
						);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (i = 0; i < VBO_VERTEX_COMPONENTS; i++){
		delete [] vbo_data[i];
		vbo_data[i] = NULL;
	}
	printf("vCnt = %i\n",vCnt);
}

void Tree::createBranchesVBO(){
	int i, sizeB = branches.size(), size;
	int vCnt = 0, iCnt = 0;
	// count total vertices & indices
	for (i=0; i<sizeB; i++){
		vCnt += branches[i]->getVertexCount();
		iCnt += branches[i]->getIndicesCount();
	}

	// get sizes
	for (i = 0; i < VBO_VERTEX_COMPONENTS; i++){
		sizes[i]=0;
	}
	sizes[POSITION] = 3;
	sizes[NORMAL]	= 3;
	sizes[TANGENT]	= 3;
	sizes[TEXCOORD0]= 2;
	//sizes[TEXCOORD1]= 2;
	sizes[BRANCH_INDEX]=1;
	sizes[XVALS]	= 4;
	int totalVertexSize = 0;
	for (i = 0; i < VBO_VERTEX_COMPONENTS; i++){
		vbo_data[i]= new GLfloat[vCnt*sizes[i]];
		totalVertexSize+=sizes[i];
	}
	// ebo
	ebo_data = new GLuint	[iCnt];

	// get offsets
	offsets[POSITION] = 0;
	offsets[NORMAL]   = offsets[POSITION]  + vCnt * sizes[POSITION]	* sizeof(GLfloat);
	offsets[TANGENT]  = offsets[NORMAL]    + vCnt * sizes[NORMAL]	* sizeof(GLfloat);
	offsets[TEXCOORD0]= offsets[TANGENT]   + vCnt * sizes[TANGENT]	* sizeof(GLfloat);
	offsets[XVALS]	  =	offsets[TEXCOORD0] + vCnt * sizes[TEXCOORD0]* sizeof(GLfloat);
	offsets[BRANCH_INDEX]= offsets[XVALS]  + vCnt * sizes[XVALS]	* sizeof(GLfloat);
	
	// fill arrays
	int k,l, indicesCnt, vertexCnt, offset, ch;
	int indexPtr = 0, dataPtr=0, v=0, id=0;
	int indexOffset = 0;
	Vertex *vertex;
	for (i=0; i<sizeB; i++)// each branch 
	{
		id = branches[i]->id;
		offset = dataPtr;
		for (k=0; k<branches[i]->vertices.size(); k++){
			vertex = branches[i]->vertices[k];
			// position
			vbo_data[POSITION][dataPtr*sizes[POSITION] + 0] = vertex->bPos.data[ 0 ];
			vbo_data[POSITION][dataPtr*sizes[POSITION] + 1] = vertex->bPos.data[ 1 ];
			vbo_data[POSITION][dataPtr*sizes[POSITION] + 2] = vertex->bPos.data[ 2 ];

			// normal
			vbo_data[NORMAL][dataPtr*sizes[NORMAL] + 0] = vertex->normal.data[ 0 ];
			vbo_data[NORMAL][dataPtr*sizes[NORMAL] + 1] = vertex->normal.data[ 1 ];
			vbo_data[NORMAL][dataPtr*sizes[NORMAL] + 2] = vertex->normal.data[ 2 ];

			// tangent
			vbo_data[TANGENT][dataPtr*sizes[TANGENT] + 0] = vertex->tangent.data[ 0 ];
			vbo_data[TANGENT][dataPtr*sizes[TANGENT] + 1] = vertex->tangent.data[ 1 ];
			vbo_data[TANGENT][dataPtr*sizes[TANGENT] + 2] = vertex->tangent.data[ 2 ];

			// texcoord0
			vbo_data[TEXCOORD0][dataPtr*sizes[TEXCOORD0] + 0] = vertex->textureCoords.data[ 0 ];
			vbo_data[TEXCOORD0][dataPtr*sizes[TEXCOORD0] + 1] = vertex->textureCoords.data[ 1 ];

			// xvals
			vbo_data[XVALS][dataPtr*sizes[XVALS] + 0] = vertex->x[ 0 ];
			vbo_data[XVALS][dataPtr*sizes[XVALS] + 1] = vertex->x[ 1 ];
			vbo_data[XVALS][dataPtr*sizes[XVALS] + 2] = vertex->x[ 2 ];
			vbo_data[XVALS][dataPtr*sizes[XVALS] + 3] = vertex->x[ 3 ];

			// branch index
			vbo_data[BRANCH_INDEX][dataPtr + 0] = id+0.5f;
			branches[i]->motionVectors[0];
			//printf("branch_index[%i] = %f\n", dataPtr, id+0.5f);

			dataPtr++;
		}
		// copy branch indices...
		int * branchIndices = branches[i]->indexPtr;
		int branchIndexCnt = branches[i]->indicesCount;
		
		for (k=0; k<branchIndexCnt; k++){
			ebo_data[indexPtr] = branchIndices[k]+offset; 
			indexPtr++;
		}
	}
	// create vbos...
	glGenBuffers(1, &branchEBOid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, branchEBOid); // vytvoreni indexoveho bufferu
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, iCnt*sizeof(GLuint), ebo_data, GL_STATIC_DRAW);
	 branchEBOcount = iCnt;	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &branchVBOid);
	glBindBuffer(GL_ARRAY_BUFFER, branchVBOid);
		// empty init
		int vboSize = totalVertexSize * vCnt * sizeof(GLfloat);
		glBufferData(GL_ARRAY_BUFFER, vboSize, 0, GL_STATIC_DRAW);
		
		// fill by parts
		// position
		glBufferSubData(GL_ARRAY_BUFFER,
						offsets   [POSITION],
						vCnt*sizes[POSITION]*sizeof(GLfloat),
						vbo_data  [POSITION]
						);
		// normal
		glBufferSubData(GL_ARRAY_BUFFER,
						offsets   [NORMAL],
						vCnt*sizes[NORMAL]*sizeof(GLfloat),
						vbo_data  [NORMAL]
						);
		// tangent
		glBufferSubData(GL_ARRAY_BUFFER,
						offsets   [TANGENT],
						vCnt*sizes[TANGENT]*sizeof(GLfloat),
						vbo_data  [TANGENT]
						);
		// xvals
		glBufferSubData(GL_ARRAY_BUFFER,
						offsets   [XVALS],
						vCnt*sizes[XVALS]*sizeof(GLfloat),
						vbo_data  [XVALS]
						);
		// branch index
		glBufferSubData(GL_ARRAY_BUFFER,
						offsets   [BRANCH_INDEX],
						vCnt*sizes[BRANCH_INDEX]*sizeof(GLfloat),
						vbo_data  [BRANCH_INDEX]
						);
		// texcoord0
		glBufferSubData(GL_ARRAY_BUFFER,
						offsets   [TEXCOORD0],
						vCnt*sizes[TEXCOORD0]*sizeof(GLfloat),
						vbo_data  [TEXCOORD0]
						);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	for (i = 0; i < VBO_VERTEX_COMPONENTS; i++){
		delete [] vbo_data[i];
		vbo_data[i] = NULL;
	}
	delete [] ebo_data;
	ebo_data = NULL;
}

bool Tree::makeShader(const char* vsfilename,const char* gsfilename,const char* fsfilename, GLuint &programID, GLuint &vertexShader, GLuint &geometryShader,  GLuint &fragmentShader){

	bool bResult = createShaderProgram(	programID,
										&vertexShader,
										&geometryShader,
										&fragmentShader,
										vsfilename,
										gsfilename,
										fsfilename,
										3,
										GL_TRIANGLES,
										GL_LINE_STRIP);
										// 6 = kolik muze gs vygenerovat primitiv
										// GL_TRIANGLES ... jaka primitiva prijma
										// GL_TRIANGLE_STRIP ... jaka primitiva produkuje
	return bResult;
}

void Tree::recalcCoordSystems()
{
	int i, size=branches.size();
	TreeBranch * branch;
	for (i=0; i<size; i++){
		branch = branches[i];
		if (branch->parent!=NULL){
			branch->cs = branch->parent->originalCS.getSystemInThisSystem(branch->originalCS);
		}
	}
}

void Tree::fillParentDataForEachBranch()
{

	// and copy from parent...
	TreeBranch* actBranch, *branch;
	CoordSystem *cSys;
	int i, size=branches.size();
	for (i=0; i<size; i++){
		branch = branches[i];
		actBranch = branch;
		while (actBranch!=NULL){
			cSys = &(actBranch->cs);
			branch->xvals.d		[actBranch->level] = actBranch->x;
			branch->lengths.d	[actBranch->level] = actBranch->L;
			//branch->phases.d	[actBranch->level] = actBranch->phase;
			branch->motionVectors[actBranch->level]= actBranch->motionVector;
			//printf("<- branch id=%i (level = %i)", actBranch->id, actBranch->level);
			if (actBranch->parent!=NULL){
				branch->origins		[actBranch->level] = cSys->origin;
				branch->upVectors	[actBranch->level] = cSys->r;
				branch->rightVectors[actBranch->level] = cSys->s;
				branch->tVectors    [actBranch->level] = cSys->t;
				actBranch = (TreeBranch*)(actBranch->parent);
				
			} else {
				branch->upVectors	[actBranch->level] = cSys->r;
				branch->rightVectors[actBranch->level] = cSys->s;
				branch->tVectors    [actBranch->level] = cSys->t;
				actBranch = NULL;
			}
		}
		//printf("\n");
	}
}

void Tree::createDataTexture()
{
	// linearize structure
	texDimX = linearizeHierarchy();
	branch_count = texDimX;
	texDimY = 18; // num of data rows  [4 floats]

	recalcCoordSystems();
	fillParentDataForEachBranch();

	TreeBranch * b;
	int channels = 4, ch, bh, z;
	float *data = new float[texDimX*texDimY*channels];
	int k,l;
	for (int i=0; i<texDimX*texDimY*channels; i=i+texDimY*channels){
		k=0;
		b = branches[i/(texDimY*channels)];
		//printf("id: %i\n", b->id);
		for (ch = 0; ch<channels; ch++){
			data[i + k*channels + ch] = 0.7f;
		}
		// motion vectors
		k = 0;
		data[i + k*channels + 0] = b->motionVectors[0].x;
		data[i + k*channels + 1] = b->motionVectors[0].y;
		data[i + k*channels + 2] = b->motionVectors[1].x;
		data[i + k*channels + 3] = b->motionVectors[1].y;
		k = 1;
		data[i + k*channels + 0] = b->motionVectors[2].x;
		data[i + k*channels + 1] = b->motionVectors[2].y;
		data[i + k*channels + 2] = b->motionVectors[3].x;
		data[i + k*channels + 3] = b->motionVectors[3].y;

		// up0_l0 - up2_l2
		k = 2;
		for (bh = 0;bh < MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->upVectors[bh].x;
			data[i + k*channels + 1] = b->upVectors[bh].y;
			data[i + k*channels + 2] = b->upVectors[bh].z;
			data[i + k*channels + 3] = b->lengths[bh];
			//printf("length: %f\n", b->lengths[bh]);
			k++;
		}
		// right0_x0 - right2_x2
		for (bh = 0;bh < MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->rightVectors[bh].x;
			data[i + k*channels + 1] = b->rightVectors[bh].y;
			data[i + k*channels + 2] = b->rightVectors[bh].z;
			data[i + k*channels + 3] = b->xvals[bh];
			k++;
		}
		// t0_x0 - t2_x2
		for (bh = 0;bh < MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->tVectors[bh].x;
			data[i + k*channels + 1] = b->tVectors[bh].y;
			data[i + k*channels + 2] = b->tVectors[bh].z;
			data[i + k*channels + 3] = 0.0f;
			k++;
		}
		// center1-center3
		for (bh = 0;bh < MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->origins[bh].x;
			data[i + k*channels + 1] = b->origins[bh].y;
			data[i + k*channels + 2] = b->origins[bh].z;
			data[i + k*channels + 3] = 0.0f;
			k++;
		}
	}
	dataTexture = new Texture(  
								GL_TEXTURE_2D,
								GL_RGBA32F,
								GL_RGBA,
								GL_FLOAT,
								data,
								texDimY, 
								texDimX
							  );
}

int Tree::linearizeHierarchy(){

	stack<TreeBranch*> bStack;	
	bStack.push(trunk);
	TreeBranch * branch, *actBranch;
	int cnt = 0, i;
	while( !bStack.empty() ){
		branch = bStack.top();
		branch->id = cnt;
		cnt++;
		bStack.pop();
		branches.push_back(branch);
		for (i=0; i<branch->children.size(); i++){
			if (! branch->children[i]->isLeaf()){
				actBranch = (TreeBranch*)(branch->children[i]);
				bStack.push(actBranch);
			} else {
				//leaves.push_back((TreeLeaf*) branch->children[i] );
			}
		}		
	}
	return cnt;
};

void Tree::draw(){
	glDisable(GL_LIGHTING);
	glPushMatrix();

	glScalef( 1.f , -1.f, 1.f);
	glScalef(2.0, 2.0, 2.0);

	dataTexture->bindTexture(1);
	branchNoiseTexture->bindTexture(2);
	bColorTexture->bindTexture(4);
	// ========================================================================================
	// draw BRANCHES
	// ========================================================================================
	glUseProgram(branchShaderID);
		// set time
		glUniform1f(locations[TIME], time);
		// set data_texture
		glUniform1i(locations[BRANCH_DATA_TEXTURE],		GLint(dataTexture->unitOffset));
		glUniform1i(locations[NOISE_TEXTURE0],			GLint(branchNoiseTexture->unitOffset));
		glUniform1i(locations[COLOR_TEXTURE0],			GLint(bColorTexture->unitOffset));
		glUniform1f(locations[BRANCH_COUNT],			GLfloat(branch_count));
		glUniform3f(locations[WIND_DIRECTION],			g_WindDirection.x, g_WindDirection.y, g_WindDirection.z);
		glUniform1f(locations[WIND_STRENGTH],			g_WindStrength);
		glUniform4f(locations[WOOD_AMPLITUDE],			g_WoodAmplitude.x, g_WoodAmplitude.y, g_WoodAmplitude.z, g_WoodAmplitude.w);
		glUniform4f(locations[WOOD_FREQUENCY],			g_WoodFrequency.x, g_WoodFrequency.y, g_WoodFrequency.z, g_WoodFrequency.w);
		// bind index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, branchEBOid);
		glBindBuffer(GL_ARRAY_BUFFER, branchVBOid); 
		   // enable states
		   glEnableClientState(GL_VERTEX_ARRAY);
		   //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		   glEnableVertexAttribArray(GLuint(locations[BRANCH_INDEX]	));
		   glEnableVertexAttribArray(GLuint(locations[NORMAL]		));
		   glEnableVertexAttribArray(GLuint(locations[TANGENT]		));
		   glEnableVertexAttribArray(GLuint(locations[XVALS]		));
		   glEnableVertexAttribArray(GLuint(locations[TEXCOORD0]	));

		   // draw VBOs...
			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(offsets[POSITION]));
			
			glVertexAttribPointer(locations[BRANCH_INDEX], sizes[BRANCH_INDEX], GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[BRANCH_INDEX]));
			
			glVertexAttribPointer(locations[XVALS], sizes[XVALS], GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[XVALS]));
			
			glVertexAttribPointer(locations[NORMAL], sizes[NORMAL], GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[NORMAL]));
			
			glVertexAttribPointer(locations[TANGENT], sizes[TANGENT], GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[TANGENT]));
			
			glVertexAttribPointer(locations[TEXCOORD0], sizes[TEXCOORD0], GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[TEXCOORD0]));
			//glClientActiveTexture(dataTexture->unitId);
			//glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(offsets[TEXCOORD0]));
			
			glDrawElements(GL_TRIANGLES, branchEBOcount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
		   // disable
		   glDisableVertexAttribArray(GLuint(locations[TEXCOORD0]));
		   glDisableVertexAttribArray(locations[BRANCH_INDEX]);
		   glDisableVertexAttribArray(locations[NORMAL]);
		   glDisableVertexAttribArray(locations[TANGENT]);
		   glDisableVertexAttribArray(locations[XVALS]);
		   //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		   glDisableClientState(GL_VERTEX_ARRAY);
		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// turn off shader
	glUseProgram(0);
	
	// ========================================================================================
	// draw LEAVES
	// ========================================================================================
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	lColorTexture->bindTexture(4);
	leafNoiseTexture->bindTexture(3);
	glUseProgram(leafShaderID);
		// set time
		glUniform1f(leafLocations[TIME], time);
		// set data_texture
		glUniform1i(leafLocations[BRANCH_DATA_TEXTURE],	GLint(dataTexture->unitOffset)			);
		glUniform1i(leafLocations[NOISE_TEXTURE0],		GLint(branchNoiseTexture->unitOffset)	);
		glUniform1i(leafLocations[NOISE_TEXTURE1],		GLint(leafNoiseTexture->unitOffset)	    );
		glUniform1f(leafLocations[BRANCH_COUNT],		GLfloat(branch_count)					);
		glUniform1i(leafLocations[COLOR_TEXTURE0],		GLint(lColorTexture->unitOffset)		);
		glUniform3f(leafLocations[WIND_DIRECTION],		g_WindDirection.x, g_WindDirection.y, g_WindDirection.z);
		glUniform1f(leafLocations[WIND_STRENGTH],		GLfloat(g_WindStrength));
		glUniform4f(leafLocations[WOOD_AMPLITUDE],		g_WoodAmplitude.x, g_WoodAmplitude.y, g_WoodAmplitude.z, g_WoodAmplitude.w);
		glUniform4f(leafLocations[WOOD_FREQUENCY],		g_WoodFrequency.x, g_WoodFrequency.y, g_WoodFrequency.z, g_WoodFrequency.w);
		glUniform1f(leafLocations[LEAF_AMPLITUDE],		g_LeafAmplitude);
		glUniform1f(leafLocations[LEAF_FREQUENCY],		g_LeafFrequency);
		// bind index buffer
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, branchEBOid);
		glBindBuffer(GL_ARRAY_BUFFER, leafVBOid); 
		   // enable states
		   glEnableClientState(GL_VERTEX_ARRAY);
		   //glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		   glEnableVertexAttribArray(GLuint(leafLocations[BRANCH_INDEX]	));
		   glEnableVertexAttribArray(GLuint(leafLocations[NORMAL]		));
		   glEnableVertexAttribArray(GLuint(leafLocations[TANGENT]		));
		   glEnableVertexAttribArray(GLuint(leafLocations[XVALS]		));
		   glEnableVertexAttribArray(GLuint(leafLocations[TEXCOORD0]	));
		   // draw VBOs...

			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(leafOffsets[POSITION]));
			glVertexAttribPointer(leafLocations[TEXCOORD0],		leafSizes[TEXCOORD0],		GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(leafOffsets[TEXCOORD0]));
			glVertexAttribPointer(leafLocations[BRANCH_INDEX],	leafSizes[BRANCH_INDEX],	GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(leafOffsets[BRANCH_INDEX]));					  
			glVertexAttribPointer(leafLocations[XVALS],			leafSizes[XVALS],			GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(leafOffsets[XVALS]));					  
			glVertexAttribPointer(leafLocations[NORMAL],		leafSizes[NORMAL],			GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(leafOffsets[NORMAL]));			  
			glVertexAttribPointer(leafLocations[TANGENT],		leafSizes[TANGENT],			GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(leafOffsets[TANGENT]));
			
			glDrawArrays(GL_QUADS, 0, leafVerticesCount);
			//glDrawElements(GL_TRIANGLES, branchEBOcount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
			//printf(".");
		   // disable
		   glDisableVertexAttribArray(locations[BRANCH_INDEX]);
		   glDisableVertexAttribArray(locations[NORMAL]);
		   glDisableVertexAttribArray(locations[TANGENT]);
		   glDisableVertexAttribArray(locations[XVALS]);
		    glDisableVertexAttribArray(locations[TEXCOORD0]);
		   //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		   glDisableClientState(GL_VERTEX_ARRAY);
		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	// turn off shader
	glUseProgram(0);

	// unbind leaf noise texture
	leafNoiseTexture->unbindTexture();
	//*/
	// unbind data texture
	dataTexture->unbindTexture();

	// unbind branch noise texture
	branchNoiseTexture->unbindTexture();

	// unbind branch color texture
	bColorTexture->unbindTexture();

	// unbind leaf color texture
	lColorTexture->unbindTexture();
	glDisable (GL_BLEND);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	
}


void Tree::setTime(float _time){
	time = _time;
}


void Tree::load(string filename, TextureManager *texManager){
	OBJTfile file;
	file.loadFromFile(filename.c_str());
	// process branches
	map <int, TreeBranch*> branches;
	map <int, StEntity>::iterator iter;
	TreeBranch *parent = NULL;
	CoordSystem cs;
	CoordSystem objectCS;
	float x;
	float length, r1, r2, c2=0.3326, c4=0.398924;
	int divT=2, divR=4;
	v3 motionVector(1.0, 1.0, 1.0);
	StEntity entity;
	// get trunk (id = parentId)
	int id=0;
	for ( iter=file.branches.begin() ; iter != file.branches.end(); iter++ ){
		parent	= NULL;
		id		= (*iter).first;
		entity	= (*iter).second;
		// parent
		if (entity.isSetParent && entity.id!=entity.parentId){
			parent = branches[entity.parentId];
		} else {
			parent = NULL;
		}
		// length
		if (entity.isSetLength){
			length = entity.length / file.maxLength;
		} else {
			if (entity.isSetLevel){
				length = 1.f/(entity.level*3.f+1.f);
			} else {
				length = 1.0;
			}
		}
		// x
		if (entity.isSetX){
			x = entity.x;
		} else {
			x = 0.5;
		}
		// base vectors
		if (entity.isSetBase){
			cs.r = entity.base[0];
			cs.s = entity.base[1];	
			cs.t = entity.base[2];
			objectCS.r = entity.base[0];
			objectCS.s = entity.base[1];
			objectCS.t = entity.base[2];
			cs.normalize();
			cs.repair();

		} else {
			cs.r = v3(1.0, 0.0, 0.0);
			cs.s = v3(0.0, 1.0, 0.0);	
			cs.t = v3(0.0, 0.0, 1.0);
			objectCS.r = v3(1.0, 0.0, 0.0);
			objectCS.s = v3(0.0, 1.0, 0.0);
			objectCS.t = v3(0.0, 0.0, 1.0);
		}
		
		// origin
		if (entity.isSetOrigin){
			cs.origin = entity.origin;
			// recalculate origin
			if (parent!=NULL){
				// TODO!!!! get position of origin{branch space} from origin{obj space} 
			}
		} else {
			if (parent!=NULL){
				cs.origin = v3(0.0, 0.0, 1.0) * x * parent->L;
			} else {
				cs.origin = v3(0.f, 0.f, 0.f);
			}
		}
		// r1
		if (entity.isSetR1){
			r1 = entity.r1;
		} else {		
			r1 = 0.02*length;
		}
		// r2
		if (entity.isSetR2){
			r2 = entity.r2;
		} else {		
			r2 = 0.0001;
		}
		// add branch...	
		// set motion vector
			float angle = randomf(0.f, 2*M_PI );
			float scale = 1.0;
			motionVector = v3(scale*cos(angle), scale*sin(angle), 0.f);
		
		branches[id] = new TreeBranch(parent, cs, x, texManager, length, r1, r2, divT, divR, c2, c4, motionVector);  
		branches[id]->id = id;
	} // for each entity in map
	
	// set trunk
	trunk = branches[0];
	int leafCnt = 0;

	bool coordsOK = true;
	// process leaves
	for ( iter=file.leaves.begin() ; iter != file.leaves.end(); iter++ ){
		id		= (*iter).first;
		entity	= (*iter).second;
		leafCnt++;
		// parent
		if (entity.isSetParent){
			//printf("%i \n", entity.parentId);
			parent = branches[entity.parentId];
		} else {
			parent = NULL;
		}

		// leaf X position
		if (entity.isSetX){
			x = entity.x;
		} else {
			x = 0.5f;
		}

		// leaf size
		float size;
		if (entity.isSetLength){
			size = entity.length;
			printf("size set\n");
		} else {
			size = LEAF_SIZE;
		}

		// base vectors
		if (entity.isSetBase){
			//cs = CoordSystem(entity.base[0]);
			cs.r = entity.base[0];
			cs.s = entity.base[1];	
			cs.t = entity.base[2];
			cs.normalize();
			cs.repair();
			objectCS.r = entity.base[0];
			objectCS.s = entity.base[1];
			objectCS.t = entity.base[2];

		} else {
			cs.r = v3(1.0, 0.0, 0.0);
			cs.s = v3(0.0, 1.0, 0.0);	
			cs.t = v3(0.0, 0.0, 1.0);
			objectCS.r = v3(1.0, 0.0, 0.0);
			objectCS.s = v3(0.0, 1.0, 0.0);
			objectCS.t = v3(0.0, 0.0, 1.0);
		}

		// create leaf
		TreeLeaf * leaf = new TreeLeaf(parent, cs, x, texManager, size, motionVector);
		// add to container
		leaves.push_back(leaf);
	}
	printf("COORDS %s \n", coordsOK?"OK":"ERROR");
	printf("leaves count: %i\n", leafCnt);
}

void Tree::save(string filename){}
