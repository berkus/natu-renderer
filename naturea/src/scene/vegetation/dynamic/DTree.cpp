#include "DTree.h"


DTree::DTree(TextureManager *texManager, ShaderManager *shManager):Vegetation(texManager, shManager)
{
	branchShader			= NULL;
	leafShader				= NULL;
	branchShader_sh			= NULL;
	leafShader_sh			= NULL;

	bLODShader				= NULL;
	lLODShader				= NULL;
							
	dataTexture				= NULL;
	lod1_dataTexture		= NULL;
	lod2_dataTexture		= NULL;
	lColorTexture			= NULL;
	frontDecalMap			= NULL;
	frontNormalMap			= NULL;
	frontTranslucencyMap	= NULL;
	frontHalfLife2Map		= NULL;
	backDecalMap			= NULL;
	backNormalMap			= NULL;
	backTranslucencyMap		= NULL;
	backHalfLife2Map		= NULL;
							
	bColorTexture			= NULL;
	bNormalTexture			= NULL;
							
	branchNoiseTexture		= NULL;
	leafNoiseTexture		= NULL;

	branchesVBO				= NULL;
	leavesVBO				= NULL;
	branchesEBO				= NULL;

	lod1shader				= NULL;
	lod1_shader				= NULL;

	lod1_jColorMap			= NULL;
	lod1_jDataMap			= NULL;
	lod1_jDepthMap			= NULL;
	lod1_jNormalMap			= NULL;

	lod2_jColorMap			= NULL;
	lod2_jDataMap			= NULL;
	lod2_jDepthMap			= NULL;
	lod2_jNormalMap			= NULL;


	lod2color1				= NULL;
	lod2color2				= NULL;
	lod2normal1				= NULL;
	lod2normal2				= NULL;
	lod2branch1				= NULL;
	lod2branch2				= NULL;
	lod2depth1				= NULL;
	lod2depth2				= NULL;
	ctr	=0;

	i_matricesBuffID		= 0;
	instancesLocked			= false;
}					
DTree::DTree(DTree* copy):
Vegetation(copy->textureManager, copy->shaderManager)
{
	//	textureId	= copy->textureId;
	//	vboId		= copy->vboId;
	//	shader		= copy->shader;
	//	shaderId	= copy->shaderId;
	//	LCmatrixLoc = copy->LCmatrixLoc;
	//	fastModeLoc	= copy->fastModeLoc;
	//	shadowMappingEnabledLoc = copy->shadowMappingEnabledLoc;
	ctr	= 0 ;
}

DTree::~DTree(void)
{
	SAFE_DELETE_PTR(	branchShader			);
	SAFE_DELETE_PTR(	leafShader				);
	SAFE_DELETE_PTR(	bLODShader				);
	SAFE_DELETE_PTR(	lLODShader				);

	SAFE_DELETE_PTR(	dataTexture				);
	SAFE_DELETE_PTR(	lod1_dataTexture			);
	SAFE_DELETE_PTR(	lod2_dataTexture			);
	SAFE_DELETE_PTR(	lColorTexture			);
	SAFE_DELETE_PTR(	frontDecalMap			);
	SAFE_DELETE_PTR(	frontNormalMap			);
	SAFE_DELETE_PTR(	frontTranslucencyMap	);
	SAFE_DELETE_PTR(	frontHalfLife2Map		);
	SAFE_DELETE_PTR(	backDecalMap			);
	SAFE_DELETE_PTR(	backNormalMap			);
	SAFE_DELETE_PTR(	backTranslucencyMap		);
	SAFE_DELETE_PTR(	backHalfLife2Map		);

	SAFE_DELETE_PTR(	bColorTexture			);
	SAFE_DELETE_PTR(	bNormalTexture			);

	SAFE_DELETE_PTR(	branchNoiseTexture		);
	SAFE_DELETE_PTR(	leafNoiseTexture		);

	SAFE_DELETE_PTR(	lod1_jColorMap			);
	SAFE_DELETE_PTR(	lod1_jDataMap			);
	SAFE_DELETE_PTR(	lod1_jDepthMap			);
	SAFE_DELETE_PTR(	lod1_jNormalMap			);

	SAFE_DELETE_PTR(	lod2_jColorMap			);
	SAFE_DELETE_PTR(	lod2_jDataMap			);
	SAFE_DELETE_PTR(	lod2_jDepthMap			);
	SAFE_DELETE_PTR(	lod2_jNormalMap			);


	SAFE_DELETE_PTR(	branchesVBO				);
	SAFE_DELETE_PTR(	leavesVBO				);
	SAFE_DELETE_PTR(	branchesEBO				);

	int i = 0;
	for (i = 0; i< slices.size(); i++){
		SAFE_DELETE_PTR( slices[i] );
	}
	slices.clear();
	for (i = 0; i< branches.size(); i++){
		SAFE_DELETE_PTR( branches[i] );
	}
	branches.clear();
	for (i = 0; i< leaves.size(); i++){
		SAFE_DELETE_PTR( leaves[i] );
	}
	leaves.clear();
	 
	// instance matrices...
	for (i=0; i< lod1_instanceMatrices.size(); i++){
		SAFE_DELETE_PTR( lod1_instanceMatrices[i] );
	}
	lod1_instanceMatrices.clear();
	// render queues
	for (i=0; i< instancesInRenderQueues.size(); i++){
		SAFE_DELETE_ARRAY_PTR( instancesInRenderQueues[i] );
	}
	instancesInRenderQueues.clear();

	SAFE_DELETE_PTR(	lod1shader		);
	SAFE_DELETE_PTR(	lod1_shader		);


	SAFE_DELETE_PTR(	lod2color1		);	
	SAFE_DELETE_PTR(	lod2color2		);
	SAFE_DELETE_PTR(	lod2normal1		);
	SAFE_DELETE_PTR(	lod2normal2		);
	SAFE_DELETE_PTR(	lod2branch1		);
	SAFE_DELETE_PTR(	lod2branch2		);
	SAFE_DELETE_PTR(	lod2depth1		);
	SAFE_DELETE_PTR(	lod2depth2		);

}

bool DTree::loadOBJT(string filename)
{
	printf(" DYNAMIC_TREE load %s\n", filename.c_str());
	OBJTfile file;
	file.loadFromFile(filename.c_str());

	// setup bbox 
	//TODO: get proper coords of bbox from OBJT / model itself
	bbox = getBBox();

	// process branches
	map <int, DTreeBranch*> m_branches;
	map <int, StEntity>::iterator iter;
	DTreeBranch *parent = NULL;
	CoordSystem cs;
	CoordSystem objectCS;
	float x;
	float length, r1, r2, c2=0.3326, c4=0.398924;
	int divT=2, divR=4;
	v3 motionVector(1.0, 1.0, 1.0);
	StEntity entity;
	// get trunk (id = parentId)
	int id=0;
	branchCount = 0;
	for ( iter=file.branches.begin() ; iter != file.branches.end(); iter++ ){
		parent	= NULL;
		id		= (*iter).first;
		entity	= (*iter).second;
		branchCount++;

		// parent
		if (entity.isSetParent && entity.id!=entity.parentId){
			parent = m_branches[entity.parentId];
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

				//printf("ORIG CS t: "); parent->originalCS.t.printOut();
				//printf("x: %f, L:%f\n", x , parent->L);
				//system("PAUSE");
				cs.origin = parent->originalCS.t * x * parent->L;


				//cs.origin = v3(0.0, 0.0, 1.0) * x * parent->L;
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

		//cs.printOut();
		//system("PAUSE");
		m_branches[id] = new DTreeBranch(parent, cs, x, length, r1, r2);  
		m_branches[id]->id = id;
	} // for each entity in map
	// set trunk
	trunk = m_branches[0];
	trunk->level = 0;
	int leafCnt = 0;
	// process leaves
	for ( iter=file.leaves.begin() ; iter != file.leaves.end(); iter++ ){
		id		= (*iter).first;
		entity	= (*iter).second;
		leafCnt++;
		// parent
		if (entity.isSetParent){
			parent = m_branches[entity.parentId];
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
		} else {
			size = DYN_TREE::LEAF_SIZE;
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

		// create leaf
		DTreeLeaf * leaf = new DTreeLeaf(parent, cs, x, size);
		// add to container
		leaves.push_back(leaf);
	}

	printf("DYNAMIC_TREE load successfull (branches: %d, leaves: %d)\n", branchCount, leafCnt);
	return true;
}

void DTree::createVBOs()
{
	// TODO:
	// create VBO for branch vertices
	createBranchesVBO();

	// create VBO for leaf vertices
	createLeavesVBO();
}

void DTree::createBranchesVBO()
{
	int i, sizeB = branches.size();
	int vCnt = 0, iCnt = 0;
	// count total vertices & indices
	for (i=0; i<sizeB; i++){
		vCnt += branches[i]->getVertexCount();
		iCnt += branches[i]->getIndicesCount();
	}
	int * sizes = new int[DYN_TREE::VERTEX_ATTRIBUTES::COUNT];
	// get sizes
	for (i = 0; i < DYN_TREE::VERTEX_ATTRIBUTES::COUNT; i++){
		sizes[i]=0;
	}
	sizes[DYN_TREE::VERTEX_ATTRIBUTES::POSITION]	= 3;
	sizes[DYN_TREE::VERTEX_ATTRIBUTES::NORMAL]		= 3;
	sizes[DYN_TREE::VERTEX_ATTRIBUTES::TANGENT]		= 3;
	sizes[DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0]	= 2;
	//sizes[DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD1]	= 2;
	sizes[DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX]= 1;
	sizes[DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT]		= 4;
	int totalVertexSize = 0;
	for (i = 0; i < DYN_TREE::VERTEX_ATTRIBUTES::COUNT; i++){
		vbo_data[i]= new GLfloat[vCnt*sizes[i]];
		totalVertexSize+=sizes[i];
	}
	// ebo
	ebo_data = new GLuint	[iCnt];

	// fill arrays
	int k, offset;
	int indexPtr = 0, dataPtr=0, v=0, id=0;
	int indexOffset = 0;
	Vertex *vertex;
	int index = 0;
	for (i=0; i<sizeB; i++)// each branch 
	{
		id = branches[i]->id;
		offset = dataPtr;
		for (k=0; k<branches[i]->vertices.size(); k++){
			vertex = branches[i]->vertices[k];
			// position
			index = DYN_TREE::VERTEX_ATTRIBUTES::POSITION;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->bPos.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->bPos.data[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->bPos.data[ 2 ];

			// normal
			index = DYN_TREE::VERTEX_ATTRIBUTES::NORMAL;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->normal.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->normal.data[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->normal.data[ 2 ];

			// tangent
			index = DYN_TREE::VERTEX_ATTRIBUTES::TANGENT;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->tangent.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->tangent.data[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->tangent.data[ 2 ];

			// texcoord0
			index = DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->textureCoords.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->textureCoords.data[ 1 ];

			// xvals
			index = DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->x[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->x[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->x[ 2 ];
			vbo_data[index][dataPtr*sizes[index] + 3] = vertex->x[ 3 ];

			// branch index
			index = DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX;
			vbo_data[index][dataPtr + 0] = id+0.5f;
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
	// create Buffers...

	// VBO
	branchesVBO = new VBO();
	branchesVBO->setVertexCount(vCnt);
	// position
	VBODataSet * dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::POSITION],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::POSITION]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::POSITION,
		true
		);
	branchesVBO->addVertexAttribute( dataSet );
	// normal
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::NORMAL],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::NORMAL]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::NORMAL,
		false
		);
	branchesVBO->addVertexAttribute( dataSet );

	// tangent
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::TANGENT],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::TANGENT]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TANGENT,
		false
		);
	branchesVBO->addVertexAttribute( dataSet );

	// texCoord0
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TEXCOORD0,
		false
		);
	branchesVBO->addVertexAttribute( dataSet );

	// weights
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::WEIGHT,
		false
		);	
	branchesVBO->addVertexAttribute( dataSet );

	// branch index
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::BRANCH_ID,
		false
		);	
	branchesVBO->addVertexAttribute( dataSet );

	branchesVBO->compileData(GL_STATIC_DRAW);

	// element buffer object
	branchesEBO = new EBO();
	branchesEBO->create(GL_UNSIGNED_INT, GL_TRIANGLES, iCnt, ebo_data, GL_STATIC_DRAW);
	branchesEBO->linkVBO(branchesVBO);

}

void DTree::createLeavesVBO()
{
	// recalc coord systems...
	int i, sizeL = leaves.size();

	DTreeLeaf * leaf;
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
	//leafVerticesCount = vCnt;
	// get sizes
	int * sizes = new int [DYN_TREE::COUNT];
	for (i = 0; i < DYN_TREE::COUNT; i++){
		sizes[i]=0;
	}
	sizes[DYN_TREE::POSITION	]	= 3;
	sizes[DYN_TREE::NORMAL		]	= 3;
	sizes[DYN_TREE::TANGENT		]	= 3;
	sizes[DYN_TREE::TEXCOORD0	]	= 2;
	sizes[DYN_TREE::BRANCH_INDEX]	= 1;
	sizes[DYN_TREE::WEIGHT		]	= 4;

	int totalVertexSize = 0;

	for (i = 0; i < DYN_TREE::COUNT; i++){
		vbo_data[i]= new GLfloat[vCnt*sizes[i]];
		totalVertexSize+=sizes[i];
	}

	// fill arrays
	int k;
	int indexPtr = 0, dataPtr=0, v=0, id=0;
	int indexOffset = 0;
	int index = 0;
	Vertex *vertex;
	for (i=0; i<sizeL; i++) // each leaf 
	{
		leaf = leaves[i];
		id = leaf->parentID;
		for (k=0; k<leaf->vertices.size(); k++){
			vertex = leaf->vertices[k];
			// position
			index = DYN_TREE::VERTEX_ATTRIBUTES::POSITION;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->bPos.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->bPos.data[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->bPos.data[ 2 ];

			// normal
			index = DYN_TREE::VERTEX_ATTRIBUTES::NORMAL;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->normal.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->normal.data[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->normal.data[ 2 ];

			// tangent
			index = DYN_TREE::VERTEX_ATTRIBUTES::TANGENT;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->tangent.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->tangent.data[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->tangent.data[ 2 ];

			// texcoord0
			index = DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->textureCoords.data[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->textureCoords.data[ 1 ];

			// xvals
			index = DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT;
			vbo_data[index][dataPtr*sizes[index] + 0] = vertex->x[ 0 ];
			vbo_data[index][dataPtr*sizes[index] + 1] = vertex->x[ 1 ];
			vbo_data[index][dataPtr*sizes[index] + 2] = vertex->x[ 2 ];
			vbo_data[index][dataPtr*sizes[index] + 3] = vertex->x[ 3 ];

			// branch index
			index = DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX;
			vbo_data[index][dataPtr + 0] = id+0.5f;

			dataPtr++;
		}
	}

	// create VBO
	leavesVBO = new VBO();
	leavesVBO->setVertexCount(vCnt);
	// position
	VBODataSet * dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::POSITION],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::POSITION]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::POSITION,
		true
		);
	leavesVBO->addVertexAttribute( dataSet );
	// normal
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::NORMAL],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::NORMAL]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::NORMAL,
		false
		);
	leavesVBO->addVertexAttribute( dataSet );

	// tangent
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::TANGENT],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::TANGENT]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TANGENT,
		false
		);
	leavesVBO->addVertexAttribute( dataSet );

	// texCoord0
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::TEXCOORD0]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TEXCOORD0,
		false
		);
	leavesVBO->addVertexAttribute( dataSet );

	// weights
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::WEIGHT]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::WEIGHT,
		false
		);	
	leavesVBO->addVertexAttribute( dataSet );

	// branch index
	dataSet = new VBODataSet(
		vbo_data[DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX],
		sizes[DYN_TREE::VERTEX_ATTRIBUTES::BRANCH_INDEX]*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::BRANCH_ID,
		false
		);	
	leavesVBO->addVertexAttribute( dataSet );

	leavesVBO->compileData(GL_STATIC_DRAW);

}

void DTree::createDataTexture()
{
	// linearize structure
	texDimX = linearizeHierarchy();
	//printf("cs:\n");
	//branches[0]->cs.printOut();
	//printf("orig cs:\n");
	//branches[0]->originalCS.printOut();
	//system("PAUSE");
	int branch_count = texDimX;
	this->branchCountF = branch_count;
	texDimY = 18; // num of data rows  [4 floats]

	recalcCoordSystems();

	fillParentDataForEachBranch();

	DTreeBranch * b;
	int channels = 4, ch, bh;
	float *data = new float[texDimX*texDimY*channels];
	int k;
	for (int i=0; i<texDimX*texDimY*channels; i=i+texDimY*channels){
		k=0;
		b = branches[i/(texDimY*channels)];
		for (ch = 0; ch<channels; ch++){
			data[i + k*channels + ch] = 0.0f;
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
		// s
		k = 2;
		for (bh = 0;bh < DYN_TREE::MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->rightVectors[bh].x;
			data[i + k*channels + 1] = b->rightVectors[bh].y;
			data[i + k*channels + 2] = b->rightVectors[bh].z;
			data[i + k*channels + 3] = b->lengths[bh];
			k++;
		}
		// right0_x0 - right2_x2
		// r
		for (bh = 0;bh < DYN_TREE::MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->upVectors[bh].x;
			data[i + k*channels + 1] = b->upVectors[bh].y;
			data[i + k*channels + 2] = b->upVectors[bh].z;
			data[i + k*channels + 3] = b->xvals[bh];
			k++;
		}
		// t0_x0 - t2_x2
		for (bh = 0;bh < DYN_TREE::MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->tVectors[bh].x;
			data[i + k*channels + 1] = b->tVectors[bh].y;
			data[i + k*channels + 2] = b->tVectors[bh].z;
			data[i + k*channels + 3] = 1.0f;
			k++;
		}
		// center1-center3
		for (bh = 0;bh < DYN_TREE::MAX_HIERARCHY_DEPTH; bh++){
			data[i + k*channels + 0] = b->parentIDs[0];
			data[i + k*channels + 1] = b->parentIDs[1];
			data[i + k*channels + 2] = b->parentIDs[2];
			data[i + k*channels + 3] = b->parentIDs[3];
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
		texDimX,
		DYN_TREE::DATA_TEXTURE_NAME
		);
	delete [] data;
	data = NULL;
}


Texture* DTree::createLODdataTexture(vector<Matrix4x4*> &MVPs)
{
	int branchCount = branches.size();
	int sliceCount = MVPs.size();
	int sliceItems = 3;
	int x = 0; // row = branch index
	int y = 0; // item index + slice index
	int channels = 4;
	int dimX = branchCount;
	int dimY = sliceItems*sliceCount;
	float *data = new float[dimX*dimY*channels];
	int sliceOffset = channels * sliceItems;
	int branchOffset = sliceOffset * sliceCount;
	// fill data buffer
	m4 * mvp;
	DTreeBranch * branch;
	int item = 0;

	v4 o0 = v4(0.0, 0.0, 0.0, 1.0);
	v4 t0 = v4(0.0, 1.0, 0.0, 1.0);
	v4 r0 = v4(1.0, 0.0, 0.0, 1.0);
	v4 s0 = v4(0.0, 0.0, 1.0, 1.0);

	// each sliceset
	for (int si=0; si<sliceCount; si++){
		// get MVP
		mvp = MVPs[si];
		//mvp->printOut();
		v4 origin = ((*mvp) * v4(0.0, 0.0, 0.0, 1.0)).ndcoord2texcoord();
		//printf("slice %i mvp: \n", si);
		//mvp->printOut();
		// for each branch
		for (int bi=0; bi<branchCount; bi++){
			branch = branches[bi];
			// express o, t,r,s in terms of texture coordinates:

			// project original t, original s, original r
			//
			v4 orig = v4( branch->originalCS.origin);
			v4 o = (*mvp) * ( orig );
			v4 t = (*mvp) * branch->originalCS.t;
			v4 r = (*mvp) * branch->originalCS.r;
			v4 s = (*mvp) * branch->originalCS.s;
			v4 ot = o.ndcoord2texcoord();
			v4 tt = t.ndcoord2texcoord() - origin;
			v4 rt =	r.ndcoord2texcoord() - origin;
			v4 st =	s.ndcoord2texcoord() - origin;

			//printf("INPUT: \n");
			//printf("t: ");
			//branch->originalCS.t.printOut();
			//printf("r: ");
			//branch->originalCS.r.printOut();
			//printf("s: ");
			//branch->originalCS.s.printOut();
			//printf("NORMALIZED DEVICE: \n");
			//printf("t: ");
			//t.printOut();
			//printf("r: ");
			//r.printOut();
			//printf("s: ");
			//s.printOut();
			//printf("TEXTURE: \n");
			//printf("tt: ");
			//tt.printOut();
			//printf("rt: ");
			//rt.printOut();
			//printf("st: ");
			//st.printOut();
			//system("PAUSE");

			// project LENGHT of the branch...
			v4 lenghtVector = tt * branch->L;
			lenghtVector.w = 0.0;
			lenghtVector.z = 0.0;
			float lenght = lenghtVector.lenght();

			// save data

			// origin + t
			item = 0;
			data[bi*branchOffset + si*sliceOffset + item*channels + 0] = ot.x ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 1] = ot.y ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 2] = branch->motionVectors[branch->level].x ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 3] = branch->motionVectors[branch->level].y ;

			// s+t vector
			item = 1;
			data[bi*branchOffset + si*sliceOffset + item*channels + 0] = rt.x ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 1] = rt.y ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 2] = rt.z ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 3] = lenght ;

			// length vector + motionVector
			item = 2;
			data[bi*branchOffset + si*sliceOffset + item*channels + 0] = st.x ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 1] = st.y ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 2] = st.z ; ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 3] = lenght ;

			// motion vectors
			//item = 3;
			//data[bi*branchOffset + si*sliceOffset + item*channels + 0] = ;
			//data[bi*branchOffset + si*sliceOffset + item*channels + 1] = ;
			//data[bi*branchOffset + si*sliceOffset + item*channels + 2] = lenght ;
			//data[bi*branchOffset + si*sliceOffset + item*channels + 3] = lenght ;

			// branch lenght
			/*
			item = 4;
			data[bi*branchOffset + si*sliceOffset + item*channels + 0] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 1] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 2] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 3] = 0.0 ;

			// motion vectors mv0, mv1
			item = 5;
			data[bi*branchOffset + si*sliceOffset + item*channels + 0] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 1] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 2] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 3] = 0.0 ;
					
			// motion vectors mv2, mv3
			item = 6;
			data[bi*branchOffset + si*sliceOffset + item*channels + 0] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 1] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 2] = 0.0 ;
			data[bi*branchOffset + si*sliceOffset + item*channels + 3] = 0.0 ;
			*/
		} // for each branch


	}// for each slice set

	Texture * dataTex = new Texture(  
		GL_TEXTURE_2D,
		GL_RGBA32F,
		GL_RGBA,
		GL_FLOAT,
		data,
		dimY, 
		dimX,
		DYN_TREE::LOD_DATA_TEXTURE_NAME
		);
	delete [] data;
	data = NULL;
	return dataTex;
}

bool DTree::loadDataTexture(string filename)
{
	return false;
}

bool DTree::saveDataTexture(string filename)
{
	return false;
}

bool DTree::loadVBO(string filename)
{
	return false;
}

bool DTree::saveVBO(string filename)
{
	return false;
}


int DTree::linearizeHierarchy(){
	queue<DTreeBranch*> bQueue;	
	bQueue.push(trunk);
	DTreeBranch * branch, *actBranch;
	int cnt = 0, i;
	while( !bQueue.empty() ){
		branch = bQueue.front();
		branch->id = cnt;
		cnt++;
		bQueue.pop();
		branches.push_back(branch);
		for (i=0; i<branch->children.size(); i++){
			if (! branch->children[i]->isLeaf()){
				actBranch = (DTreeBranch*)(branch->children[i]);
				bQueue.push(actBranch);
			} else {
				//leaves.push_back((TreeLeaf*) branch->children[i] );
			}
		}		
	}
	return cnt;
};

void DTree::recalcCoordSystems()

{
	/*
	*	Express original coord system in terms of parent coord system...
	*/

	int i, size=branches.size();
	DTreeBranch * branch;
	for (i=0; i<size; i++){
		branch = branches[i];
		if (branch->parent!=NULL){
			branch->cs = branch->parent->originalCS.getSystemInThisSystem(branch->originalCS);
		}
	}

}

void DTree::fillParentDataForEachBranch()
{
	/*
	*	Propagate data from parents to all children, grandchildren, etc.
	*/
	// and copy from parent...
	DTreeBranch* actBranch, *branch;
	CoordSystem *cSys;
	
	int i, size=branches.size();
	for (i=0; i<size; i++){
		branch = branches[i];
		actBranch = branch;
		while (actBranch!=NULL){
			cSys = &(actBranch->cs);
			branch->xvals.data		[actBranch->level] = actBranch->x;
			branch->lengths.data	[actBranch->level] = actBranch->L;
			branch->motionVectors	[actBranch->level] = actBranch->motionVector;
			branch->parentIDs		[actBranch->level] = actBranch->id;
			if (actBranch->parent!=NULL){
				branch->origins		[actBranch->level] = cSys->origin;
				branch->upVectors	[actBranch->level] = cSys->r;
				branch->rightVectors[actBranch->level] = cSys->s;
				branch->tVectors    [actBranch->level] = cSys->t;
				actBranch = (DTreeBranch*)(actBranch->parent);

			} else {
				branch->upVectors	[actBranch->level] = cSys->r;
				branch->rightVectors[actBranch->level] = cSys->s;
				branch->tVectors    [actBranch->level] = cSys->t;
				actBranch = NULL;
			}
		}
	}
}


Vegetation* DTree::getCopy(){
	DTree * copy = new DTree(this);
	return copy;
}

void DTree::draw_instance_LOD0(DTreeInstanceData * instance, float alpha){
	//drawNormals(instance);
	
	if (g_draw_lod0){
			// calc relative wind direction
			wind_relative_direction = instance->wind_dir;

			time_offset = instance->time_offset;
			glColor4f(1.0,1.0,1.0, alpha);
		
			glDisable(GL_CULL_FACE);
			glPushMatrix();
			glTranslatef(instance->position.x, instance->position.y, instance->position.z);
			glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);
			glScalef( 10.f , 10.f, 10.f);
		
			
		if (g_Draw2Shadowmap){
		
			// bind textures			
			dataTexture->bind(GL_TEXTURE1);
			branchNoiseTexture->bind(GL_TEXTURE2);
			// draw branches
			branchesEBO->draw(branchShader_sh);

			// bind textures
			
			leafNoiseTexture	->bind(GL_TEXTURE3);
			frontDecalMap		->bind(GL_TEXTURE4);
			seasonMap			->bind(GL_TEXTURE12);
		
			// draw leaves
			leavesVBO->draw(leafShader_sh, GL_QUADS, 0);
			leafShader_sh->use(false);
			leafNoiseTexture	->unbind();
			frontDecalMap		->unbind();
			seasonMap			->unbind();
			
			branchNoiseTexture	->unbind();
			dataTexture			->unbind();
		
		
		} else {
	
			// draw bbox
			//bbox->draw();


			// bind textures
			g_shadowmap1->bind(GL_TEXTURE0);
			dataTexture->bind(GL_TEXTURE1);
			branchNoiseTexture->bind(GL_TEXTURE2);
			bColorTexture->bind(GL_TEXTURE4);

			// TODO: use positions
			// draw branches
			branchesEBO->draw(branchShader);

			bColorTexture->unbind();

			// bind textures
			leafNoiseTexture->bind(GL_TEXTURE3);
			frontDecalMap		->bind(GL_TEXTURE4);
			seasonMap			->bind(GL_TEXTURE12);
			frontNormalMap		->bind(GL_TEXTURE5);
			frontTranslucencyMap->bind(GL_TEXTURE6);
			frontHalfLife2Map	->bind(GL_TEXTURE7);
			backDecalMap		->bind(GL_TEXTURE8);
			backNormalMap		->bind(GL_TEXTURE9);
			backTranslucencyMap	->bind(GL_TEXTURE10);
			backHalfLife2Map	->bind(GL_TEXTURE11);
		
			// TODO: send instance attributes
			leafShader->use(true);
			leafShader->setUniform3f(iu0Loc1, instance->colorVariance.r,instance->colorVariance.g, instance->colorVariance.b);

			// draw leaves
			leavesVBO->draw(leafShader, GL_QUADS, 0);

			frontNormalMap		->unbind();
			frontTranslucencyMap->unbind();
			frontHalfLife2Map	->unbind();
			backDecalMap		->unbind();
			backNormalMap		->unbind();
			backTranslucencyMap	->unbind();
			backHalfLife2Map	->unbind();
		
			leafNoiseTexture	->unbind();
			frontDecalMap		->unbind();
			branchNoiseTexture	->unbind();
			dataTexture			->unbind();
			seasonMap			->unbind();
			g_shadowmap1		->unbind();
			leafShader->use(false);
			
		}
		
		glPopMatrix();
		glEnable(GL_CULL_FACE);
	}
}

void DTree::draw_instance_LOD1(DTreeInstanceData * instance, float alpha){
	if (g_draw_lod1){
		if (g_Draw2Shadowmap){
			time_offset = instance->time_offset;
			//printf("draw LOD1 instance\n");
			glColor4f(1.0,1.0,1.0, alpha);
			glPushMatrix();
				glTranslatef(instance->position.x, instance->position.y, instance->position.z);
				glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);	
				glDisable(GL_CULL_FACE);
				glDisable(GL_LIGHTING);

				lod1_jColorMap			->bind(GL_TEXTURE1);
				leafNoiseTexture	->bind(GL_TEXTURE2);
				branchNoiseTexture  ->bind(GL_TEXTURE3);
				lod1_jDataMap			->bind(GL_TEXTURE4);
				lod1_jNormalMap			->bind(GL_TEXTURE5);
				seasonMap			->bind(GL_TEXTURE6);
				lod1_jDepthMap			->bind(GL_TEXTURE7);				
				lod1_dataTexture		->bind(GL_TEXTURE8);

				lod1_shader_shadow->use(true);
				lod1_vbo->bind(lod1_shader_shadow);
				// set attribute
				// draw EBO...
				lod1_ebo->bind();
			
					int off = instance->offset*(3*3*4*sizeof(unsigned int));
					// draw ebo
					lod1_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 3*3*4, BUFFER_OFFSET(off));  
				
					// disable all...
					lod1_ebo->unbind();
					lod1_vbo->unbind(lod1_shader_shadow);
			
				lod1_jColorMap			->unbind();
				leafNoiseTexture	->unbind();
				branchNoiseTexture	->unbind();
				lod1_jDataMap			->unbind();
				lod1_jNormalMap			->unbind();
				lod1_jDepthMap			->unbind();
				seasonMap			->unbind();
				lod1_dataTexture		->unbind();
					// turn shader off
				lod1_shader_shadow->use(false);
				glUseProgram(0);	
				glEnable(GL_CULL_FACE);
				glEnable(GL_LIGHTING);
			
			glPopMatrix();


		} else {
			time_offset = instance->time_offset;
			//printf("draw LOD1 instance\n");
			glColor4f(1.0,1.0,1.0, alpha);
			glPushMatrix();
				glTranslatef(instance->position.x, instance->position.y, instance->position.z);
				glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);

				Texture * colorTexture, * dataTexture, *displacementTexture, *displacement2Texture, *normalTexture;
				displacementTexture		=	leafNoiseTexture;
				displacement2Texture	=	branchNoiseTexture;
					
				glDisable(GL_CULL_FACE);
				glDisable(GL_LIGHTING);

				g_shadowmap1		->bind(GL_TEXTURE0);
				lod1_jColorMap			->bind(GL_TEXTURE1);
				displacementTexture	->bind(GL_TEXTURE2);
				displacement2Texture->bind(GL_TEXTURE3);
				lod1_jDataMap			->bind(GL_TEXTURE4);
				lod1_jNormalMap			->bind(GL_TEXTURE5);
				seasonMap			->bind(GL_TEXTURE6);
				lod1_jDepthMap			->bind(GL_TEXTURE7);				
				lod1_dataTexture		->bind(GL_TEXTURE8);
				lod1_shader->use(true);
			
			
			
			//lod1_shader->setTexture(l2_season	, seasonMap				->textureUnitNumber	);
			//
			//lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
			//lod1_shader->setTexture(l2_displ	, displacementTexture	->textureUnitNumber	);			
			//lod1_shader->setTexture(l2_displ2	, displacement2Texture	->textureUnitNumber	);
			//lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
			//lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
			//lod1_shader->setTexture(l2_depth	, lod1_jDepthMap				->textureUnitNumber	);
			//
				lod1_shader->setUniform3f(iu1Loc1, instance->colorVariance.r, instance->colorVariance.g, instance->colorVariance.b);
				lod1_vbo->bind(lod1_shader);
				// set attribute
				// draw EBO...
				lod1_ebo->bind();
			
					int off = instance->offset*(3*3*4*sizeof(unsigned int));
					// draw ebo
					lod1_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 3*3*4, BUFFER_OFFSET(off));  


					// disable all...
					lod1_ebo->unbind();
					lod1_vbo->unbind(lod1_shader);
			
				
					// turn shader off
				lod1_shader->use(false);
				lod1_jColorMap			->unbind();
				displacementTexture	->unbind();
				displacement2Texture->unbind();
				lod1_jDataMap			->unbind();
				lod1_jNormalMap			->unbind();
				lod1_jDepthMap			->unbind();
				seasonMap			->unbind();
				g_shadowmap1		->unbind();
				lod1_dataTexture		->unbind();

				glUseProgram(0);	
				glEnable(GL_CULL_FACE);
				glEnable(GL_LIGHTING);
			
			glPopMatrix();
		} // if not draw to shadow map...
	}
}

void DTree::draw_all_instances_LOD1(){
	g_transitionControl = 1.0;
	if (g_draw_lod1 && g_tree_lod1_count>0){
		if (g_Draw2Shadowmap){			
			glColor4f(1.0, 1.0, 1.0, 1.0);	
			int i, j, sliceCount, setCount=lod1_sliceSets.size();
			
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
				lod1_jColorMap			->bind(GL_TEXTURE1);
				leafNoiseTexture	->bind(GL_TEXTURE2);
				branchNoiseTexture	->bind(GL_TEXTURE3);
				lod1_jDataMap			->bind(GL_TEXTURE4);
				lod1_jNormalMap			->bind(GL_TEXTURE5);
				seasonMap			->bind(GL_TEXTURE6);
				lod1_jDepthMap			->bind(GL_TEXTURE7);
				lod1_dataTexture		->bind(GL_TEXTURE8);

			lod1_shader_shadow->use(true);
			
				//lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_displ	, displacementTexture	->textureUnitNumber	);			
				//lod1_shader->setTexture(l2_displ2	, displacement2Texture	->textureUnitNumber	);
				//lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
				//lod1_shader->setTexture(l2_season	, seasonMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_depth	, lod1_jDepthMap				->textureUnitNumber	);
			
				// bind element buffer
				lod1_ebo->bind();
				// bind vertex attribute buffer
				lod1_vbo->bind(lod1_shader_shadow);	

				// instance matrices VBO


				// bind instance data
				glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
				glEnableVertexAttribArray(tmLoc0_shadow);
				glEnableVertexAttribArray(tmLoc0_shadow + 1);
				glEnableVertexAttribArray(tmLoc0_shadow + 2);
				glEnableVertexAttribArray(tmLoc0_shadow + 3);
				
				if (iaLoc1_shadow>=0){
					glEnableVertexAttribArray(iaLoc1_shadow);
					glVertexAttribPointer(iaLoc1_shadow, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 16));
					glVertexAttribDivisor(iaLoc1_shadow, 1);
				}
				
				glVertexAttribPointer(tmLoc0_shadow, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(0));
				glVertexAttribPointer(tmLoc0_shadow + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 4));
				glVertexAttribPointer(tmLoc0_shadow + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 8));
				glVertexAttribPointer(tmLoc0_shadow + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 12));
				// color variations, time_offset
			
				glVertexAttribDivisor(tmLoc0_shadow, 1);
				glVertexAttribDivisor(tmLoc0_shadow + 1, 1);
				glVertexAttribDivisor(tmLoc0_shadow + 2, 1);
				glVertexAttribDivisor(tmLoc0_shadow + 3, 1);
			
				for (int i=0; i<lod1_instanceMatrices.size(); i++){
					// transfer data to buffer
					if (lod1_typeIndices[i]>0){
						glBufferData(GL_ARRAY_BUFFER, lod1_typeIndices[i] * instanceFloatCount * sizeof(float), lod1_instanceMatrices[i], GL_STREAM_DRAW);
						int off = i*(3*3*4*sizeof(unsigned int));
						void * offset = BUFFER_OFFSET(off);
						// draw instanced
						lod1_ebo->drawInstanced(GL_QUADS, 3*3*4, GL_UNSIGNED_INT, offset, lod1_typeIndices[i]);  
					}
				} // for each configuration
			
				glVertexAttribDivisor(tmLoc0_shadow, 0);
				glVertexAttribDivisor(tmLoc0_shadow + 1, 0);
				glVertexAttribDivisor(tmLoc0_shadow + 2, 0);
				glVertexAttribDivisor(tmLoc0_shadow + 3, 0);
				// disable all...
				
				if (iaLoc1_shadow>=0){
					glVertexAttribDivisor(iaLoc1_shadow, 0);
					glDisableVertexAttribArray(iaLoc1_shadow);
				}
				
				glDisableVertexAttribArray(tmLoc0_shadow);
				glDisableVertexAttribArray(tmLoc0_shadow + 1);
				glDisableVertexAttribArray(tmLoc0_shadow + 2);
				glDisableVertexAttribArray(tmLoc0_shadow + 3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				lod1_ebo->unbind();
				lod1_vbo->unbind(lod1_shader_shadow);
			
				lod1_jColorMap			->unbind();
				leafNoiseTexture	->unbind();
				branchNoiseTexture	->unbind();
				lod1_jDataMap			->unbind();
				lod1_jNormalMap			->unbind();
				lod1_jDepthMap			->unbind();
				seasonMap			->unbind();
				lod1_dataTexture		->unbind();
				// turn shader off
			lod1_shader_shadow->use(false);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		} else {
			glColor4f(1.0, 1.0, 1.0, 1.0);	
			int i, j, sliceCount, setCount=lod1_sliceSets.size();
		
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);

				g_shadowmap1		->bind(GL_TEXTURE0);
				lod1_jColorMap			->bind(GL_TEXTURE1);
				leafNoiseTexture	->bind(GL_TEXTURE2);
				branchNoiseTexture	->bind(GL_TEXTURE3);
				lod1_jDataMap			->bind(GL_TEXTURE4);
				lod1_jNormalMap			->bind(GL_TEXTURE5);
				seasonMap			->bind(GL_TEXTURE6);
				lod1_jDepthMap			->bind(GL_TEXTURE7);
				lod1_dataTexture		->bind(GL_TEXTURE8);

			lod1_shader->use(true);
			
				//lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_displ	, leafNoiseTexture		->textureUnitNumber	);			
				//lod1_shader->setTexture(l2_displ2	, branchNoiseTexture	->textureUnitNumber	);
				//lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
				//lod1_shader->setTexture(l2_season	, seasonMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_depth	, lod1_jDepthMap				->textureUnitNumber	);
			
				// bind element buffer
				lod1_ebo->bind();
				// bind vertex attribute buffer
				lod1_vbo->bind(lod1_shader);	

				// instance matrices VBO


				// bind instance data
				glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
				glEnableVertexAttribArray(tmLoc0);
				glEnableVertexAttribArray(tmLoc0 + 1);
				glEnableVertexAttribArray(tmLoc0 + 2);
				glEnableVertexAttribArray(tmLoc0 + 3);
				if (iaLoc1>=0){
					glEnableVertexAttribArray(iaLoc1);
					glVertexAttribPointer(iaLoc1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 16));
					glVertexAttribDivisor(iaLoc1, 1);
				}
				glVertexAttribPointer(tmLoc0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(0));
				glVertexAttribPointer(tmLoc0 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 4));
				glVertexAttribPointer(tmLoc0 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 8));
				glVertexAttribPointer(tmLoc0 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 12));
				// color variations, time_offset
			
				glVertexAttribDivisor(tmLoc0, 1);
				glVertexAttribDivisor(tmLoc0 + 1, 1);
				glVertexAttribDivisor(tmLoc0 + 2, 1);
				glVertexAttribDivisor(tmLoc0 + 3, 1);
			
				for (int i=0; i<lod1_instanceMatrices.size(); i++){
					// transfer data to buffer
					if (lod1_typeIndices[i]>0){
						glBufferData(GL_ARRAY_BUFFER, lod1_typeIndices[i] * instanceFloatCount * sizeof(float), lod1_instanceMatrices[i], GL_STREAM_DRAW);
						int off = i*(3*3*4*sizeof(unsigned int));
						void * offset = BUFFER_OFFSET(off);
						// draw instanced
						lod1_ebo->drawInstanced(GL_QUADS, 3*3*4, GL_UNSIGNED_INT, offset, lod1_typeIndices[i]);  
					}
				} // for each configuration
			
				glVertexAttribDivisor(tmLoc0, 0);
				glVertexAttribDivisor(tmLoc0 + 1, 0);
				glVertexAttribDivisor(tmLoc0 + 2, 0);
				glVertexAttribDivisor(tmLoc0 + 3, 0);
				// disable all...
				if (iaLoc1>=0){
					glVertexAttribDivisor(iaLoc1, 0);
					glDisableVertexAttribArray(iaLoc1);
				}
				glDisableVertexAttribArray(tmLoc0);
				glDisableVertexAttribArray(tmLoc0 + 1);
				glDisableVertexAttribArray(tmLoc0 + 2);
				glDisableVertexAttribArray(tmLoc0 + 3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				lod1_ebo->unbind();
				lod1_vbo->unbind(lod1_shader);
			
				
				// turn shader off
			lod1_shader->use(false);
			lod1_jColorMap			->unbind();
			leafNoiseTexture	->unbind();
			branchNoiseTexture	->unbind();
			lod1_jDataMap			->unbind();
			lod1_jNormalMap			->unbind();
			lod1_jDepthMap			->unbind();
			seasonMap			->unbind();
			g_shadowmap1        ->unbind();
			lod1_dataTexture		->unbind();
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		} // if not drawing to shadow map
	}
}

void DTree::draw_instance_LOD2b(DTreeInstanceData * instance, float alpha){
	if (g_draw_lod2){
		if (g_Draw2Shadowmap){
			time_offset = instance->time_offset;
			//printf("draw LOD1 instance\n");
			glColor4f(1.0,1.0,1.0, alpha);
			glPushMatrix();
				glTranslatef(instance->position.x, instance->position.y, instance->position.z);
				glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);	
				glDisable(GL_CULL_FACE);
				glDisable(GL_LIGHTING);
				
				leafNoiseTexture		->bind(GL_TEXTURE2);
				branchNoiseTexture		->bind(GL_TEXTURE3);
				seasonMap				->bind(GL_TEXTURE6);
				lod2_jColorMap			->bind(GL_TEXTURE1);
				lod2_jDataMap			->bind(GL_TEXTURE4);
				lod2_jNormalMap			->bind(GL_TEXTURE5);
				lod2_jDepthMap			->bind(GL_TEXTURE7);				
				lod2_dataTexture		->bind(GL_TEXTURE8);

				lod2_shader_shadow->use(true);
				lod2_vbo->bind(lod2_shader_shadow);
				// set attribute
				// draw EBO...
				lod2_ebo->bind();
			
					int off = instance->offset*(2*4*sizeof(unsigned int));
					// draw ebo
					lod2_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 2*4, BUFFER_OFFSET(off));  
				
					// disable all...
					lod2_ebo->unbind();
					lod2_vbo->unbind(lod2_shader_shadow);

				seasonMap				->unbind();
				leafNoiseTexture		->unbind();
				branchNoiseTexture		->unbind();
				lod2_jColorMap			->unbind();
				lod2_jDataMap			->unbind();
				lod2_jNormalMap			->unbind();
				lod2_jDepthMap			->unbind();
				lod2_dataTexture		->unbind();
					// turn shader off
				lod2_shader_shadow->use(false);
				glUseProgram(0);	
				glEnable(GL_CULL_FACE);
				glEnable(GL_LIGHTING);
			
			glPopMatrix();


		} else {
			time_offset = instance->time_offset;
			//printf("draw LOD1 instance\n");
			glColor4f(1.0,1.0,1.0, alpha);
			glPushMatrix();
				glTranslatef(instance->position.x, instance->position.y, instance->position.z);
				glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);
	
				glDisable(GL_CULL_FACE);
				glDisable(GL_LIGHTING);

				g_shadowmap1			->bind(GL_TEXTURE0);				
				leafNoiseTexture		->bind(GL_TEXTURE2);
				branchNoiseTexture		->bind(GL_TEXTURE3);
				seasonMap				->bind(GL_TEXTURE6);
				lod2_jColorMap			->bind(GL_TEXTURE1);
				lod2_jDataMap			->bind(GL_TEXTURE4);
				lod2_jNormalMap			->bind(GL_TEXTURE5);				
				lod2_jDepthMap			->bind(GL_TEXTURE7);				
				lod2_dataTexture		->bind(GL_TEXTURE8);
				lod2_shader->use(true);
			
			
			
			//lod1_shader->setTexture(l2_season	, seasonMap				->textureUnitNumber	);
			//
			//lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
			//lod1_shader->setTexture(l2_displ	, displacementTexture	->textureUnitNumber	);			
			//lod1_shader->setTexture(l2_displ2	, displacement2Texture	->textureUnitNumber	);
			//lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
			//lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
			//lod1_shader->setTexture(l2_depth	, lod1_jDepthMap				->textureUnitNumber	);
			//
				lod2_shader->setUniform3f(lod2_iu1Loc1, instance->colorVariance.r, instance->colorVariance.g, instance->colorVariance.b);
				lod2_vbo->bind(lod2_shader);
				// set attribute
				// draw EBO...
				lod2_ebo->bind();
			
					int off = instance->offset*(2*4*sizeof(unsigned int));
					// draw ebo
					lod2_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 2*4, BUFFER_OFFSET(off));  


					// disable all...
					lod2_ebo->unbind();
					lod2_vbo->unbind(lod2_shader);
			
				
					// turn shader off
				lod2_shader->use(false);
				lod2_jColorMap			->unbind();				
				lod2_jDataMap			->unbind();
				lod2_jNormalMap			->unbind();
				lod2_jDepthMap			->unbind();
				lod2_dataTexture		->unbind();

				seasonMap				->unbind();
				g_shadowmap1			->unbind();
				leafNoiseTexture		->unbind();
				branchNoiseTexture		->unbind();
				
				glUseProgram(0);	
				glEnable(GL_CULL_FACE);
				glEnable(GL_LIGHTING);
			
			glPopMatrix();
		} // if not draw to shadow map...
	}
}

void DTree::draw_all_instances_LOD2b(){
	g_transitionControl = 1.0;
	if (g_draw_lod2 && g_tree_lod2_count>0){
		if (g_Draw2Shadowmap){			
			glColor4f(1.0, 1.0, 1.0, 1.0);	
			int i, j, sliceCount, setCount=lod2_sliceSets.size();
			
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
				lod2_jColorMap			->bind(GL_TEXTURE1);
				leafNoiseTexture		->bind(GL_TEXTURE2);
				branchNoiseTexture		->bind(GL_TEXTURE3);
				lod2_jDataMap			->bind(GL_TEXTURE4);
				lod2_jNormalMap			->bind(GL_TEXTURE5);
				seasonMap				->bind(GL_TEXTURE6);
				lod2_jDepthMap			->bind(GL_TEXTURE7);
				lod2_dataTexture		->bind(GL_TEXTURE8);

			lod2_shader_shadow->use(true);
			
				//lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_displ	, displacementTexture	->textureUnitNumber	);			
				//lod1_shader->setTexture(l2_displ2	, displacement2Texture	->textureUnitNumber	);
				//lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
				//lod1_shader->setTexture(l2_season	, seasonMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_depth	, lod1_jDepthMap				->textureUnitNumber	);
			
				// bind element buffer
				lod2_ebo->bind();
				// bind vertex attribute buffer
				lod2_vbo->bind(lod2_shader_shadow);	

				// instance matrices VBO


				// bind instance data
				glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
				glEnableVertexAttribArray(lod2_tmLoc0_shadow);
				glEnableVertexAttribArray(lod2_tmLoc0_shadow + 1);
				glEnableVertexAttribArray(lod2_tmLoc0_shadow + 2);
				glEnableVertexAttribArray(lod2_tmLoc0_shadow + 3);
				
				if (lod2_iaLoc1_shadow>=0){
					glEnableVertexAttribArray(lod2_iaLoc1_shadow);
					glVertexAttribPointer(lod2_iaLoc1_shadow, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 16));
					glVertexAttribDivisor(lod2_iaLoc1_shadow, 1);
				}
				
				glVertexAttribPointer(lod2_tmLoc0_shadow, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(0));
				glVertexAttribPointer(lod2_tmLoc0_shadow + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 4));
				glVertexAttribPointer(lod2_tmLoc0_shadow + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 8));
				glVertexAttribPointer(lod2_tmLoc0_shadow + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 12));
				// color variations, time_offset
			
				glVertexAttribDivisor(lod2_tmLoc0_shadow, 1);
				glVertexAttribDivisor(lod2_tmLoc0_shadow + 1, 1);
				glVertexAttribDivisor(lod2_tmLoc0_shadow + 2, 1);
				glVertexAttribDivisor(lod2_tmLoc0_shadow + 3, 1);
			
				for (int i=0; i<lod2_instanceMatrices.size(); i++){
					// transfer data to buffer
					if (lod2_typeIndices[i]>0){
						glBufferData(GL_ARRAY_BUFFER, lod2_typeIndices[i] * instanceFloatCount * sizeof(float), lod2_instanceMatrices[i], GL_STREAM_DRAW);
						int off = i*(2*4*sizeof(unsigned int));
						void * offset = BUFFER_OFFSET(off);
						// draw instanced
						lod2_ebo->drawInstanced(GL_QUADS, 2*4, GL_UNSIGNED_INT, offset, lod2_typeIndices[i]);  
					}
				} // for each configuration
			
				glVertexAttribDivisor(lod2_tmLoc0_shadow, 0);
				glVertexAttribDivisor(lod2_tmLoc0_shadow + 1, 0);
				glVertexAttribDivisor(lod2_tmLoc0_shadow + 2, 0);
				glVertexAttribDivisor(lod2_tmLoc0_shadow + 3, 0);
				// disable all...
				
				if (lod2_iaLoc1_shadow>=0){
					glVertexAttribDivisor(lod2_iaLoc1_shadow, 0);
					glDisableVertexAttribArray(lod2_iaLoc1_shadow);
				}
				
				glDisableVertexAttribArray(lod2_tmLoc0_shadow);
				glDisableVertexAttribArray(lod2_tmLoc0_shadow + 1);
				glDisableVertexAttribArray(lod2_tmLoc0_shadow + 2);
				glDisableVertexAttribArray(lod2_tmLoc0_shadow + 3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				lod2_ebo->unbind();
				lod2_vbo->unbind(lod2_shader_shadow);
			
				leafNoiseTexture		->unbind();
				branchNoiseTexture		->unbind();
				seasonMap				->unbind();
				lod2_jDataMap			->unbind();
				lod2_jNormalMap			->unbind();
				lod2_jDepthMap			->unbind();
				lod2_jColorMap			->unbind();
				lod2_dataTexture		->unbind();
				// turn shader off
			lod2_shader_shadow->use(false);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		} else {
			glColor4f(1.0, 1.0, 1.0, 1.0);	
			int i, j, sliceCount, setCount=lod2_sliceSets.size();
		
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);

				g_shadowmap1			->bind(GL_TEXTURE0);
				seasonMap				->bind(GL_TEXTURE6);
				leafNoiseTexture		->bind(GL_TEXTURE2);
				branchNoiseTexture		->bind(GL_TEXTURE3);
				lod2_jDataMap			->bind(GL_TEXTURE4);
				lod2_jNormalMap			->bind(GL_TEXTURE5);
				lod2_jColorMap			->bind(GL_TEXTURE1);
				lod2_jDepthMap			->bind(GL_TEXTURE7);
				lod2_dataTexture		->bind(GL_TEXTURE8);

			lod2_shader->use(true);
			
				//lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_displ	, leafNoiseTexture		->textureUnitNumber	);			
				//lod1_shader->setTexture(l2_displ2	, branchNoiseTexture	->textureUnitNumber	);
				//lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
				//lod1_shader->setTexture(l2_season	, seasonMap				->textureUnitNumber	);
				//lod1_shader->setTexture(l2_depth	, lod1_jDepthMap				->textureUnitNumber	);
			
				// bind element buffer
				lod2_ebo->bind();
				// bind vertex attribute buffer
				lod2_vbo->bind(lod2_shader);	

				// instance matrices VBO


				// bind instance data
				glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
				glEnableVertexAttribArray(lod2_tmLoc0);
				glEnableVertexAttribArray(lod2_tmLoc0 + 1);
				glEnableVertexAttribArray(lod2_tmLoc0 + 2);
				glEnableVertexAttribArray(lod2_tmLoc0 + 3);
				if (lod2_iaLoc1>=0){
					glEnableVertexAttribArray(lod2_iaLoc1);
					glVertexAttribPointer(lod2_iaLoc1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 16));
					glVertexAttribDivisor(lod2_iaLoc1, 1);
				}
				glVertexAttribPointer(lod2_tmLoc0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(0));
				glVertexAttribPointer(lod2_tmLoc0 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 4));
				glVertexAttribPointer(lod2_tmLoc0 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 8));
				glVertexAttribPointer(lod2_tmLoc0 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 12));
				// color variations, time_offset
			
				glVertexAttribDivisor(lod2_tmLoc0, 1);
				glVertexAttribDivisor(lod2_tmLoc0 + 1, 1);
				glVertexAttribDivisor(lod2_tmLoc0 + 2, 1);
				glVertexAttribDivisor(lod2_tmLoc0 + 3, 1);
			
				for (int i=0; i<lod2_instanceMatrices.size(); i++){
					// transfer data to buffer
					if (lod2_typeIndices[i]>0){
						glBufferData(GL_ARRAY_BUFFER, lod2_typeIndices[i] * instanceFloatCount * sizeof(float), lod2_instanceMatrices[i], GL_STREAM_DRAW);
						int off = i*(2*4*sizeof(unsigned int));
						void * offset = BUFFER_OFFSET(off);
						// draw instanced
						lod2_ebo->drawInstanced(GL_QUADS, 2*4, GL_UNSIGNED_INT, offset, lod2_typeIndices[i]);  
					}
				} // for each configuration
			
				glVertexAttribDivisor(lod2_tmLoc0, 0);
				glVertexAttribDivisor(lod2_tmLoc0 + 1, 0);
				glVertexAttribDivisor(lod2_tmLoc0 + 2, 0);
				glVertexAttribDivisor(lod2_tmLoc0 + 3, 0);
				// disable all...
				if (lod2_iaLoc1>=0){
					glVertexAttribDivisor(lod2_iaLoc1, 0);
					glDisableVertexAttribArray(lod2_iaLoc1);
				}
				glDisableVertexAttribArray(lod2_tmLoc0);
				glDisableVertexAttribArray(lod2_tmLoc0 + 1);
				glDisableVertexAttribArray(lod2_tmLoc0 + 2);
				glDisableVertexAttribArray(lod2_tmLoc0 + 3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				lod2_ebo->unbind();
				lod2_vbo->unbind(lod2_shader);
			
				
				// turn shader off
			lod2_shader->use(false);
			leafNoiseTexture		->unbind();
			branchNoiseTexture		->unbind();
			seasonMap				->unbind();
			g_shadowmap1			->unbind();
			lod2_jColorMap			->unbind();
			lod2_jDataMap			->unbind();
			lod2_jNormalMap			->unbind();
			lod2_jDepthMap			->unbind();	
			lod2_dataTexture		->unbind();
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		} // if not drawing to shadow map
	}
}


void DTree::draw_instance_LOD2(DTreeInstanceData * instance, float alpha){
	if (g_draw_lod2){
		
		if (g_Draw2Shadowmap){
			time_offset = instance->time_offset;
			//printf("draw LOD1 instance\n");
			glColor4f(1.0,1.0,1.0, alpha);
			glPushMatrix();
				glTranslatef(instance->position.x, instance->position.y, instance->position.z);
				glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);

				glDisable(GL_CULL_FACE);
				glDisable(GL_LIGHTING);	
				leafNoiseTexture	->bind(GL_TEXTURE7);
				branchNoiseTexture  ->bind(GL_TEXTURE8);
				seasonMap			->bind(GL_TEXTURE6);
				lod2color1			->bind(GL_TEXTURE0);
				lod2color2			->bind(GL_TEXTURE1);
				lod2normal1			->bind(GL_TEXTURE2);
				lod2normal2			->bind(GL_TEXTURE3);
				lod2branch1			->bind(GL_TEXTURE4);
				lod2branch2			->bind(GL_TEXTURE5);
				lod2depth1			->bind(GL_TEXTURE10);
				lod2depth2			->bind(GL_TEXTURE11);
			
				lod2_shader_shadow->use(true);
				lod2_vbo->bind(lod2_shader_shadow);
				// set attribute
				// draw EBO...
				lod2_ebo->bind();
			
					int off = instance->offset*(2*4*sizeof(unsigned int));
					// draw ebo
					lod2_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 2*4, BUFFER_OFFSET(off));  

					// disable all...
					lod2_ebo->unbind();
					lod2_vbo->unbind(lod2_shader_shadow);
				leafNoiseTexture	->unbind();
				branchNoiseTexture	->unbind();
				lod2color1			->unbind();
				lod2color2			->unbind();
				lod2normal1			->unbind();
				lod2normal2			->unbind();
				lod2branch1			->unbind();
				lod2branch2			->unbind();
					// turn shader off
				lod2_shader_shadow->use(false);
				glUseProgram(0);	
				glEnable(GL_CULL_FACE);
				glEnable(GL_LIGHTING);			
			glPopMatrix();
		} else {
			time_offset = instance->time_offset;
			//printf("draw LOD1 instance\n");
			glColor4f(1.0,1.0,1.0, alpha);
			glPushMatrix();
				glTranslatef(instance->position.x, instance->position.y, instance->position.z);
				glRotatef(instance->rotation_y, 0.0, 1.0, 0.0);

				glDisable(GL_CULL_FACE);
				glDisable(GL_LIGHTING);
				g_shadowmap1		->bind(GL_TEXTURE9);			
				leafNoiseTexture	->bind(GL_TEXTURE7);
				branchNoiseTexture  ->bind(GL_TEXTURE8);
				seasonMap			->bind(GL_TEXTURE6);
				lod2color1			->bind(GL_TEXTURE0);
				lod2color2			->bind(GL_TEXTURE1);
				lod2normal1			->bind(GL_TEXTURE2);
				lod2normal2			->bind(GL_TEXTURE3);
				lod2branch1			->bind(GL_TEXTURE4);
				lod2branch2			->bind(GL_TEXTURE5);
				lod2depth1			->bind(GL_TEXTURE10);
				lod2depth2			->bind(GL_TEXTURE11);
			
				lod2_shader->use(true);
				lod2_shader->setUniform3f(lod2loc_colorVariance, instance->colorVariance.r, instance->colorVariance.g, instance->colorVariance.b);
				lod2_vbo->bind(lod2_shader);
				// set attribute
				// draw EBO...
				lod2_ebo->bind();
			
					int off = instance->offset*(2*4*sizeof(unsigned int));
					// draw ebo
					lod2_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 2*4, BUFFER_OFFSET(off));  

					// disable all...
					lod2_ebo->unbind();
					lod2_vbo->unbind(lod2_shader);
				leafNoiseTexture	->unbind();
				branchNoiseTexture	->unbind();
				lod2color1			->unbind();
				lod2color2			->unbind();
				lod2normal1			->unbind();
				lod2normal2			->unbind();
				lod2branch1			->unbind();
				lod2branch2			->unbind();
				g_shadowmap1		->unbind();
					// turn shader off
				lod2_shader->use(false);
				glUseProgram(0);	
				glEnable(GL_CULL_FACE);
				glEnable(GL_LIGHTING);
			
			glPopMatrix();
		} // if not drawing to shadowmap
	}

}

void DTree::draw_all_instances_LOD2(){

	
	if (g_draw_lod2 && g_tree_lod2_count>0){		
		if (g_Draw2Shadowmap){
			glColor4f(1.0, 1.0, 1.0, 1.0);	
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
				leafNoiseTexture	->bind(GL_TEXTURE7);
				branchNoiseTexture  ->bind(GL_TEXTURE8);
				seasonMap			->bind(GL_TEXTURE6);
				lod2color1			->bind(GL_TEXTURE0);
				lod2color2			->bind(GL_TEXTURE1);
				lod2normal1			->bind(GL_TEXTURE2);
				lod2normal2			->bind(GL_TEXTURE3);
				lod2branch1			->bind(GL_TEXTURE4);
				lod2branch2			->bind(GL_TEXTURE5);
				lod2depth1			->bind(GL_TEXTURE10);
				lod2depth2			->bind(GL_TEXTURE11);

			lod2_shader_shadow->use(true);
			

				//lod2_shader->setTexture(lod2loc_color_tex_1		, lod2color1			->textureUnitNumber	);
				//lod2_shader->setTexture(lod2loc_color_tex_2		, lod2color2			->textureUnitNumber	);
				//lod2_shader->setTexture(lod2loc_normal_tex_1		, lod2normal1			->textureUnitNumber	);
				//lod2_shader->setTexture(lod2loc_normal_tex_2		, lod2normal2			->textureUnitNumber	);
				//lod2_shader->setTexture(lod2loc_branch_tex_1		, lod2branch1			->textureUnitNumber	);
				//lod2_shader->setTexture(lod2loc_branch_tex_2		, lod2branch2			->textureUnitNumber	);
				//
				//
				//lod2_shader->setTexture(lod2loc_leaf_tex		, leafNoiseTexture		->textureUnitNumber	);	
				//lod2_shader->setTexture(lod2loc_branch_tex	, branchNoiseTexture	->textureUnitNumber	);
				//lod2_shader->setTexture(lod2loc_season_tex	, seasonMap				->textureUnitNumber	);
				// bind element buffer
				lod2_ebo->bind();
				// bind vertex attribute buffer
				lod2_vbo->bind(lod2_shader_shadow);	

				// instance matrices VBO


				// bind instance data
				glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
				glEnableVertexAttribArray(tm2Loc0_shadow);
				glEnableVertexAttribArray(tm2Loc0_shadow + 1);
				glEnableVertexAttribArray(tm2Loc0_shadow + 2);
				glEnableVertexAttribArray(tm2Loc0_shadow + 3);
				glEnableVertexAttribArray(ia2Loc1_shadow);
				glVertexAttribPointer(tm2Loc0_shadow, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(0));
				glVertexAttribPointer(tm2Loc0_shadow + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 4));
				glVertexAttribPointer(tm2Loc0_shadow + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 8));
				glVertexAttribPointer(tm2Loc0_shadow + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 12));
				// color variations, time_offset
				glVertexAttribPointer(ia2Loc1_shadow, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 16));

				glVertexAttribDivisor(tm2Loc0_shadow, 1);
				glVertexAttribDivisor(tm2Loc0_shadow + 1, 1);
				glVertexAttribDivisor(tm2Loc0_shadow + 2, 1);
				glVertexAttribDivisor(tm2Loc0_shadow + 3, 1);
				glVertexAttribDivisor(ia2Loc1_shadow, 1);
				int size = lod2_instanceMatrices.size();

				for (int i=0; i<size; i++){
					if (lod2_typeIndices[i]>0){
						// transfer data to buffer
						glBufferData(GL_ARRAY_BUFFER, lod2_typeIndices[i] * instanceFloatCount * sizeof(float), lod2_instanceMatrices[i], GL_STREAM_DRAW);
						int off = i*(2*4*sizeof(unsigned int));
						void * offset = BUFFER_OFFSET(off);
						// draw instanced
						lod2_ebo->drawInstanced(GL_QUADS, 2*4, GL_UNSIGNED_INT, offset, lod2_typeIndices[i]);  
					}
				} // for each configuration
				glVertexAttribDivisor(ia2Loc1_shadow, 0);
				glVertexAttribDivisor(tm2Loc0_shadow, 0);
				glVertexAttribDivisor(tm2Loc0_shadow + 1, 0);
				glVertexAttribDivisor(tm2Loc0_shadow + 2, 0);
				glVertexAttribDivisor(tm2Loc0_shadow + 3, 0);
				// disable all...
				glDisableVertexAttribArray(ia2Loc1_shadow);
				glDisableVertexAttribArray(tm2Loc0_shadow);
				glDisableVertexAttribArray(tm2Loc0_shadow + 1);
				glDisableVertexAttribArray(tm2Loc0_shadow + 2);
				glDisableVertexAttribArray(tm2Loc0_shadow + 3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				lod2_ebo->unbind();
				lod2_vbo->unbind(lod2_shader_shadow);
			
				leafNoiseTexture	->unbind();
				branchNoiseTexture  ->unbind();
				seasonMap			->unbind();
				lod2color1			->unbind();
				lod2color2			->unbind();
				lod2normal1			->unbind();
				lod2normal2			->unbind();
				lod2branch1			->unbind();
				lod2branch2			->unbind();
				lod2depth1			->unbind();
				lod2depth2			->unbind();
				// turn shader off
			lod2_shader_shadow->use(false);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		} else {
			// not draw to shadow map...
			glColor4f(1.0, 1.0, 1.0, 1.0);	
		
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);

				leafNoiseTexture	->bind(GL_TEXTURE7);
				branchNoiseTexture  ->bind(GL_TEXTURE8);
				seasonMap			->bind(GL_TEXTURE6);
				lod2color1			->bind(GL_TEXTURE0);
				lod2color2			->bind(GL_TEXTURE1);
				lod2normal1			->bind(GL_TEXTURE2);
				lod2normal2			->bind(GL_TEXTURE3);
				lod2branch1			->bind(GL_TEXTURE4);
				lod2branch2			->bind(GL_TEXTURE5);
				g_shadowmap1		->bind(GL_TEXTURE9);

			lod2_shader->use(true);
			

				lod2_shader->setTexture(lod2loc_color_tex_1		, lod2color1			->textureUnitNumber	);
				lod2_shader->setTexture(lod2loc_color_tex_2		, lod2color2			->textureUnitNumber	);
				lod2_shader->setTexture(lod2loc_normal_tex_1		, lod2normal1			->textureUnitNumber	);
				lod2_shader->setTexture(lod2loc_normal_tex_2		, lod2normal2			->textureUnitNumber	);
				lod2_shader->setTexture(lod2loc_branch_tex_1		, lod2branch1			->textureUnitNumber	);
				lod2_shader->setTexture(lod2loc_branch_tex_2		, lod2branch2			->textureUnitNumber	);


				lod2_shader->setTexture(lod2loc_leaf_tex		, leafNoiseTexture		->textureUnitNumber	);	
				lod2_shader->setTexture(lod2loc_branch_tex	, branchNoiseTexture	->textureUnitNumber	);
				lod2_shader->setTexture(lod2loc_season_tex	, seasonMap				->textureUnitNumber	);
				// bind element buffer
				lod2_ebo->bind();
				// bind vertex attribute buffer
				lod2_vbo->bind(lod2_shader);	

				// instance matrices VBO

				// bind instance data
				glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
				glEnableVertexAttribArray(tm2Loc0);
				glEnableVertexAttribArray(tm2Loc0 + 1);
				glEnableVertexAttribArray(tm2Loc0 + 2);
				glEnableVertexAttribArray(tm2Loc0 + 3);
				glEnableVertexAttribArray(ia2Loc1);
				glVertexAttribPointer(tm2Loc0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(0));
				glVertexAttribPointer(tm2Loc0 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 4));
				glVertexAttribPointer(tm2Loc0 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 8));
				glVertexAttribPointer(tm2Loc0 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 12));
				// color variations, time_offset
				glVertexAttribPointer(ia2Loc1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * instanceFloatCount, (void*)(sizeof(float) * 16));

				glVertexAttribDivisor(tm2Loc0, 1);
				glVertexAttribDivisor(tm2Loc0 + 1, 1);
				glVertexAttribDivisor(tm2Loc0 + 2, 1);
				glVertexAttribDivisor(tm2Loc0 + 3, 1);
				glVertexAttribDivisor(ia2Loc1, 1);
				int size = lod2_instanceMatrices.size();
				for (int i=0; i<size; i++){
					if (lod2_typeIndices[i]>0){
						// transfer data to buffer
						glBufferData(GL_ARRAY_BUFFER, lod2_typeIndices[i] * instanceFloatCount * sizeof(float), lod2_instanceMatrices[i], GL_STREAM_DRAW);
						int off = i*(2*4*sizeof(unsigned int));
						void * offset = BUFFER_OFFSET(off);
						// draw instanced
						lod2_ebo->drawInstanced(GL_QUADS, 2*4, GL_UNSIGNED_INT, offset, lod2_typeIndices[i]);  
					}
				} // for each configuration
				glVertexAttribDivisor(ia2Loc1, 0);
				glVertexAttribDivisor(tm2Loc0, 0);
				glVertexAttribDivisor(tm2Loc0 + 1, 0);
				glVertexAttribDivisor(tm2Loc0 + 2, 0);
				glVertexAttribDivisor(tm2Loc0 + 3, 0);
				// disable all...
				glDisableVertexAttribArray(ia2Loc1);
				glDisableVertexAttribArray(tm2Loc0);
				glDisableVertexAttribArray(tm2Loc0 + 1);
				glDisableVertexAttribArray(tm2Loc0 + 2);
				glDisableVertexAttribArray(tm2Loc0 + 3);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				lod2_ebo->unbind();
				lod2_vbo->unbind(lod2_shader);
			
				leafNoiseTexture	->unbind();
				branchNoiseTexture  ->unbind();
				seasonMap			->unbind();
				lod2color1			->unbind();
				lod2color2			->unbind();
				lod2normal1			->unbind();
				lod2normal2			->unbind();
				lod2branch1			->unbind();
				lod2branch2			->unbind();
				g_shadowmap1		->unbind();
				// turn shader off
			lod2_shader->use(false);
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		} // if not drawing to shadowmap
	}
}

/*
void DTree::drawLOD0()
{
	if (g_draw_lod0){
		glColor4f(0.0, 0.0,0.0, alpha_c);
		
		glDisable(GL_CULL_FACE);
		glPushMatrix();
		glScalef( 10.f , 10.f, 10.f);
	
		// draw bbox
		//bbox->draw();


		// bind textures
		dataTexture->bind(GL_TEXTURE1);
		branchNoiseTexture->bind(GL_TEXTURE2);
		bColorTexture->bind(GL_TEXTURE4);

		// TODO: use positions

		// draw branches
		branchesEBO->draw(branchShader);

		bColorTexture->unbind();

		// bind textures
		leafNoiseTexture->bind(GL_TEXTURE3);
		frontDecalMap		->bind(GL_TEXTURE4);
		frontNormalMap		->bind(GL_TEXTURE5);
		frontTranslucencyMap->bind(GL_TEXTURE6);
		frontHalfLife2Map	->bind(GL_TEXTURE7);
		backDecalMap		->bind(GL_TEXTURE8);
		backNormalMap		->bind(GL_TEXTURE9);
		backTranslucencyMap	->bind(GL_TEXTURE10);
		backHalfLife2Map	->bind(GL_TEXTURE11);

		// TODO: use positions

		// draw leaves
		leavesVBO->draw(leafShader, GL_QUADS, 0);


		leafNoiseTexture->unbind();
		frontDecalMap		->unbind();
		frontNormalMap		->unbind();
		frontTranslucencyMap->unbind();
		frontHalfLife2Map	->unbind();
		backDecalMap		->unbind();
		backNormalMap		->unbind();
		backTranslucencyMap	->unbind();
		backHalfLife2Map	->unbind();
		branchNoiseTexture->unbind();
		dataTexture->unbind();
		glPopMatrix();
		glEnable(GL_CULL_FACE);
	}
}

void DTree::drawLOD1()
{
	if (g_draw_lod1){
		glColor4f(0.0, 0.0,0.0, alpha_c);
		
		int i, j, sliceCount, setCount=sliceSets.size();
		Texture * colorTexture, * dataTexture, *displacementTexture, *displacement2Texture, *normalTexture;
			displacementTexture		= leafNoiseTexture;
			displacement2Texture	= branchNoiseTexture;
			DTreeSlice * slice;
		for (j = 0; j< setCount; j++){
			sliceCount = sliceSets[j]->size();
			// if discrepacy angle under treshold, DO NOT write in z-buffer
			v3 p = position;
			v3 v = *viewer_position;
			p.y = 0.0;
			v.y = 0.0;
			v3 eyeDir = (p-v).getNormalized();
			v3 normalDir = sliceSets[j]->getNormal();
			normalDir.rotateY(rotationY*DEG_TO_RAD);
			float discrepacy = abs(normalDir.dot(eyeDir));
			//printf("sliceset[%i] DISC: %f\n",j, discrepacy);
			for (i=0; i<sliceCount; i++){
				slice					= sliceSets[j]->getSlice(i);
				colorTexture			= slice->colormap;
				//printf("sliceset: %i, slice: %i is using: glTextureID %i\n", j,i, colorTexture->id);
				normalTexture			= slice->normalmap;
				dataTexture				= slice->datamap;

				glPushMatrix();
					
					//l3dBillboardCheatCylindricalBegin();
			
					glRotatef(sliceSets[j]->rotation_y, 0.0, 1.0, 0.0);
					glTranslatef((i-(float(sliceCount)/2.f) + 0.5), 0.0, 0.0);
					glScalef(10.0,10.0,-10.0);
					//glRotatef(90, 0.0, 0.0, 1.0);
					glRotatef(-90.0, 0.0, 1.0, 0.0);
					glDisable(GL_CULL_FACE);
					glDisable(GL_LIGHTING);
					colorTexture		->bind(GL_TEXTURE0);
					displacementTexture	->bind(GL_TEXTURE1);
					displacement2Texture	->bind(GL_TEXTURE4);
					dataTexture			->bind(GL_TEXTURE2);
					normalTexture		->bind(GL_TEXTURE3);

					u_time_offset->data = &	g_tree_time_offset_1;
					// turn on shader
					lod1shader->use(true);
					lod1shader->setTexture(l_color	, colorTexture			->textureUnitNumber	);
					lod1shader->setTexture(l_displ	, displacementTexture	->textureUnitNumber	);
					lod1shader->setTexture(l_displ2	, displacement2Texture	->textureUnitNumber	);
					lod1shader->setTexture(l_data	, dataTexture			->textureUnitNumber	);
					lod1shader->setTexture(l_normal	, normalTexture			->textureUnitNumber	);
			
				

					// TODO: draw at all positions
					if (discrepacy<0.8){
						glDepthMask(GL_FALSE);
						lod1vbo->draw(lod1shader, GL_QUADS, 0);
						glDepthMask(GL_TRUE);
					} else {
						lod1vbo->draw(lod1shader, GL_QUADS, 0);
					}
				
					colorTexture		->unbind();
					displacementTexture	->unbind();
					displacement2Texture	->unbind();
					dataTexture			->unbind();
					normalTexture		->unbind();
					// turn off shader
					lod1shader->use(false);
					glEnable(GL_CULL_FACE);
					glEnable(GL_LIGHTING);
				glPopMatrix();
			} // for i - each slice
		} // for j - sliceSets
	}
}

void DTree::drawLOD1b()
{
	if (g_draw_lod1){
		glColor4f(0.0, 0.0,0.0, alpha_c);
		g_tree_lod1_count++;
		int i, j, sliceCount, setCount=lod1_sliceSets.size();
		Texture * colorTexture, * dataTexture, *displacementTexture, *displacement2Texture, *normalTexture;
		displacementTexture		=	leafNoiseTexture;
		displacement2Texture	=	branchNoiseTexture;
		
		colorTexture			=	lod1_jColorMap;
		dataTexture				=	lod1_jDataMap;
		normalTexture			=	lod1_jNormalMap;

		glPushMatrix();
					
			//l3dBillboardCheatCylindricalBegin();
			
			//glRotatef(90, 0.0, 1.0, 0.0);
			glDisable(GL_CULL_FACE);
			glDisable(GL_LIGHTING);
			
			
			//v3 dirA = v3( -1.0, 0.0, 0.0);									// A
			//v3 rightA = dirA.getRotated( 90*DEG_TO_RAD, v3(0.0, 1.0, 0.0));
			//v3 dirB = dirA.getRotated(  60*DEG_TO_RAD, v3(0.0, 1.0, 0.0));	// B
			//v3 rightB = dirB.getRotated( 90*DEG_TO_RAD, v3(0.0, 1.0, 0.0));
			//v3 dirC = dirA.getRotated( -60*DEG_TO_RAD, v3(0.0, 1.0, 0.0));	// C
			//v3 rightC = dirC.getRotated( 90*DEG_TO_RAD, v3(0.0, 1.0, 0.0));
			//float short_l = 0.1, long_l = 0.3;
			//glPushMatrix();
			//	glTranslatef(0.0, 0.1, 0.0);
			//	glBegin(GL_LINE_STRIP);
			//		glColor4f(1.0, 0.0,0.0, alpha_c);
			//		glVertex3f(short_l*rightA.x, short_l*rightA.y, short_l*rightA.z); glVertex3f(0.0, .0, .0); glVertex3f(long_l*dirA.x, long_l*dirA.y, long_l*dirA.z);
			//	glEnd();
			//	glBegin(GL_LINE_STRIP);
			//		glColor4f(0.0, 1.0,0.0, alpha_c);
			//		glVertex3f(short_l*rightB.x, short_l*rightB.y, short_l*rightB.z); glVertex3f(0.0, .0, .0); glVertex3f(long_l*dirB.x, long_l*dirB.y, long_l*dirB.z);
			//	glEnd();
			//	glBegin(GL_LINE_STRIP);
			//		glColor4f(0.0, 0.0, 1.0, alpha_c);
			//		glVertex3f(short_l*rightC.x, short_l*rightC.y, short_l*rightC.z); glVertex3f(0.0, .0, .0); glVertex3f(long_l*dirC.x, long_l*dirC.y, long_l*dirC.z);
			//	glEnd();

			//	glBegin(GL_LINES);
			//		glColor4f(1.0, 1.0, 1.0, alpha_c);
			//		glVertex3f(0.0, 0.0, 0.0); glVertex3f(eyeDir.x,eyeDir.y, eyeDir.z);
			//	glEnd();
			//glPopMatrix();
			
			lod1_shader->use(true);
			lod1_jColorMap			->bind(GL_TEXTURE1);
			displacementTexture	->bind(GL_TEXTURE2);
			displacement2Texture->bind(GL_TEXTURE3);
			lod1_jDataMap			->bind(GL_TEXTURE4);
			lod1_jNormalMap			->bind(GL_TEXTURE5);

			//u_time_offset->data = &	g_tree_time_offset_1;
	
			//printf("SETTING TEXTURE[%s] loc:%i, unit id: %i\n", colorTexture->inShaderName.c_str(),l2_color, colorTexture->textureUnitNumber);
			lod1_shader->setTexture(l2_color	, lod1_jColorMap				->textureUnitNumber	);
			//printf("SETTING TEXTURE[%s] loc:%i, unit id: %i\n", displacementTexture->inShaderName.c_str(),l2_displ, displacementTexture->textureUnitNumber);
			lod1_shader->setTexture(l2_displ	, displacementTexture	->textureUnitNumber	);			
			//printf("SETTING TEXTURE[%s] loc:%i, unit id: %i\n", displacement2Texture->inShaderName.c_str(),l2_displ2, displacement2Texture->textureUnitNumber);
			lod1_shader->setTexture(l2_displ2	, displacement2Texture	->textureUnitNumber	);
			//printf("SETTING TEXTURE[%s] loc:%i, unit id: %i\n", dataTexture->inShaderName.c_str(),l2_data, dataTexture->textureUnitNumber);
			lod1_shader->setTexture(l2_data		, lod1_jDataMap				->textureUnitNumber	);
			//printf("SETTING TEXTURE[%s] loc:%i, unit id: %i\n", normalTexture->inShaderName.c_str(),l2_normal, normalTexture->textureUnitNumber);
			lod1_shader->setTexture(l2_normal	, lod1_jNormalMap			->textureUnitNumber	);
			//glDepthMask(GL_FALSE);
			lod1_vbo->bind(lod1_shader);

			// draw EBO...
			lod1_ebo->bind();

			// for each instance

			for (int i = 0; i < tree_instances.size(); i++){
				// determine LOD configuration
				position = tree_instances[i]->position;
				rotationY= tree_instances[i]->rotation_y;
				glPushMatrix();
					glTranslate(position);
					glRotatef(rotationY, 0.0, 1.0 ,0.0);
					glScalef(10.0, 10.0, 10.0);
					v3 p = position;
					v3 v = *viewer_position;
					p.y = 0.0;
					v.y = 0.0;
					v3 eyeDir = -(p-v).getNormalized();	
					v3 dirA = v3( -1.0, 0.0, 0.0).getRotated( (rotationY)*DEG_TO_RAD, v3(0.0, 1.0, 0.0));	// A									// A
					v3 dirB = dirA.getRotated( 60 * DEG_TO_RAD, v3(0.0, 1.0, 0.0));							// B
					//v3 dirC = dirA.getRotated( -60 * DEG_TO_RAD, v3(0.0, 1.0, 0.0));						// C
				
					float cosA = dirA.dot(eyeDir);
					float cosB = dirB.dot(eyeDir);
					//float cosC = dirC.dot(eyeDir);
					int o = 1; 
					if (cosA<= -0.86 || cosA>=0.86){
						o = 0;
					} else if (cosB<= -0.86 || cosB>=0.86){
						o = 2;
					}
					int off = o*(4*3*3*sizeof(unsigned int));
					void * offset = BUFFER_OFFSET(off);
					lod1_ebo->draw(GL_UNSIGNED_INT, GL_QUADS, 3*3*4, offset);
				glPopMatrix();
			} // for each position
			
			lod1_ebo->unbind();

			lod1_vbo->unbind(lod1_shader);
			//lod1_vbo->draw(lod1_shader, GL_QUADS, 0);
			//glDepthMask(GL_TRUE);
			colorTexture		->unbind();
			displacementTexture	->unbind();
			displacement2Texture->unbind();
			dataTexture			->unbind();
			normalTexture		->unbind();
			// turn off shader
			lod1_shader->use(false);
			
			glEnable(GL_CULL_FACE);
			glEnable(GL_LIGHTING);
		
		glPopMatrix();
	}
}

void DTree::drawLOD2()
{
	if (g_draw_lod2){
		glColor4f(0.0, 0.0,0.0, alpha_c);
		g_tree_lod2_count++;
		int i, j, sliceCount, setCount=sliceSets.size();
		Texture * colorTexture, * dataTexture, *displacementTexture, *displacement2Texture, *normalTexture;
			displacementTexture		= leafNoiseTexture;
			displacement2Texture	= branchNoiseTexture;
			DTreeSlice * slice;
		for (j = 0; j< setCount; j++){
			// if discrepacy angle under treshold, DO NOT write in z-buffer
			v3 p = position;
			v3 v = *viewer_position;
			p.y = 0.0;
			v.y = 0.0;
			v3 eyeDir = (p-v).getNormalized();
			v3 normalDir = sliceSets[j]->getNormal();
			normalDir.rotateY(rotationY*DEG_TO_RAD);
			float discrepacy = abs(normalDir.dot(eyeDir));

			sliceCount = sliceSets[j]->size();
			i = sliceCount/2;
				slice					= sliceSets[j]->getSlice(i);
				colorTexture			= slice->colormap;
				normalTexture			= slice->normalmap;
				dataTexture				= slice->datamap;
				glPushMatrix();	
					//l3dBillboardCheatCylindricalBegin();
			
					glRotatef(j*90, 0.0, 1.0, 0.0);
					glTranslatef((i-(float(sliceCount)/2.f) + 0.5), 0.0, 0.0);
					glScalef(10.0,10.0,-10.0);
					//glRotatef(90, 0.0, 0.0, 1.0);
					glRotatef(-90, 0.0, 1.0, 0.0);
					glDisable(GL_CULL_FACE);
					colorTexture		->bind(GL_TEXTURE0);
					displacementTexture	->bind(GL_TEXTURE1);
					displacement2Texture->bind(GL_TEXTURE4);
					dataTexture			->bind(GL_TEXTURE2);
					normalTexture		->bind(GL_TEXTURE3);

					u_time_offset->data = &	g_tree_time_offset_1;
					// turn on shader
					lod1shader->use(true);
					lod1shader->setTexture(l_color	, colorTexture			->textureUnitNumber	);
					lod1shader->setTexture(l_displ	, displacementTexture	->textureUnitNumber	);
					lod1shader->setTexture(l_displ2	, displacement2Texture	->textureUnitNumber	);
					lod1shader->setTexture(l_data	, dataTexture			->textureUnitNumber	);
					lod1shader->setTexture(l_normal	, normalTexture			->textureUnitNumber	);
			
					// TODO: draw at all positions
					if (discrepacy<0.8){
						glDepthMask(GL_FALSE);
						lod1vbo->draw(lod1shader, GL_QUADS, 0);
						glDepthMask(GL_TRUE);
					} else {
						lod1vbo->draw(lod1shader, GL_QUADS, 0);
					}

					colorTexture		->unbind();
					displacementTexture	->unbind();
					displacement2Texture->unbind();
					dataTexture			->unbind();
					normalTexture		->unbind();
					// turn off shader
					lod1shader->use(false);
					glEnable(GL_CULL_FACE);

				glPopMatrix();
		} // for j - sliceSets
	}
}

void DTree::drawNormals(DTreeInstanceData* instance){
	glColor4f(1.0,1.0,1.0, instance->alpha);
		
	glDisable(GL_CULL_FACE);
	glPushMatrix();
	glTranslatef(instance->position.x, instance->position.y, instance->position.z);
	glRotatef(instance->rotation_y+180, 0.0, 1.0, 0.0);
	glScalef( 10.f , 10.f, 10.f);
	
	// draw bbox
	//bbox->draw();


	// bind textures
	dataTexture->bind(GL_TEXTURE1);
	branchNoiseTexture->bind(GL_TEXTURE2);
	bColorTexture->bind(GL_TEXTURE4);

	// TODO: use positions

	// draw branches
	//branchesEBO->draw(n_branchShader);

	
	leafNoiseTexture->bind(GL_TEXTURE3);
	frontDecalMap		->bind(GL_TEXTURE4);
	frontNormalMap		->bind(GL_TEXTURE5);
	frontTranslucencyMap->bind(GL_TEXTURE6);
	frontHalfLife2Map	->bind(GL_TEXTURE7);
	backDecalMap		->bind(GL_TEXTURE8);
	backNormalMap		->bind(GL_TEXTURE9);
	backTranslucencyMap	->bind(GL_TEXTURE10);
	backHalfLife2Map	->bind(GL_TEXTURE11);

	// TODO: use positions

	// draw leaves
	leavesVBO->draw(n_leafShader, GL_QUADS, 0);


	leafNoiseTexture->unbind();
	frontDecalMap		->unbind();
	frontNormalMap		->unbind();
	frontTranslucencyMap->unbind();
	frontHalfLife2Map	->unbind();
	backDecalMap		->unbind();
	backNormalMap		->unbind();
	backTranslucencyMap	->unbind();
	backHalfLife2Map	->unbind();
	branchNoiseTexture->unbind();
	dataTexture->unbind();
	
	glPopMatrix();
	glEnable(GL_CULL_FACE);
}
*/
// 
void DTree::drawForLOD(){
	// TODO: optimize rendering for LOD slice generation...
	glDisable(GL_CULL_FACE);
	// draw bbox
	//bbox->draw();

	glDisable(GL_LIGHTING);
	// bind textures
	dataTexture->bind(GL_TEXTURE1);
	branchNoiseTexture->bind(GL_TEXTURE2);
	bColorTexture->bind(GL_TEXTURE4);

	// draw branches
	branchesEBO->draw(bLODShader);

	bColorTexture->unbind();

	// bind textures
	leafNoiseTexture->bind(GL_TEXTURE3);
	frontDecalMap		->bind(GL_TEXTURE4);
	frontNormalMap		->bind(GL_TEXTURE5);
	frontTranslucencyMap->bind(GL_TEXTURE6);
	frontHalfLife2Map	->bind(GL_TEXTURE7);
	backDecalMap		->bind(GL_TEXTURE8);
	backNormalMap		->bind(GL_TEXTURE9);
	backTranslucencyMap	->bind(GL_TEXTURE10);
	backHalfLife2Map	->bind(GL_TEXTURE11);



	// draw leaves
	leavesVBO->draw(lLODShader, GL_QUADS, 0);


	leafNoiseTexture->unbind();
	frontDecalMap		->unbind();
	frontNormalMap		->unbind();
	frontTranslucencyMap->unbind();
	frontHalfLife2Map	->unbind();
	backDecalMap		->unbind();
	backNormalMap		->unbind();
	backTranslucencyMap	->unbind();
	backHalfLife2Map	->unbind();
	branchNoiseTexture->unbind();
	dataTexture->unbind();
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

}

void DTree::prepareForRender(){
	countRenderQueues[0] = 0;
	countRenderQueues[1] = 0;
	countRenderQueues[2] = 0;
	countRenderQueues[3] = 0;
	countRenderQueues[4] = 0;
	// go through instances and put them in proper render pipelines
	// TODO: this part can be PARALLEL to the rendering thread...
	DTreeInstanceData * instance;
	int i;
	for (i=0; i<lod1_typeIndices.size(); i++){
		lod1_typeIndices[i] = 0;
	}
	for (i=0; i<lod2_typeIndices.size(); i++){
		lod2_typeIndices[i] = 0;
	}
	int instanceCount = tree_instances.size();

	if (instanceCount<=0){
		return;
	}
	DTreeInstanceData * act_instance = tree_instances[0] ;
	DTreeInstanceData * next_instance;
	act_instance->eye_dir =  act_instance->position - *g_viewer_position;
	act_instance->distance = act_instance->eye_dir.length();
	act_instance->eye_dir.normalize();
	act_instance->discrepacy = g_viewer_direction->dot(act_instance->eye_dir);
	
	DTreeInstanceData* done_instance;
	// go through instances, perform one bubble sort walktrough & split into different render queues
	DTreeInstanceData* rest_instance = act_instance;
	//int i=1;
	for (int i=1; i<tree_instances.size(); i++){
		next_instance = tree_instances[i];
		next_instance->eye_dir = next_instance->position - *g_viewer_position;
		next_instance->distance= next_instance->eye_dir.length();
		next_instance->eye_dir.normalize();
		next_instance->discrepacy = g_viewer_direction->dot(next_instance->eye_dir);
		bool swap;
		switch (g_sorting){
			case SORT_FRONT_TO_BACK:
				swap = next_instance->distance < act_instance->distance;
			break;
			case SORT_BACK_TO_FRONT:
				swap = next_instance->distance > act_instance->distance;
			break;
			default:
				swap = false;
		}
		
		if (swap){
			// swap
			tree_instances[i-1]	= next_instance;
			tree_instances[i-1]	->index = i-1;
			tree_instances[i]	= act_instance;
			tree_instances[i]	->index = i;
			done_instance		= next_instance;
			//if (i==1){ done_instance=NULL; }
			rest_instance		= act_instance;
		} else {
			done_instance	= act_instance;
			act_instance	= next_instance;
			rest_instance	= act_instance;
		}
		// draw only instances near the view angle
		enqueueInRenderList(done_instance);
	} // loop over instances
	if (rest_instance!=NULL){
		enqueueInRenderList(rest_instance);
	}
}

void DTree::makeTransition(float control, bool maskOld, DTreeInstanceData* instance, void (DTree::*drawLODA)(DTreeInstanceData*, float),  void (DTree::*drawLODB)(DTreeInstanceData*, float)){
	float shift;
	float c;
	float a1;
	float a2;
	g_transitionControl = control;
	switch (g_lodTransition){
			case LODTransitionMethod::HARD_SWITCH:
				if (control<0.5){
					a2 = 1.0;
					a1 = 0.0;
				} else {
					a2 = 0.0;
					a1 = 1.0;
				}
				break;
			case LODTransitionMethod::CROSS_FADE:
				a1 = control;
				a2 = 1.0-control;			
				break;
			case LODTransitionMethod::FADE_IN_BACKGROUND:
				if (control<0.5){
					a1 = 2*control;
					a2 = 1.0;			
				} else {			
					a2 = 2.0*(1.0-control);
					a1 = 1.0;			
				}
				break;
			case LODTransitionMethod::SHIFTED_CROSS_FADE:
				shift = g_transitionShift;
				c = 1.0f/(1.f - shift);
				a1 = min(1.f, max(0.f , control*c));
				a2 = min(1.f, max(0.f, 1.f-(control-shift)*c));
				break;
			case LODTransitionMethod::SHIFTED_SOFT_FADE:
				shift = g_transitionShift;
				c = 1.0f/(1.f - shift);
				a1 = 1.f - (smoothTransitionCos(c*(control)*PI)*0.5f + 0.5f);
				a2 = smoothTransitionCos(c*(control-shift)*PI)*0.5f + 0.5f;
				break;
		}

		if (maskOld){
			// swap drawing commands
			//void (DTree::*drawLODempty)(DTreeInstanceData*, float) = drawLODA;
			//drawLODA = drawLODB;
			//drawLODB = drawLODempty;
		}


		if (control<0.5){
			// show LOD B 
			if (!g_Draw2Shadowmap){
				glDepthMask(GL_FALSE);
				(this->*drawLODB)(instance, a1);
				glDepthMask(GL_TRUE);
			} else {
				(this->*drawLODB)(instance, a1);
			}

			// show LOD A						
			(this->*drawLODA)(instance, a2);				
			
		} else {		
			// show LOD B			
			(this->*drawLODB)(instance, a1);
			// show LOD A
			if (!g_Draw2Shadowmap){
				glDepthMask(GL_FALSE);
				(this->*drawLODA)(instance, a2);
				glDepthMask(GL_TRUE);
			} else {
				(this->*drawLODA)(instance, a2);
			}
			
		}


}

void DTree::render(){
	glBeginQuery(GL_SAMPLES_PASSED, samples_query_id1);
	int i;
	DTreeInstanceData* instance;

	// render LOD2 instances
	isInstancingEnabled = true;
	draw_all_instances_LOD2b();

	// render LOD1-LOD2 transitioning instances
	isInstancingEnabled = false;
	for (i=0; i<countRenderQueues[3]; i++){
		instance = instancesInRenderQueues[3][i];
		float alpha = instance->alpha;
		makeTransition(alpha,true, instance, &DTree::draw_instance_LOD1, &DTree::draw_instance_LOD2b);
	}
	// render LOD1 instances
	isInstancingEnabled = true;
	draw_all_instances_LOD1();

	// render LOD0-LOD1 transitioning instances
	isInstancingEnabled = false;
	for (i=0; i<countRenderQueues[1]; i++){
		instance = instancesInRenderQueues[1][i];
		float alpha = instance->alpha;
		makeTransition(alpha,false, instance, &DTree::draw_instance_LOD0, &DTree::draw_instance_LOD1);
	}
	
	// render LOD0 instances
	isInstancingEnabled = false;
	for (i=0; i<countRenderQueues[0]; i++){
		instance = instancesInRenderQueues[0][i];
		// show LOD 0
		g_transitionControl = 0.0;
		//drawForLOD();
		draw_instance_LOD0(instance, 1.0);	
	}
	glEndQuery(GL_SAMPLES_PASSED);


	GLint avail = false;
	glGetQueryObjectiv(samples_query_id1, GL_QUERY_RESULT_AVAILABLE, & avail );
	while( avail != GL_TRUE){ // blocking CPU
		glGetQueryObjectiv(samples_query_id1, GL_QUERY_RESULT_AVAILABLE, & avail );
	}
	GLuint64 fragments = 0;
	glGetQueryObjectui64v(samples_query_id1, GL_QUERY_RESULT, &fragments); 
	samples_lod12 = (double)fragments;
	g_tree_lod0_count	= countRenderQueues[0];
	g_tree_lod01_count	= countRenderQueues[1];	 
	g_tree_lod1_count	= countRenderQueues[2];
	g_tree_lod12_count	= countRenderQueues[3];	 
	g_tree_lod2_count	= countRenderQueues[4];
}

void DTree::draw(){
	if (!instancesLocked){
		prepareForRender();
		render();	
	}
}

void DTree::enqueueInRenderList(DTreeInstanceData * instance){
	if (instance!=NULL){
		// calc wind dir;
		if (g_wind_dirty){
			instance->wind_dir = g_tree_wind_direction.getRotatedY(-DEG_TO_RAD*(instance->rotation_y+90));
			//instance->wind_dir = g_tree_wind_direction;//.getRotatedY(-DEG_TO_RAD*instance->rotation_y);
		}

		int LODindex = 0;
		if (instance->distance<=g_lodTresholds.x){
			//	LOD0
			g_tree_lod0_count++;
			instance->alpha = 1.0;
			instancesInRenderQueues[LODindex][countRenderQueues[LODindex]] = instance;
			countRenderQueues[LODindex]+=1;
			return;
		}
	
		if(instance->discrepacy>0.5){
			instance->dirA.y = 0.0;
			instance->dirB.y = 0.0;
			instance->eye_dir.y = 0.0;
			instance->eye_dir.normalize();
			// determine LOD pipeline
		
			if (instance->distance>g_lodTresholds.w){
				// LOD 2
				// TODO: instanced drawing of LOD2
				g_tree_lod2_count++;
				LODindex = 4;
				instance->offset = 1;
				
				float cosA = instance->dirA.dot(instance->eye_dir);
				// 45° treshold
				if (cosA<= -0.70 || cosA>= 0.70){
					// varianta A
					instance->offset = 0;
				}
				memcpy( lod2_instanceMatrices[instance->offset] + lod2_typeIndices[instance->offset]*instanceFloatCount,
						instance->transformMatrix.m,
						16*sizeof(float)
					  );
					// instance attribs
				memcpy( lod2_instanceMatrices[instance->offset] + lod2_typeIndices[instance->offset]*instanceFloatCount + 16,
						instance->colorVariance.data,
						3*sizeof(float)
					  );
				lod2_instanceMatrices[instance->offset][lod2_typeIndices[instance->offset]*instanceFloatCount + 19] = instance->time_offset;
				lod2_typeIndices[instance->offset] += 1;
			} else if (instance->distance>g_lodTresholds.z){
				// transition between LOD1-LOD2
				g_tree_lod12_count++;
				// TODO: direct drawing of LOD2 & LOD1
				LODindex = 3;
				float cosA = instance->dirA.dot(instance->eye_dir);
				// 45° treshold
				if (cosA<= -0.70 || cosA>= 0.70){
					// varianta A
					instance->offset = 0;
				} else {
					// varianta B
					instance->offset = 1;
				}
				instance->alpha = (instance->distance - g_lodTresholds.z) / (g_lodTresholds.w - g_lodTresholds.z);

			} else
			if (instance->distance>g_lodTresholds.y){
				g_tree_lod1_count++;
				LODindex = 2;
				// LOD1
				// determine the order of slices to render
				float cosA = instance->dirA.dot(instance->eye_dir);
				float cosB = instance->dirB.dot(instance->eye_dir);
				instance->offset = 1;
				if (cosA<= -0.86 || cosA>=0.86){
					// B
					instance->offset = 0;
				} else if (cosB<= -0.86 || cosB>=0.86){
					// C
					instance->offset = 2;
				} 
				// copy matrices (and other instance attributes to array for VBO)
					// matrices
				memcpy( lod1_instanceMatrices[instance->offset] + lod1_typeIndices[instance->offset]*instanceFloatCount,
						instance->transformMatrix.m,
						16*sizeof(float)
					  );
					// instance attribs
				memcpy( lod1_instanceMatrices[instance->offset] + lod1_typeIndices[instance->offset]*instanceFloatCount + 16,
						instance->colorVariance.data,
						3*sizeof(float)
					  );
				lod1_instanceMatrices[instance->offset][lod1_typeIndices[instance->offset]*instanceFloatCount + 19] = instance->time_offset;

				lod1_typeIndices[instance->offset] += 1;
			} else
			if (instance->distance>g_lodTresholds.x){
				//	transition between LOD0-LOD1
				g_tree_lod01_count++;
				LODindex = 1;
				// determine the order of slices to render
				float cosA = instance->dirA.dot(instance->eye_dir);
				float cosB = instance->dirB.dot(instance->eye_dir);
				instance->offset = 1;
				if (cosA<= -0.86 || cosA>=0.86){
					// B
					instance->offset = 0;
				} else if (cosB<= -0.86 || cosB>=0.86){
					// C
					instance->offset = 2;
				}
				// TODO: direct drawing of LOD0 & LOD1
				instance->alpha = (instance->distance - g_lodTresholds.x) / (g_lodTresholds.y - g_lodTresholds.x);
			}
			/*
			else {
				// LOD0
				LODindex = 0;
				instance->alpha = 1.0;
				//draw_instance_LOD0(instance);
			}*/
			instancesInRenderQueues[LODindex][countRenderQueues[LODindex]] = instance;
			countRenderQueues[LODindex]+=1;
		}
	} // not NULL instance
}

void DTree::initLOD0()
{
	printf("DYN_TREE init\n");
	// load textures
	// SEASON MAP
	seasonMap = new Texture("seasonMap");
	seasonMap->load(DYN_TREE::SEASON_MAP, false, false, GL_CLAMP, GL_LINEAR, GL_LINEAR);
	seasonMap->textureUnitNumber = 6;
	seasonMap->textureUnit = GL_TEXTURE0 + 6;

	// NOISE
	branchNoiseTexture = new Texture("branch_noise_tex"); 
	branchNoiseTexture->load(DYN_TREE::BRANCH_NOISE_TEXTURE, false, true, GL_REPEAT, GL_LINEAR, GL_LINEAR);
	leafNoiseTexture = new Texture("leaf_noise_tex");
	leafNoiseTexture->load(DYN_TREE::LEAF_NOISE_TEXTURE, false, true, GL_REPEAT, GL_LINEAR, GL_LINEAR);

	//  COLOR
	bColorTexture = new Texture("color_texture");
	bColorTexture->load(DYN_TREE::TEX_WOOD1, true, false , GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	//lColorTexture = new Texture("color_texture");
	//lColorTexture->load(DYN_TREE::TEX_LEAF1, true, false , GL_REPEAT, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);

	//frontDecalMap		=new Texture("color_texture");
	frontDecalMap		=new Texture("frontDecalMap");
	frontNormalMap		=new Texture("frontNormalMap");
	frontTranslucencyMap=new Texture("frontTranslucencyMap");
	frontHalfLife2Map	=new Texture("frontHalfLife2Map");
	backDecalMap		=new Texture("backDecalMap");
	backNormalMap		=new Texture("backNormalMap");
	backTranslucencyMap	=new Texture("backTranslucencyMap");
	backHalfLife2Map	=new Texture("backHalfLife2Map");
	frontDecalMap		->load(DYN_TREE::TEX_FDM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	frontNormalMap		->load(DYN_TREE::TEX_FNM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	frontTranslucencyMap->load(DYN_TREE::TEX_FTM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	frontHalfLife2Map	->load(DYN_TREE::TEX_FHM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	backDecalMap		->load(DYN_TREE::TEX_BDM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	backNormalMap		->load(DYN_TREE::TEX_BNM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	backTranslucencyMap	->load(DYN_TREE::TEX_BTM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);
	backHalfLife2Map	->load(DYN_TREE::TEX_BHM, true, false, GL_CLAMP, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR);



	// init shaders
	branchShader = new Shader("branch");
	branchShader->loadShader(DYN_TREE::SHADER_BRANCH_V,DYN_TREE::SHADER_BRANCH_F); 
	leafShader = new Shader("leaf");
	leafShader->loadShader(DYN_TREE::SHADER_LEAF_V,DYN_TREE::SHADER_LEAF_F); 
	// shadow shaders
	branchShader_sh = new Shader("branch_shadow");
	branchShader_sh->loadShader(DYN_TREE::SHADER_BRANCH_SH_V,DYN_TREE::SHADER_BRANCH_SH_F);
	leafShader_sh = new Shader("leaf_shadow");
	leafShader_sh->loadShader(DYN_TREE::SHADER_LEAF_SH_V,DYN_TREE::SHADER_LEAF_SH_F); 
		
	
	// LOD shaders
	bLODShader = new Shader("LODbranch");
	bLODShader->loadShader(DYN_TREE::SHADER_BRANCH_LOD_V,DYN_TREE::SHADER_BRANCH_LOD_F); 
	lLODShader = new Shader("LODleaf");
	lLODShader->loadShader(DYN_TREE::SHADER_LEAF_LOD_V,DYN_TREE::SHADER_LEAF_LOD_F); 
	 

	// connect textures with shader
	branchShader->linkTexture(branchNoiseTexture);
	branchShader->linkTexture(bColorTexture);
	branchShader->linkTexture(g_shadowmap1);
	branchShader_sh->linkTexture(branchNoiseTexture	);
	branchShader_sh->linkTexture(bColorTexture		);

	bLODShader->linkTexture(branchNoiseTexture);
	bLODShader->linkTexture(bColorTexture);

	leafShader->linkTexture(branchNoiseTexture);
	leafShader->linkTexture(leafNoiseTexture);
	//leafShader->linkTexture(lColorTexture);
	leafShader->linkTexture(g_shadowmap1);
	leafShader->linkTexture(frontDecalMap		);
	leafShader->linkTexture(frontNormalMap		);
	leafShader->linkTexture(frontTranslucencyMap);
	leafShader->linkTexture(frontHalfLife2Map	);
	leafShader->linkTexture(backDecalMap		);
	leafShader->linkTexture(backNormalMap		);
	leafShader->linkTexture(backTranslucencyMap	);
	leafShader->linkTexture(backHalfLife2Map	);
	leafShader->linkTexture(seasonMap			);

	leafShader_sh->linkTexture(frontDecalMap		);
	leafShader_sh->linkTexture(seasonMap			);
	leafShader_sh->linkTexture(branchNoiseTexture	);
	leafShader_sh->linkTexture(leafNoiseTexture		);

	lLODShader->linkTexture(branchNoiseTexture	);
	lLODShader->linkTexture(leafNoiseTexture	);
	//lLODShader->linkTexture(lColorTexture);
	lLODShader->linkTexture(frontDecalMap		);
	lLODShader->linkTexture(frontNormalMap		);
	lLODShader->linkTexture(frontTranslucencyMap);
	lLODShader->linkTexture(frontHalfLife2Map	);
	lLODShader->linkTexture(backDecalMap		);
	lLODShader->linkTexture(backNormalMap		);
	lLODShader->linkTexture(backTranslucencyMap	);
	lLODShader->linkTexture(backHalfLife2Map	);
	lLODShader->linkTexture(seasonMap			);


	// register uniforms
	this->branchCountF = 0;
	branchShader->registerUniform("shadowmap_size",			UniformType::F2,	& g_ShadowMapSize.data);
	branchShader->registerUniform("samples",				UniformType::I1,	& g_shadowMapSamples);
	
	branchShader->registerUniform("branch_count",			UniformType::F1,	& this->branchCountF);
	branchShader->registerUniform("time",					UniformType::F1,	& g_float_time);
	branchShader->registerUniform("time_offset",			UniformType::F1,	& time_offset);	
	branchShader->registerUniform("wind_direction",			UniformType::F3,	& wind_relative_direction.data);
	branchShader->registerUniform("wind_strength",			UniformType::F1,	& g_tree_wind_strength);
	branchShader->registerUniform("wood_amplitudes",		UniformType::F4,	& g_tree_wood_amplitudes.data);
	branchShader->registerUniform("wood_frequencies",		UniformType::F4,	& g_tree_wood_frequencies.data);
	branchShader->registerUniform("window_size",			UniformType::F2,	& g_window_sizes.data);		
	branchShader->registerUniform("LightMVPCameraVInverseMatrix",			UniformType::M4,	g_LightMVPCameraVInverseMatrix);
	branchShader->registerUniform("shadowMappingEnabled",	UniformType::B1,	& g_ShadowMappingEnabled);
	branchShader->registerUniform("transition_control",		UniformType::F1,	& g_transitionControl);		
	branchShader_sh->registerUniform("branch_count",		UniformType::F1,	& this->branchCountF);
	branchShader_sh->registerUniform("time",				UniformType::F1,	& g_float_time);
	branchShader_sh->registerUniform("time_offset",			UniformType::F1,	& time_offset);	
	branchShader_sh->registerUniform("wind_direction",		UniformType::F3,	& g_tree_wind_direction.data);
	branchShader_sh->registerUniform("wind_strength",		UniformType::F1,	& g_tree_wind_strength);
	branchShader_sh->registerUniform("wood_amplitudes",		UniformType::F4,	& g_tree_wood_amplitudes.data);
	branchShader_sh->registerUniform("wood_frequencies",	UniformType::F4,	& g_tree_wood_frequencies.data);
	branchShader_sh->registerUniform("window_size",			UniformType::F2,	& g_window_sizes.data);		
	branchShader_sh->registerUniform("transition_control",	UniformType::F1,	& g_transitionControl);		

	bLODShader->registerUniform("branch_count",				UniformType::F1,	& this->branchCountF);
	bLODShader->registerUniform("time",						UniformType::F1,	& g_float_time);
	bLODShader->registerUniform("time_offset",				UniformType::F1,	& time_offset);
	bLODShader->registerUniform("wind_direction",			UniformType::F3,	& wind_relative_direction.data);
	bLODShader->registerUniform("wind_strength",			UniformType::F1,	& g_tree_wind_strength);
	bLODShader->registerUniform("wood_amplitudes",			UniformType::F4,	& g_tree_wood_amplitudes.data);
	bLODShader->registerUniform("wood_frequencies",			UniformType::F4,	& g_tree_wood_frequencies.data);
	bLODShader->registerUniform("window_size",				UniformType::F2,	& g_window_sizes.data);		

	//branchShader->registerUniform("leaf_amplitude", UniformType::F1, & g_tree_wood_amplitudes.data);
	//branchShader->registerUniform("leaf_frequency", UniformType::F1, & g_tree_wood_amplitudes.data);

	leafShader->registerUniform("shadowmap_size",			UniformType::F2,	& g_ShadowMapSize.data);
	leafShader->registerUniform("samples",					UniformType::I1,	& g_shadowMapSamples);
	
	leafShader->registerUniform("branch_count",				UniformType::F1,	& this->branchCountF);
	leafShader->registerUniform("time",						UniformType::F1,	& g_float_time);
	leafShader->registerUniform("time_offset",				UniformType::F1,	& time_offset);
	leafShader->registerUniform("wind_direction",			UniformType::F3,	& wind_relative_direction.data);
	leafShader->registerUniform("wind_strength",			UniformType::F1,	& g_tree_wind_strength);
	leafShader->registerUniform("wood_amplitudes",			UniformType::F4,	& g_tree_wood_amplitudes.data);
	leafShader->registerUniform("wood_frequencies",			UniformType::F4,	& g_tree_wood_frequencies.data);
	leafShader->registerUniform("leaf_amplitude",			UniformType::F1,	& g_tree_leaf_amplitude);
	leafShader->registerUniform("leaf_frequency",			UniformType::F1,	& g_tree_leaf_frequency);
	leafShader->registerUniform("MultiplyAmbient",			UniformType::F1,	& g_leaves_MultiplyAmbient);
	leafShader->registerUniform("MultiplyDiffuse",			UniformType::F1,	& g_leaves_MultiplyDiffuse);
	leafShader->registerUniform("MultiplySpecular",			UniformType::F1,	& g_leaves_MultiplySpecular);
	leafShader->registerUniform("MultiplyTranslucency",		UniformType::F1,	& g_leaves_MultiplyTranslucency);
	leafShader->registerUniform("ReduceTranslucencyInShadow", UniformType::F1,	& g_leaves_ReduceTranslucencyInShadow);
	leafShader->registerUniform("shadow_intensity",			UniformType::F1,	& g_leaves_shadow_intensity);
	leafShader->registerUniform("LightDiffuseColor",		UniformType::F3,	& g_leaves_LightDiffuseColor.data);
	leafShader->registerUniform("window_size",				UniformType::F2,	& g_window_sizes.data);		
	leafShader->registerUniform("season",					UniformType::F1,	& g_season);		
	leafShader->registerUniform("transition_control",		UniformType::F1,	& g_transitionControl);		
	leafShader->registerUniform("LightMVPCameraVInverseMatrix",			UniformType::M4,	g_LightMVPCameraVInverseMatrix);
	leafShader->registerUniform("shadowMappingEnabled",		UniformType::B1,	& g_ShadowMappingEnabled);


	leafShader->registerUniform("gauss_shift",				UniformType::F1,	& g_gauss_shift);		
	leafShader->registerUniform("gauss_steep",				UniformType::F1,	& g_gauss_steep);		
	leafShader->registerUniform("gauss_weight",				UniformType::F1,	& g_gauss_weight);		
	


	iu0Loc1 = leafShader->getLocation("colorVariance");

	leafShader_sh->registerUniform("branch_count",				UniformType::F1,	& this->branchCountF);
	leafShader_sh->registerUniform("time",						UniformType::F1,	& g_float_time);
	leafShader_sh->registerUniform("time_offset",				UniformType::F1,	& time_offset);
	leafShader_sh->registerUniform("wind_direction",			UniformType::F3,	& wind_relative_direction.data);
	leafShader_sh->registerUniform("wind_strength",				UniformType::F1,	& g_tree_wind_strength);
	leafShader_sh->registerUniform("wood_amplitudes",			UniformType::F4,	& g_tree_wood_amplitudes.data);
	leafShader_sh->registerUniform("wood_frequencies",			UniformType::F4,	& g_tree_wood_frequencies.data);
	leafShader_sh->registerUniform("leaf_amplitude",			UniformType::F1,	& g_tree_leaf_amplitude);
	leafShader_sh->registerUniform("leaf_frequency",			UniformType::F1,	& g_tree_leaf_frequency);
	leafShader_sh->registerUniform("window_size",				UniformType::F2,	& g_window_sizes.data);		
	leafShader_sh->registerUniform("season",					UniformType::F1,	& g_season);		
	leafShader_sh->registerUniform("transition_control",		UniformType::F1,	& g_transitionControl);		
	leafShader_sh->registerUniform("dither",					UniformType::F1,	& g_dither);

	lLODShader->registerUniform("branch_count",				UniformType::F1,	& this->branchCountF);
	lLODShader->registerUniform("time",						UniformType::F1,	& g_float_time);
	lLODShader->registerUniform("time_offset",				UniformType::F1,	& time_offset);	
	lLODShader->registerUniform("wind_direction",			UniformType::F3,	& wind_relative_direction.data);
	lLODShader->registerUniform("wind_strength",			UniformType::F1,	& g_tree_wind_strength);
	lLODShader->registerUniform("wood_amplitudes",			UniformType::F4,	& g_tree_wood_amplitudes.data);
	lLODShader->registerUniform("wood_frequencies",			UniformType::F4,	& g_tree_wood_frequencies.data);
	lLODShader->registerUniform("leaf_amplitude",			UniformType::F1,	& g_tree_leaf_amplitude);
	lLODShader->registerUniform("leaf_frequency",			UniformType::F1,	& g_tree_leaf_frequency);
	lLODShader->registerUniform("MultiplyAmbient",			UniformType::F1,	& g_leaves_MultiplyAmbient);
	lLODShader->registerUniform("MultiplyDiffuse",			UniformType::F1,	& g_leaves_MultiplyDiffuse);
	lLODShader->registerUniform("MultiplySpecular",			UniformType::F1,	& g_leaves_MultiplySpecular);
	lLODShader->registerUniform("MultiplyTranslucency",		UniformType::F1,	& g_leaves_MultiplyTranslucency);
	lLODShader->registerUniform("ReduceTranslucencyInShadow", UniformType::F1,	& g_leaves_ReduceTranslucencyInShadow);
	lLODShader->registerUniform("shadow_intensity",			UniformType::F1,	& g_leaves_shadow_intensity);
	lLODShader->registerUniform("LightDiffuseColor",		UniformType::F3,	& g_leaves_LightDiffuseColor.data);
	lLODShader->registerUniform("window_size",				UniformType::F2,	& g_window_sizes.data);		


	// create branch data texture
	createDataTexture();
	// link data texture
	//dataTexture->save("dataTexture.png");
	branchShader->linkTexture(dataTexture);
	branchShader_sh->linkTexture(dataTexture);
	leafShader	->linkTexture(dataTexture);
	leafShader_sh->linkTexture(dataTexture);
	bLODShader	->linkTexture(dataTexture);
	lLODShader	->linkTexture(dataTexture);


	// create VBO & EBO with geometry...
	createVBOs();

	// link vbos & shaders
	branchesVBO	->compileWithShader(branchShader);
	leavesVBO	->compileWithShader(leafShader);
	branchesVBO	->compileWithShader(branchShader_sh);
	leavesVBO	->compileWithShader(leafShader_sh);
	branchesVBO	->compileWithShader(bLODShader);
	leavesVBO	->compileWithShader(lLODShader);

	//printf("branch count = %i\n", branchCount);
	//printf("DYN_TREE done (branch VBOid:%i, branchEBOid:%i, leafVBOid:%i)\n", branchesVBO->getID(), branchesEBO->getID(), leavesVBO->getID());


}

void DTree::initLOD1()
{
	lod1_sliceCount = 3.0;
	lod1_sliceSetCount = 3.0;
	eBranchmapInternalFormat = GL_RGBA32F;
	eBranchmapFormat		 = GL_RGBA;
	eBranchmapDataType		 = GL_FLOAT;
	// create place to store MVP matrices of slices...
	vector<Matrix4x4*> MVPmatrices;
	Matrix4x4 *mvp;
	// create slices

	// create 2 sliceSets (cross, double sided)
	v3 dir = v3(-1.0, 0.0, 0.0);
	v3 right = v3(0.0, 0.0, -1.0);
	float res = 512;
	lod1_win_resolution = v2 (res, res);
	
	DTreeSliceSet * set;	
	set = new DTreeSliceSet();
	set->rotation_y = 0.0f;
	
	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod1_sliceCount, lod1_win_resolution.x, lod1_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->setSlices(this->slices);
	lod1_sliceSets.push_back(set);
	slices.clear();	
	
	float angle = 45;

	dir.rotateY(angle*DEG_TO_RAD);
	right.rotateY(angle*DEG_TO_RAD);
	set = new DTreeSliceSet();
	set->rotation_y = angle;
	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod1_sliceCount, lod1_win_resolution.x, lod1_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->setSlices(this->slices);
	lod1_sliceSets.push_back(set);
	slices.clear();	

	dir.rotateY(-2*angle*DEG_TO_RAD);
	right.rotateY(-2*angle*DEG_TO_RAD);
	set = new DTreeSliceSet();	
	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod1_sliceCount, lod1_win_resolution.x, lod1_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->rotation_y = -angle;
	set->setSlices(this->slices);
	//set->createFromDir(this, dir);
	lod1_sliceSets.push_back(set);
	slices.clear();	


	// create data texture for slices...
	SAFE_DELETE_PTR(lod1_dataTexture);
	lod1_dataTexture = createLODdataTexture(MVPmatrices);

	for (int t=0; t<3; t++){
		SAFE_DELETE_PTR(MVPmatrices[t]);
	}
	MVPmatrices.clear();
	// join textures to one...
	joinSliceSetsTextures(	& lod1_jColorMap,
							& lod1_jDataMap,
							& lod1_jDepthMap,
							& lod1_jNormalMap,
							& lod1_sliceSets,
							lod1_sliceSetCount,
							lod1_sliceCount, 
							lod1_win_resolution.x,
							lod1_win_resolution.y);
	
	// create mipmaps
	lod1_jColorMap->generateMipmaps();
	lod1_jNormalMap->generateMipmaps();
	lod1_jNormalMap->setParameterI(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//lod1_jDepthMap->generateMipmaps();

	// init shaders
	lod1_shader = new Shader("lod1");
	lod1_shader->loadShader(DYN_TREE::SHADER_LOD_V, DYN_TREE::SHADER_LOD_F);
	
	lod1_shader_shadow = new Shader("lod1_shadow");
	lod1_shader_shadow->loadShader(DYN_TREE::SHADER_SHADOW_LOD_V, DYN_TREE::SHADER_SHADOW_LOD_F);
	
	lod1_jColorMap			 ->textureUnitNumber = 0;
	lod1_jNormalMap			 ->textureUnitNumber = 0;
	branchNoiseTexture	 ->textureUnitNumber = 0;
	leafNoiseTexture	 ->textureUnitNumber = 0;
	lod1_jDataMap			 ->textureUnitNumber = 0;
	lod1_jDepthMap			 ->textureUnitNumber = 0;
	seasonMap			 ->textureUnitNumber = 0;
	lod1_dataTexture		 ->textureUnitNumber = 0;
	lod1_shader_shadow->linkTexture(lod1_jColorMap);
	lod1_shader_shadow->linkTexture(lod1_jNormalMap);
	lod1_shader_shadow->linkTexture(branchNoiseTexture);
	lod1_shader_shadow->linkTexture(leafNoiseTexture);
	lod1_shader_shadow->linkTexture(lod1_jDataMap);
	lod1_shader_shadow->linkTexture(lod1_jDepthMap);
	lod1_shader_shadow->linkTexture(seasonMap);
	lod1_shader_shadow->linkTexture(lod1_dataTexture);
	// link textures to shader
	//shader->linkTexture(colorMap			);
	//shader->linkTexture(displacementMap	);
	//shader->linkTexture(dataMap			);

	//l2_color	= lod1_shader->getGLLocation("colorMap"			);
	//l2_displ	= lod1_shader->getGLLocation("leaf_noise_tex"	);
	//l2_displ2	= lod1_shader->getGLLocation("branch_noise_tex"	);
	//l2_data		= lod1_shader->getGLLocation("dataMap"			);
	//l2_normal	= lod1_shader->getGLLocation("normalMap"		);
	//l2_depth	= lod1_shader->getGLLocation("depthMap"			);
	//
	//l2_season	= lod1_shader->getGLLocation("seasonMap"		);

	lod1_shader->linkTexture(lod1_jColorMap);
	lod1_shader->linkTexture(lod1_jNormalMap);
	lod1_shader->linkTexture(branchNoiseTexture);
	lod1_shader->linkTexture(leafNoiseTexture);
	lod1_shader->linkTexture(lod1_jDataMap);
	lod1_shader->linkTexture(lod1_jDepthMap);
	lod1_shader->linkTexture(seasonMap);
	lod1_shader->linkTexture(g_shadowmap1);
	lod1_shader->linkTexture(lod1_dataTexture);

	lod1_shader->registerUniform("shadowmap_size"			, UniformType::F2, & g_ShadowMapSize.data				);
	lod1_shader->registerUniform("samples"					, UniformType::I1, & g_shadowMapSamples					);
	lod1_shader->registerUniform("branch_count"			, UniformType::F1, & this->branchCountF				);
	lod1_shader->registerUniform("time"					, UniformType::F1, & g_float_time					);
	lod1_shader->registerUniform("time_offset"			, UniformType::F1, & time_offset					);
	lod1_shader->registerUniform("season"				, UniformType::F1, & g_season						);
	lod1_shader->registerUniform("instancing"			, UniformType::I1, & isInstancingEnabled			);
	lod1_shader->registerUniform("shift"				, UniformType::F1, & g_transitionShift				);
	lod1_shader->registerUniform("transition_control"	, UniformType::F1, & g_transitionControl			);
	lod1_shader->registerUniform("movementVectorA"		, UniformType::F2, & g_tree_movementVectorA			);
	lod1_shader->registerUniform("movementVectorB"		, UniformType::F2, & g_tree_movementVectorB			);
	lod1_shader->registerUniform("window_size"			, UniformType::F2, & lod1_win_resolution			);
	lod1_shader->registerUniform("wood_amplitudes"		, UniformType::F4, & g_tree_wood_amplitudes.data	);
	lod1_shader->registerUniform("wood_frequencies"		, UniformType::F4, & g_tree_wood_frequencies.data	);
	lod1_shader->registerUniform("leaf_amplitude"		, UniformType::F1, & g_tree_leaf_amplitude			);
	lod1_shader->registerUniform("leaf_frequency"		, UniformType::F1, & g_tree_leaf_frequency			);
	lod1_shader->registerUniform("wind_direction"		, UniformType::F3, & g_tree_wind_direction			);
	lod1_shader->registerUniform("wind_strength"		, UniformType::F1, & g_tree_wind_strength			);
	//lod1_shader->registerUniform("varA"					, UniformType::F1, & g_varA							);
	//lod1_shader->registerUniform("scale"				, UniformType::F1, & g_ParallaxScale				);
	//lod1_shader->registerUniform("bias"					, UniformType::F1, & g_ParallaxBias					);
	lod1_shader->registerUniform("MultiplyAmbient",				UniformType::F1,	& g_leaves_MultiplyAmbient);
	lod1_shader->registerUniform("MultiplyDiffuse",				UniformType::F1,	& g_leaves_MultiplyDiffuse);
	lod1_shader->registerUniform("MultiplySpecular",			UniformType::F1,	& g_leaves_MultiplySpecular);
	lod1_shader->registerUniform("MultiplyTranslucency",		UniformType::F1,	& g_leaves_MultiplyTranslucency);
	lod1_shader->registerUniform("shadow_intensity",			UniformType::F1,	& g_leaves_shadow_intensity);
	lod1_shader->registerUniform("branch_count",				UniformType::F1,	& this->branchCountF);
	lod1_shader->registerUniform("sliceCnt",					UniformType::F1,	& this->lod1_sliceCount);
	lod1_shader->registerUniform("sliceSetsCnt",				UniformType::F1,	& this->lod1_sliceSetCount);
	lod1_shader->registerUniform("farMnear",	UniformType::F1,	& g_ShadowFarMNear);
	
	lod1_shader->registerUniform("LightMVPCameraVInverseMatrix",			UniformType::M4,	g_LightMVPCameraVInverseMatrix);
	lod1_shader->registerUniform("shadowMappingEnabled",	UniformType::B1,	& g_ShadowMappingEnabled);

	// shadow shader
	lod1_shader_shadow->registerUniform("branch_count"				, UniformType::F1, & this->branchCountF				);
	lod1_shader_shadow->registerUniform("time"						, UniformType::F1, & g_float_time					);
	lod1_shader_shadow->registerUniform("time_offset"				, UniformType::F1, & time_offset					);
	lod1_shader_shadow->registerUniform("season"						, UniformType::F1, & g_season						);
	lod1_shader_shadow->registerUniform("instancing"					, UniformType::I1, & isInstancingEnabled			);
	lod1_shader_shadow->registerUniform("shift"						, UniformType::F1, & g_transitionShift				);
	lod1_shader_shadow->registerUniform("transition_control"			, UniformType::F1, & g_transitionControl			);
	lod1_shader_shadow->registerUniform("movementVectorA"			, UniformType::F2, & g_tree_movementVectorA			);
	lod1_shader_shadow->registerUniform("movementVectorB"			, UniformType::F2, & g_tree_movementVectorB			);
	lod1_shader_shadow->registerUniform("window_size"				, UniformType::F2, & lod1_win_resolution			);
	lod1_shader_shadow->registerUniform("wood_amplitudes"			, UniformType::F4, & g_tree_wood_amplitudes.data	);
	lod1_shader_shadow->registerUniform("wood_frequencies"			, UniformType::F4, & g_tree_wood_frequencies.data	);
	lod1_shader_shadow->registerUniform("leaf_amplitude"				, UniformType::F1, & g_tree_leaf_amplitude			);
	lod1_shader_shadow->registerUniform("leaf_frequency"				, UniformType::F1, & g_tree_leaf_frequency			);
	lod1_shader_shadow->registerUniform("dither"						, UniformType::F1, & g_dither						);
	lod1_shader_shadow->registerUniform("farMnear"						, UniformType::F1, & g_ShadowFarMNear					);
	lod1_shader_shadow->registerUniform("branch_count"				, UniformType::F1, & this->branchCountF				);
	lod1_shader_shadow->registerUniform("wind_direction"			, UniformType::F3, & g_tree_wind_direction			);
	lod1_shader_shadow->registerUniform("wind_strength"				, UniformType::F1, & g_tree_wind_strength			);
	lod1_shader_shadow->registerUniform("sliceCnt",					UniformType::F1,	& this->lod1_sliceCount);
	lod1_shader_shadow->registerUniform("sliceSetsCnt",				UniformType::F1,	& this->lod1_sliceSetCount);
	
	iu1Loc1 = lod1_shader->getLocation("u_colorVariance");
	
	tmLoc0 = lod1_shader->getAttributeLocation("transformMatrix");
	iaLoc1 = lod1_shader->getAttributeLocation("colorVariance");
	tmLoc0_shadow = lod1_shader_shadow->getAttributeLocation("transformMatrix");
	iaLoc1_shadow = lod1_shader_shadow->getAttributeLocation("colorVariance");

	int i;
	// int i = lod1_shader->registerUniform("time_offset"	, UniformType::F1, & tree_time_offset);
	// u_time_offset = lod1_shader->getUniform(i);
	
	/*
	shader = new Shader("test");
	shader->loadShader("shaders/test_vs.glsl", "shaders/test_fs.glsl");
	// link textures to shader
	shader->linkTexture(frontDecalMap		);
	shader->linkTexture(frontNormalMap		);
	shader->linkTexture(frontTranslucencyMap);
	shader->linkTexture(frontHalfLife2Map	);
	shader->linkTexture(backDecalMap		);
	shader->linkTexture(backNormalMap		);
	shader->linkTexture(backTranslucencyMap	);
	shader->linkTexture(backHalfLife2Map	);
	*/
	vector<v3> vertexArr0;
	vector<v3> normalArr0;
	vector<v3> tangentArr0;
	vector<v2> texCoordArr0;
	vector<v2> sliceAtrArr0;

	vertexArr0.push_back(v3( 0.0,  0.0, -0.5 ));
	vertexArr0.push_back(v3( 0.0,  0.0,  0.5 ));
	vertexArr0.push_back(v3( 0.0,  1.0,  0.5 ));
	vertexArr0.push_back(v3( 0.0,  1.0, -0.5 ));
	vertexArr0.push_back(v3(-0.1,  0.0, -0.5 ));
	vertexArr0.push_back(v3(-0.1,  0.0,  0.5 ));
	vertexArr0.push_back(v3(-0.1,  1.0,  0.5 ));
	vertexArr0.push_back(v3(-0.1,  1.0, -0.5 ));
	vertexArr0.push_back(v3( 0.1,  0.0, -0.5 ));
	vertexArr0.push_back(v3( 0.1,  0.0,  0.5 ));
	vertexArr0.push_back(v3( 0.1,  1.0,  0.5 ));
	vertexArr0.push_back(v3( 0.1,  1.0, -0.5 ));

	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));

	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	
	float y_max = 1.0;//0.97; 

	texCoordArr0.push_back(v2( 1.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  y_max	));
	texCoordArr0.push_back(v2( 1.0,  y_max	));
	texCoordArr0.push_back(v2( 1.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  y_max	));
	texCoordArr0.push_back(v2( 1.0,  y_max	));
	texCoordArr0.push_back(v2( 1.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  y_max	));
	texCoordArr0.push_back(v2( 1.0,  y_max	));

	// first slice number, second sliceSet number
	sliceAtrArr0.push_back(v2( 1.0,  0.0));
	sliceAtrArr0.push_back(v2( 1.0,  0.0));
	sliceAtrArr0.push_back(v2( 1.0,  0.0));
	sliceAtrArr0.push_back(v2( 1.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 2.0,  0.0));
	sliceAtrArr0.push_back(v2( 2.0,  0.0));
	sliceAtrArr0.push_back(v2( 2.0,  0.0));
	sliceAtrArr0.push_back(v2( 2.0,  0.0));
	
	// A 

	int sliceCount = 3;
	v3 axisY = v3(0.0, 1.0, 0.0);
	// B
	float angleR= angle;
	for (i=0; i<(sliceCount*4); i++){
		vertexArr0.push_back(vertexArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		normalArr0.push_back(normalArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		tangentArr0.push_back(tangentArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		texCoordArr0.push_back(texCoordArr0[i]);
		sliceAtrArr0.push_back(v2(sliceAtrArr0[i].x, 1.0));
	}
	// C
	angleR= -angle;
	for (i=0; i<(sliceCount*4); i++){
		vertexArr0.push_back(vertexArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		normalArr0.push_back(normalArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		tangentArr0.push_back(tangentArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		texCoordArr0.push_back(texCoordArr0[i]);
		sliceAtrArr0.push_back(v2(sliceAtrArr0[i].x, 2.0));
	}
	// copy to buffer
	GLfloat* vertices	= new GLfloat[3*vertexArr0.size()]; 
	GLfloat* normals	= new GLfloat[3*normalArr0.size()]; 
	GLfloat* tangents	= new GLfloat[3*tangentArr0.size()]; 
	GLfloat* texCoords	= new GLfloat[2*texCoordArr0.size()]; 
	GLfloat* sliceAttr	= new GLfloat[2*sliceAtrArr0.size()]; 
	int vertexCount = vertexArr0.size();
	for (i=0; i<vertexCount; i++){
		vertices[i*3 + 0] = vertexArr0[i].x;
		vertices[i*3 + 1] = vertexArr0[i].y;
		vertices[i*3 + 2] = vertexArr0[i].z;
		normals	[i*3 + 0] = normalArr0[i].x;
		normals	[i*3 + 1] = normalArr0[i].y;
		normals	[i*3 + 2] = normalArr0[i].z;
		tangents[i*3 + 0] = tangentArr0[i].x;
		tangents[i*3 + 1] = tangentArr0[i].y;
		tangents[i*3 + 2] = tangentArr0[i].z;
		texCoords[i*2 + 0] = texCoordArr0[i].x;
		texCoords[i*2 + 1] = texCoordArr0[i].y;
		sliceAttr[i*2 + 0] = sliceAtrArr0[i].x;
		sliceAttr[i*2 + 1] = sliceAtrArr0[i].y;
	}
	// delete vector 'vertexArr0'

	vertexArr0.clear();
	normalArr0.clear();
	tangentArr0.clear();
	texCoordArr0.clear();
	sliceAtrArr0.clear();


	// init VBO
	int count = vertexCount;
	lod1_vbo = new VBO();
	lod1_vbo->setVertexCount(count);
	// position
	VBODataSet * dataSet = new VBODataSet(
		vertices,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::POSITION,
		true
	);
	lod1_vbo->addVertexAttribute( dataSet );
	// normal
	dataSet = new VBODataSet(
		normals,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::NORMAL,
		false
	);
	lod1_vbo->addVertexAttribute( dataSet );
	// tangent
	dataSet = new VBODataSet(
		tangents,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TANGENT,
		false
	);
	lod1_vbo->addVertexAttribute( dataSet );
	// texture coordinates
	dataSet = new VBODataSet(
		texCoords,
		2*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TEXCOORD0,
		false
	);
	lod1_vbo->addVertexAttribute( dataSet );
	// slice attributes
	dataSet = new VBODataSet(
		sliceAttr,
		2*sizeof(GLfloat),
		GL_FLOAT, 
		"sliceDescription",
		false
	);
	lod1_vbo->addVertexAttribute( dataSet );
	// link vbo and shaders
	lod1_vbo->compileData(GL_STATIC_DRAW);
	lod1_vbo->compileWithShader(lod1_shader);
	lod1_vbo->compileWithShader(lod1_shader_shadow);

	/*	
	*	CREATE EBO
	*		
	*/
	unsigned int ebo_data[3*36] = {	 0, 1, 2, 3,	 4, 5, 6, 7,	 8, 9,10,11, // A
									24,25,26,27,	28,29,30,31,	32,33,34,35, // C
									12,13,14,15,	16,17,18,19,	20,21,22,23, // B
									
									24,25,26,27,	28,29,30,31,	32,33,34,35, // C
									12,13,14,15,	16,17,18,19,	20,21,22,23, // B
									 0, 1, 2, 3,	 4, 5, 6, 7,	 8, 9,10,11, // A

									12,13,14,15,	16,17,18,19,	20,21,22,23, // B
									 0, 1, 2, 3,	 4, 5, 6, 7,	 8, 9,10,11, // A
									24,25,26,27,	28,29,30,31,	32,33,34,35  // C
									
								};
	int iCnt = 3 * 4 * sliceCount * 3; // 4 = vertexCnt of one slice, 3 = number of slice sets
	
	lod1_ebo = new EBO();
	lod1_ebo->create(GL_UNSIGNED_INT, GL_QUADS, iCnt, ebo_data, GL_STATIC_DRAW);
}

void DTree::initLOD2b()
{   
	lod2_sliceCount = 1.0;
	lod2_sliceSetCount = 2.0;
	eBranchmapInternalFormat = GL_RGBA32F;
	eBranchmapFormat		 = GL_RGBA;
	eBranchmapDataType		 = GL_FLOAT;
	// create place to store MVP matrices of slices...
	vector<Matrix4x4*> MVPmatrices;
	Matrix4x4 *mvp;
	// create slices

	// create 2 sliceSets (cross, double sided)
	v3 dir = v3(-1.0, 0.0, 0.0);
	v3 right = v3(0.0, 0.0, -1.0);
	float res = 256;
	lod2_win_resolution = v2 (res, res);
	
	DTreeSliceSet * set;	
	set = new DTreeSliceSet();
	set->rotation_y = 0.0f;
	
	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod2_sliceCount, lod2_win_resolution.x, lod2_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->setSlices(this->slices);
	lod2_sliceSets.push_back(set);
	slices.clear();	
	
	float angle = 90;

	dir.rotateY(angle*DEG_TO_RAD);
	right.rotateY(angle*DEG_TO_RAD);
	set = new DTreeSliceSet();
	set->rotation_y = angle;
	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod2_sliceCount, lod2_win_resolution.x, lod2_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->setSlices(this->slices);
	lod2_sliceSets.push_back(set);
	slices.clear();	


	// create data texture for slices...
	SAFE_DELETE_PTR(lod2_dataTexture);
	lod2_dataTexture = createLODdataTexture(MVPmatrices);

	for (int t=0; t<int(lod2_sliceSetCount); t++){
		SAFE_DELETE_PTR(MVPmatrices[t]);
	}
	MVPmatrices.clear();
	// join textures to one...
	joinSliceSetsTextures(	&lod2_jColorMap,
							&lod2_jDataMap,
							&lod2_jDepthMap,
							&lod2_jNormalMap,
							&lod2_sliceSets,
							lod2_sliceSetCount,
							lod2_sliceCount, 
							lod2_win_resolution.x,
							lod2_win_resolution.y);
	// create mipmaps
	lod2_jColorMap->generateMipmaps();
	lod2_jNormalMap->generateMipmaps();
	lod2_jNormalMap->setParameterI(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//lod1_jDepthMap->generateMipmaps();

	// init shaders
	lod2_shader = new Shader("lod2");
	lod2_shader->loadShader(DYN_TREE::SHADER_LOD_V, DYN_TREE::SHADER_LOD_F);
	
	lod2_shader_shadow = new Shader("lod2_shadow");
	lod2_shader_shadow->loadShader(DYN_TREE::SHADER_SHADOW_LOD_V, DYN_TREE::SHADER_SHADOW_LOD_F);
	
	lod2_jColorMap			->textureUnitNumber = 0;
	lod2_jNormalMap			->textureUnitNumber = 0;
	branchNoiseTexture		->textureUnitNumber = 0;
	leafNoiseTexture		->textureUnitNumber = 0;
	lod2_jDataMap			->textureUnitNumber = 0;
	lod2_jDepthMap			->textureUnitNumber = 0;
	seasonMap				->textureUnitNumber = 0;
	lod2_dataTexture		->textureUnitNumber = 0;
	lod2_shader_shadow->linkTexture(lod2_jColorMap);
	lod2_shader_shadow->linkTexture(lod2_jNormalMap);
	lod2_shader_shadow->linkTexture(branchNoiseTexture);
	lod2_shader_shadow->linkTexture(leafNoiseTexture);
	lod2_shader_shadow->linkTexture(lod2_jDataMap);
	lod2_shader_shadow->linkTexture(lod2_jDepthMap);
	lod2_shader_shadow->linkTexture(seasonMap);
	lod2_shader_shadow->linkTexture(lod2_dataTexture);
	

	lod2_shader->linkTexture(lod2_jColorMap);
	lod2_shader->linkTexture(lod2_jNormalMap);
	lod2_shader->linkTexture(branchNoiseTexture);
	lod2_shader->linkTexture(leafNoiseTexture);
	lod2_shader->linkTexture(lod2_jDataMap);
	lod2_shader->linkTexture(lod2_jDepthMap);
	lod2_shader->linkTexture(seasonMap);
	lod2_shader->linkTexture(g_shadowmap1);
	lod2_shader->linkTexture(lod2_dataTexture);

	lod2_shader->registerUniform("shadowmap_size"			, UniformType::F2, & g_ShadowMapSize.data			);
	lod2_shader->registerUniform("samples"					, UniformType::I1, & g_shadowMapSamples				);
	lod2_shader->registerUniform("branch_count"				, UniformType::F1, & this->branchCountF				);
	lod2_shader->registerUniform("time"						, UniformType::F1, & g_float_time					);
	lod2_shader->registerUniform("time_offset"				, UniformType::F1, & time_offset					);
	lod2_shader->registerUniform("season"					, UniformType::F1, & g_season						);
	lod2_shader->registerUniform("instancing"				, UniformType::I1, & isInstancingEnabled			);
	lod2_shader->registerUniform("shift"					, UniformType::F1, & g_transitionShift				);
	lod2_shader->registerUniform("transition_control"		, UniformType::F1, & g_transitionControl			);
	lod2_shader->registerUniform("movementVectorA"			, UniformType::F2, & g_tree_movementVectorA			);
	lod2_shader->registerUniform("movementVectorB"			, UniformType::F2, & g_tree_movementVectorB			);
	lod2_shader->registerUniform("window_size"				, UniformType::F2, & lod2_win_resolution			);
	lod2_shader->registerUniform("wood_amplitudes"			, UniformType::F4, & g_tree_wood_amplitudes.data	);
	lod2_shader->registerUniform("wood_frequencies"			, UniformType::F4, & g_tree_wood_frequencies.data	);
	lod2_shader->registerUniform("leaf_amplitude"			, UniformType::F1, & g_tree_leaf_amplitude			);
	lod2_shader->registerUniform("leaf_frequency"			, UniformType::F1, & g_tree_leaf_frequency			);
	lod2_shader->registerUniform("wind_direction"			, UniformType::F3, & g_tree_wind_direction			);
	lod2_shader->registerUniform("wind_strength"			, UniformType::F1, & g_tree_wind_strength			);
	lod2_shader->registerUniform("MultiplyAmbient"			, UniformType::F1,	& g_leaves_MultiplyAmbient		);
	lod2_shader->registerUniform("MultiplyDiffuse"			, UniformType::F1,	& g_leaves_MultiplyDiffuse		);
	lod2_shader->registerUniform("MultiplySpecular"			, UniformType::F1,	& g_leaves_MultiplySpecular		);
	lod2_shader->registerUniform("MultiplyTranslucency"		, UniformType::F1,	& g_leaves_MultiplyTranslucency	);
	lod2_shader->registerUniform("shadow_intensity"			, UniformType::F1,	& g_leaves_shadow_intensity		);
	lod2_shader->registerUniform("branch_count"				, UniformType::F1,	& this->branchCountF			);
	lod2_shader->registerUniform("sliceCnt"					, UniformType::F1,	& this->lod2_sliceCount			);
	lod2_shader->registerUniform("sliceSetsCnt"				, UniformType::F1,	& this->lod2_sliceSetCount		);
	lod2_shader->registerUniform("farMnear"					, UniformType::F1,	& g_ShadowFarMNear				);
	lod2_shader->registerUniform("LightMVPCameraVInverseMatrix", UniformType::M4,	g_LightMVPCameraVInverseMatrix);
	lod2_shader->registerUniform("shadowMappingEnabled"		, UniformType::B1,	& g_ShadowMappingEnabled		);

	// shadow shader
	lod2_shader_shadow->registerUniform("branch_count"				, UniformType::F1, & this->branchCountF				);
	lod2_shader_shadow->registerUniform("time"						, UniformType::F1, & g_float_time					);
	lod2_shader_shadow->registerUniform("time_offset"				, UniformType::F1, & time_offset					);
	lod2_shader_shadow->registerUniform("season"					, UniformType::F1, & g_season						);
	lod2_shader_shadow->registerUniform("instancing"				, UniformType::I1, & isInstancingEnabled			);
	lod2_shader_shadow->registerUniform("shift"						, UniformType::F1, & g_transitionShift				);
	lod2_shader_shadow->registerUniform("transition_control"		, UniformType::F1, & g_transitionControl			);
	lod2_shader_shadow->registerUniform("movementVectorA"			, UniformType::F2, & g_tree_movementVectorA			);
	lod2_shader_shadow->registerUniform("movementVectorB"			, UniformType::F2, & g_tree_movementVectorB			);
	lod2_shader_shadow->registerUniform("window_size"				, UniformType::F2, & lod2_win_resolution			);
	lod2_shader_shadow->registerUniform("wood_amplitudes"			, UniformType::F4, & g_tree_wood_amplitudes.data	);
	lod2_shader_shadow->registerUniform("wood_frequencies"			, UniformType::F4, & g_tree_wood_frequencies.data	);
	lod2_shader_shadow->registerUniform("leaf_amplitude"			, UniformType::F1, & g_tree_leaf_amplitude			);
	lod2_shader_shadow->registerUniform("leaf_frequency"			, UniformType::F1, & g_tree_leaf_frequency			);
	lod2_shader_shadow->registerUniform("dither"					, UniformType::F1, & g_dither						);
	lod2_shader_shadow->registerUniform("farMnear"					, UniformType::F1, & g_ShadowFarMNear				);
	lod2_shader_shadow->registerUniform("branch_count"				, UniformType::F1, & this->branchCountF				);
	lod2_shader_shadow->registerUniform("wind_direction"			, UniformType::F3, & g_tree_wind_direction			);
	lod2_shader_shadow->registerUniform("wind_strength"				, UniformType::F1, & g_tree_wind_strength			);
	lod2_shader_shadow->registerUniform("sliceCnt"					, UniformType::F1, & this->lod2_sliceCount			);
	lod2_shader_shadow->registerUniform("sliceSetsCnt"				, UniformType::F1, & this->lod2_sliceSetCount		);
	
	lod2_iu1Loc1		= lod2_shader->getLocation("u_colorVariance");
	lod2_tmLoc0			= lod2_shader->getAttributeLocation("transformMatrix");
	lod2_iaLoc1			= lod2_shader->getAttributeLocation("colorVariance");
	lod2_tmLoc0_shadow	= lod2_shader_shadow->getAttributeLocation("transformMatrix");
	lod2_iaLoc1_shadow	= lod2_shader_shadow->getAttributeLocation("colorVariance");

	int i;
	// int i = lod1_shader->registerUniform("time_offset"	, UniformType::F1, & tree_time_offset);
	// u_time_offset = lod1_shader->getUniform(i);
	
	/*
	shader = new Shader("test");
	shader->loadShader("shaders/test_vs.glsl", "shaders/test_fs.glsl");
	// link textures to shader
	shader->linkTexture(frontDecalMap		);
	shader->linkTexture(frontNormalMap		);
	shader->linkTexture(frontTranslucencyMap);
	shader->linkTexture(frontHalfLife2Map	);
	shader->linkTexture(backDecalMap		);
	shader->linkTexture(backNormalMap		);
	shader->linkTexture(backTranslucencyMap	);
	shader->linkTexture(backHalfLife2Map	);
	*/
	vector<v3> vertexArr0;
	vector<v3> normalArr0;
	vector<v3> tangentArr0;
	vector<v2> texCoordArr0;
	vector<v2> sliceAtrArr0;

	vertexArr0.push_back(v3( 0.0,  0.0, -0.5 ));
	vertexArr0.push_back(v3( 0.0,  0.0,  0.5 ));
	vertexArr0.push_back(v3( 0.0,  1.0,  0.5 ));
	vertexArr0.push_back(v3( 0.0,  1.0, -0.5 ));

	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));

	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  -1.0));
	
	float y_max = 1.0;//0.97; 

	texCoordArr0.push_back(v2( 1.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  0.0	));
	texCoordArr0.push_back(v2( 0.0,  y_max	));
	texCoordArr0.push_back(v2( 1.0,  y_max	));

	// first slice number, second sliceSet number
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	
	
	// A 

	int sliceCount = 1;
	v3 axisY = v3(0.0, 1.0, 0.0);
	// B
	float angleR= angle;
	for (i=0; i<(sliceCount*4); i++){
		vertexArr0.push_back(vertexArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		normalArr0.push_back(normalArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		tangentArr0.push_back(tangentArr0[i].getRotated(angleR*DEG_TO_RAD, axisY));
		texCoordArr0.push_back(texCoordArr0[i]);
		sliceAtrArr0.push_back(v2(sliceAtrArr0[i].x, 1.0));
	}
	// copy to buffer
	GLfloat* vertices	= new GLfloat[3*vertexArr0.size()]; 
	GLfloat* normals	= new GLfloat[3*normalArr0.size()]; 
	GLfloat* tangents	= new GLfloat[3*tangentArr0.size()]; 
	GLfloat* texCoords	= new GLfloat[2*texCoordArr0.size()]; 
	GLfloat* sliceAttr	= new GLfloat[2*sliceAtrArr0.size()]; 
	int vertexCount = vertexArr0.size();
	for (i=0; i<vertexCount; i++){
		vertices[i*3 + 0] = vertexArr0[i].x;
		vertices[i*3 + 1] = vertexArr0[i].y;
		vertices[i*3 + 2] = vertexArr0[i].z;
		normals	[i*3 + 0] = normalArr0[i].x;
		normals	[i*3 + 1] = normalArr0[i].y;
		normals	[i*3 + 2] = normalArr0[i].z;
		tangents[i*3 + 0] = tangentArr0[i].x;
		tangents[i*3 + 1] = tangentArr0[i].y;
		tangents[i*3 + 2] = tangentArr0[i].z;
		texCoords[i*2 + 0] = texCoordArr0[i].x;
		texCoords[i*2 + 1] = texCoordArr0[i].y;
		sliceAttr[i*2 + 0] = sliceAtrArr0[i].x;
		sliceAttr[i*2 + 1] = sliceAtrArr0[i].y;
	}
	// delete vector 'vertexArr0'

	vertexArr0.clear();
	normalArr0.clear();
	tangentArr0.clear();
	texCoordArr0.clear();
	sliceAtrArr0.clear();


	// init VBO
	int count = vertexCount;
	lod2_vbo = new VBO();
	lod2_vbo->setVertexCount(count);
	// position
	VBODataSet * dataSet = new VBODataSet(
		vertices,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::POSITION,
		true
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// normal
	dataSet = new VBODataSet(
		normals,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::NORMAL,
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// tangent
	dataSet = new VBODataSet(
		tangents,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TANGENT,
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// texture coordinates
	dataSet = new VBODataSet(
		texCoords,
		2*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TEXCOORD0,
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// slice attributes
	dataSet = new VBODataSet(
		sliceAttr,
		2*sizeof(GLfloat),
		GL_FLOAT, 
		"sliceDescription",
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// link vbo and shaders
	lod2_vbo->compileData(GL_STATIC_DRAW);
	lod2_vbo->compileWithShader(lod2_shader);
	lod2_vbo->compileWithShader(lod2_shader_shadow);

	/*	
	*	CREATE EBO
	*		
	*/

	unsigned int ebo_data[3*36] = {	 0, 1, 2, 3,  // A
									 4, 5, 6, 7,  // B

									 4, 5, 6, 7,  // B
									 0, 1, 2, 3   // A									
								};
	int cases_count = 2;
	int iCnt = cases_count * 4 * lod2_sliceCount * lod2_sliceSetCount; // 4 = vertexCnt of one slice
	
	lod2_ebo = new EBO();
	lod2_ebo->create(GL_UNSIGNED_INT, GL_QUADS, iCnt, ebo_data, GL_STATIC_DRAW);

}

void DTree::initLOD2()
{
	// create slices
	lod2_sliceCount = 1.0;
	lod2_sliceSetCount = 2.0;
	// create place to store MVP matrices of slices...
	vector<Matrix4x4*> MVPmatrices;
	Matrix4x4 *mvp;

	// create 2 sliceSets (cross, double sided)
	v3 dir = v3(-1.0, 0.0, 0.0);
	v3 right = v3(0.0, 0.0, -1.0);
	float res = 256;
	lod2_win_resolution = v2 (res, res);
	
	DTreeSliceSet * set;
	set = new DTreeSliceSet();
	set->rotation_y = 0.0f;	

	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod2_sliceCount, lod2_win_resolution.x, lod2_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->setSlices(this->slices);
	lod2color1	= set->getSlice(0)->colormap;
	lod2normal1 = set->getSlice(0)->normalmap;
	lod2branch1 = set->getSlice(0)->datamap;
	lod2depth1	= set->getSlice(0)->depthmap;
	slices.clear();	

	dir.rotateY(90*DEG_TO_RAD);
	right.rotateY(90*DEG_TO_RAD);
	set = new DTreeSliceSet();
	set->rotation_y = 90;
	mvp = new Matrix4x4();
	this->createSlices(& this->slices, lod2_sliceCount, lod2_win_resolution.x, lod2_win_resolution.y, dir, right, mvp, false);
	MVPmatrices.push_back(mvp);
	set->setSlices(this->slices);	
	lod2color2	= set->getSlice(0)->colormap;	
	lod2normal2 = set->getSlice(0)->normalmap;
	lod2branch2 = set->getSlice(0)->datamap;
	lod2depth2	= set->getSlice(0)->depthmap;
	slices.clear();	

	// create data texture for slices...
	SAFE_DELETE_PTR(lod2dataTexture);
	//printf("LOD2 datatexture: ");
	lod2dataTexture = createLODdataTexture(MVPmatrices);
	// clear matrices
	for (int t=0; t<2; t++){
		SAFE_DELETE_PTR(MVPmatrices[t]);
	}
	MVPmatrices.clear();
	// join textures to one...
	//joinSliceSetsTextures();
	
	// create mipmaps
	//lod2color1->generateMipmaps();
	//lod2color2->generateMipmaps();
	//lod2branch1->generateMipmaps();
	//lod2branch2->generateMipmaps();
	//lod2normal1->generateMipmaps();
	//lod2normal2->generateMipmaps();
	//lod2normal1->setParameterI(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//lod2normal2->setParameterI(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//lod2depth1->generateMipmaps();
	//lod2depth2->generateMipmaps();

	//lod1_jColorMap->setParameterI(GL_TEXTURE_SAMPLES, GL_LINEAR_MIPMAP_LINEAR);
	
	// init shaders
	lod2_shader = new Shader("lod2");
	lod2_shader->loadShader("shaders/test4_vs.glsl", "shaders/test4_fs.glsl");
	lod2_shader->linkTexture(g_shadowmap1);
	lod2_shader_shadow = new Shader("lod2_shadow");
	lod2_shader_shadow->loadShader("shaders/test6_shadow_vs.glsl", "shaders/test6_shadow_fs.glsl");
	
	// link textures to shader
	lod2color1 ->inShaderName = "color_tex_1";
	lod2color2 ->inShaderName = "color_tex_2";
	lod2normal1->inShaderName = "normal_tex_1";
	lod2normal2->inShaderName = "normal_tex_2";
	lod2branch1->inShaderName = "branch_tex_1";
	lod2branch2->inShaderName = "branch_tex_2";
	lod2depth1->inShaderName = "depth_tex_1";
	lod2depth2->inShaderName = "depth_tex_2";

	lod2color1 ->textureUnitNumber = 0;
	lod2color2 ->textureUnitNumber = 0;
	lod2normal1->textureUnitNumber = 0;
	lod2normal2->textureUnitNumber = 0;
	lod2branch1->textureUnitNumber = 0;
	lod2branch2->textureUnitNumber = 0;
	lod2depth1->textureUnitNumber = 0;
	lod2depth2->textureUnitNumber = 0;
	lod2dataTexture->textureUnitNumber =0;

	lod2_shader_shadow->linkTexture(seasonMap);
	lod2_shader_shadow->linkTexture(leafNoiseTexture);
	lod2_shader_shadow->linkTexture(branchNoiseTexture);
	lod2_shader_shadow->linkTexture(lod2color1);
	lod2_shader_shadow->linkTexture(lod2color2);
	lod2_shader_shadow->linkTexture(lod2normal1);
	lod2_shader_shadow->linkTexture(lod2normal2);
	lod2_shader_shadow->linkTexture(lod2branch1);
	lod2_shader_shadow->linkTexture(lod2branch2);
	lod2_shader_shadow->linkTexture(lod2depth1);
	lod2_shader_shadow->linkTexture(lod2depth2);
	lod2_shader_shadow->linkTexture(lod2dataTexture);

	// lod2loc_color_tex_1	 = lod2_shader->getGLLocation("color_tex_1"		);
	// lod2loc_color_tex_2	 = lod2_shader->getGLLocation("color_tex_2"		);
	// lod2loc_normal_tex_1 = lod2_shader->getGLLocation("normal_tex_1"		);
	// lod2loc_normal_tex_2 = lod2_shader->getGLLocation("normal_tex_2"		);
	// lod2loc_branch_tex_1 = lod2_shader->getGLLocation("branch_tex_1"		);
	// lod2loc_branch_tex_2 = lod2_shader->getGLLocation("branch_tex_2"		);


	lod2_shader->linkTexture(seasonMap);
	lod2_shader->linkTexture(leafNoiseTexture);
	lod2_shader->linkTexture(branchNoiseTexture);
	lod2_shader->linkTexture(lod2color1);
	lod2_shader->linkTexture(lod2color2);
	lod2_shader->linkTexture(lod2normal1);
	lod2_shader->linkTexture(lod2normal2);
	lod2_shader->linkTexture(lod2branch1);
	lod2_shader->linkTexture(lod2branch2);
	lod2_shader->linkTexture(lod2dataTexture);



	//lod2loc_season_tex	= lod2_shader->getGLLocation("seasonMap"		);
	//lod2loc_leaf_tex	= lod2_shader->getGLLocation("leaf_noise_tex"	);
	//lod2loc_branch_tex	= lod2_shader->getGLLocation("branch_noise_tex"	);


	lod2_shader->registerUniform("time"							, UniformType::F1, & g_float_time	);
	lod2_shader->registerUniform("time_offset"					, UniformType::F1, & time_offset	);
	lod2_shader->registerUniform("season"						, UniformType::F1, & g_season		);
	lod2_shader->registerUniform("instancing"					, UniformType::I1, & isInstancingEnabled);
	lod2_shader->registerUniform("movementVectorA"				, UniformType::F2, & g_tree_movementVectorA			);
	lod2_shader->registerUniform("movementVectorB"				, UniformType::F2, & g_tree_movementVectorB			);
	lod2_shader->registerUniform("window_size"					, UniformType::F2, & lod2_win_resolution			);
	lod2_shader->registerUniform("wood_amplitudes"				, UniformType::F4, & g_tree_wood_amplitudes.data	);
	lod2_shader->registerUniform("wood_frequencies"				, UniformType::F4, & g_tree_wood_frequencies.data	);
	lod2_shader->registerUniform("leaf_amplitude"				, UniformType::F1, & g_tree_leaf_amplitude			);
	lod2_shader->registerUniform("leaf_frequency"				, UniformType::F1, & g_tree_leaf_frequency			);
	lod2_shader->registerUniform("MultiplyAmbient"				, UniformType::F1,	& g_leaves_MultiplyAmbient);
	lod2_shader->registerUniform("MultiplyDiffuse"				, UniformType::F1,	& g_leaves_MultiplyDiffuse);
	lod2_shader->registerUniform("MultiplySpecular"				, UniformType::F1,	& g_leaves_MultiplySpecular);
	lod2_shader->registerUniform("MultiplyTranslucency"			, UniformType::F1,	& g_leaves_MultiplyTranslucency);
	lod2_shader->registerUniform("ReduceTranslucencyInShadow"	, UniformType::F1,	& g_leaves_ReduceTranslucencyInShadow);
	lod2_shader->registerUniform("shadow_intensity"				, UniformType::F1,	& g_leaves_shadow_intensity);
	lod2_shader->registerUniform("LightDiffuseColor"				, UniformType::F3,	& g_leaves_LightDiffuseColor.data);
	lod2loc_colorVariance = lod2_shader->getLocation("u_colorVariance");
	lod2_shader->registerUniform("LightMVPCameraVInverseMatrix"	, UniformType::M4,	g_LightMVPCameraVInverseMatrix);
	lod2_shader->registerUniform("shadowMappingEnabled"			, UniformType::B1, & g_ShadowMappingEnabled);
	lod2_shader->registerUniform("near"							, UniformType::F1, & g_ShadowNear);
	lod2_shader->registerUniform("far"							, UniformType::F1, & g_ShadowFar);

	// shadow
	lod2_shader_shadow->registerUniform("time"					, UniformType::F1, & g_float_time	);
	lod2_shader_shadow->registerUniform("time_offset"			, UniformType::F1, & time_offset	);
	lod2_shader_shadow->registerUniform("season"					, UniformType::F1, & g_season		);
	lod2_shader_shadow->registerUniform("instancing"				, UniformType::I1, & isInstancingEnabled);
	lod2_shader_shadow->registerUniform("movementVectorA"		, UniformType::F2, & g_tree_movementVectorA			);
	lod2_shader_shadow->registerUniform("movementVectorB"		, UniformType::F2, & g_tree_movementVectorB			);
	lod2_shader_shadow->registerUniform("window_size"			, UniformType::F2, & lod2_win_resolution			);
	lod2_shader_shadow->registerUniform("wood_amplitudes"		, UniformType::F4, & g_tree_wood_amplitudes.data	);
	lod2_shader_shadow->registerUniform("wood_frequencies"		, UniformType::F4, & g_tree_wood_frequencies.data	);
	lod2_shader_shadow->registerUniform("leaf_amplitude"			, UniformType::F1, & g_tree_leaf_amplitude			);
	lod2_shader_shadow->registerUniform("leaf_frequency"			, UniformType::F1, & g_tree_leaf_frequency			);
	lod2_shader_shadow->registerUniform("near"					, UniformType::F1, & g_ShadowNear);
	lod2_shader_shadow->registerUniform("far"					, UniformType::F1, & g_ShadowFar);

	tm2Loc0 = lod2_shader->getAttributeLocation("transformMatrix");
	ia2Loc1 = lod2_shader->getAttributeLocation("colorVariance");
	tm2Loc0_shadow = lod2_shader_shadow->getAttributeLocation("transformMatrix");
	ia2Loc1_shadow = lod2_shader_shadow->getAttributeLocation("colorVariance");

	int i;
	
	vector<v3> vertexArr0;
	vector<v3> normalArr0;
	vector<v3> tangentArr0;
	vector<v2> texCoordArr0;
	vector<v2> sliceAtrArr0;

	vertexArr0.push_back(v3( 0.0,  0.0, -0.5 ));
	vertexArr0.push_back(v3( 0.0,  0.0,  0.5 ));
	vertexArr0.push_back(v3( 0.0,  1.0,  0.5 ));
	vertexArr0.push_back(v3( 0.0,  1.0, -0.5 ));
	
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));
	normalArr0.push_back(v3( 1.0,  0.0,  0.0));

	tangentArr0.push_back(v3( 0.0,  0.0,  1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  1.0));
	tangentArr0.push_back(v3( 0.0,  0.0,  1.0));
	
	float y_max = 1.0; 

	texCoordArr0.push_back(v2( 0.0,  0.0	));
	texCoordArr0.push_back(v2( 1.0,  0.0	));
	texCoordArr0.push_back(v2( 1.0,  y_max	));
	texCoordArr0.push_back(v2( 0.0,  y_max	));

	// first slice number, second sliceSet number
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	sliceAtrArr0.push_back(v2( 0.0,  0.0));
	
	// A 

	int sliceCount = 3;
	v3 axisY = v3(0.0, 1.0, 0.0);
	// B
	float angle= 90.0;
	for (i=0; i<4; i++){
		vertexArr0.push_back(vertexArr0[i].getRotated(angle*DEG_TO_RAD, axisY));
		normalArr0.push_back(normalArr0[i].getRotated(angle*DEG_TO_RAD, axisY));
		tangentArr0.push_back(tangentArr0[i].getRotated(angle*DEG_TO_RAD, axisY));
		texCoordArr0.push_back(texCoordArr0[i]);
		sliceAtrArr0.push_back(v2(sliceAtrArr0[i].x, 1.0));
	}
	// copy to buffer
	GLfloat* vertices	= new GLfloat[3*vertexArr0.size()]; 
	GLfloat* normals	= new GLfloat[3*normalArr0.size()]; 
	GLfloat* tangents	= new GLfloat[3*tangentArr0.size()]; 
	GLfloat* texCoords	= new GLfloat[2*texCoordArr0.size()]; 
	GLfloat* sliceAttr	= new GLfloat[2*sliceAtrArr0.size()]; 
	int vertexCount = vertexArr0.size();
	for (i=0; i<vertexCount; i++){
		vertices[i*3 + 0] = vertexArr0[i].x;
		vertices[i*3 + 1] = vertexArr0[i].y;
		vertices[i*3 + 2] = vertexArr0[i].z;
		normals	[i*3 + 0] = normalArr0[i].x;
		normals	[i*3 + 1] = normalArr0[i].y;
		normals	[i*3 + 2] = normalArr0[i].z;
		tangents[i*3 + 0] = tangentArr0[i].x;
		tangents[i*3 + 1] = tangentArr0[i].y;
		tangents[i*3 + 2] = tangentArr0[i].z;
		texCoords[i*2 + 0] = texCoordArr0[i].x;
		texCoords[i*2 + 1] = texCoordArr0[i].y;
		sliceAttr[i*2 + 0] = sliceAtrArr0[i].x;
		sliceAttr[i*2 + 1] = sliceAtrArr0[i].y;
	}
	// delete vector 'vertexArr0'

	vertexArr0.clear();
	normalArr0.clear();
	tangentArr0.clear();
	texCoordArr0.clear();
	sliceAtrArr0.clear();


	// init VBO
	int count = vertexCount;
	lod2_vbo = new VBO();
	lod2_vbo->setVertexCount(count);
	// position
	VBODataSet * dataSet = new VBODataSet(
		vertices,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::POSITION,
		true
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// normal
	dataSet = new VBODataSet(
		normals,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::NORMAL,
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// tangent
	dataSet = new VBODataSet(
		tangents,
		3*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TANGENT,
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// texture coordinates
	dataSet = new VBODataSet(
		texCoords,
		2*sizeof(GLfloat),
		GL_FLOAT, 
		ATTRIB_NAMES::TEXCOORD0,
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// slice attributes
	dataSet = new VBODataSet(
		sliceAttr,
		2*sizeof(GLfloat),
		GL_FLOAT, 
		"sliceDescription",
		false
	);
	lod2_vbo->addVertexAttribute( dataSet );
	// link vbo and shaders
	lod2_vbo->compileData(GL_STATIC_DRAW);
	lod2_vbo->compileWithShader(lod2_shader);
	lod2_vbo->compileWithShader(lod2_shader_shadow);

	/*	
	*	CREATE EBO
	*		
	*/
	const int iCnt = 2 * 8;
	unsigned int ebo_data[iCnt] = {	 0, 1, 2, 3,	 4, 5, 6, 7,
									 4, 5, 6, 7,	 0, 1, 2, 3,
									 
									};
	
	
	lod2_ebo = new EBO();
	lod2_ebo->create(GL_UNSIGNED_INT, GL_QUADS, iCnt, ebo_data, GL_STATIC_DRAW);

}
/*
void DTree::setInstances(v4 ** p_r, int c){
	positions_rotations = p_r;
	count = c;
}*/
void DTree::initInstances(vector<v4> &positions_rotations){
	// clear instances
	instancesLocked = true;
	count = positions_rotations.size();
	
	int i;

	for (i = 0; i<tree_instances.size(); i++){
		SAFE_DELETE_ARRAY_PTR(tree_instances[i]);
	}
	tree_instances.clear();
	for (i = 0; i<lod2_instanceMatrices.size(); i++){
		SAFE_DELETE_ARRAY_PTR(lod2_instanceMatrices[i]);
		lod2_instanceMatrices[i]=new float[ instanceFloatCount * count];
	}
	
	//lod2_instanceMatrices.clear();
	
	for (i = 0; i<lod1_instanceMatrices.size(); i++){
		SAFE_DELETE_ARRAY_PTR(lod1_instanceMatrices[i]);
		lod1_instanceMatrices[i]=new float[ instanceFloatCount * count];
	}
	
	//lod1_instanceMatrices.clear();
	
	
	for (i = 0; i<instancesInRenderQueues.size(); i++){
		SAFE_DELETE_ARRAY_PTR(instancesInRenderQueues[i]);
		instancesInRenderQueues[i]=new DTreeInstanceData * [count];
	}
	
	int k = instancesInRenderQueues.size();
	// init instances
	instanceFloatCount = 20;
	if (k<1){
		// first run
		// init instance matrices
		// LOD2
		lod2_instanceMatrices.push_back( new float[ instanceFloatCount * count]);	// A
		lod2_instanceMatrices.push_back( new float[ instanceFloatCount * count]);	// B
		// LOD1
		lod1_instanceMatrices.push_back( new float[ instanceFloatCount * count]);	// A
		lod1_instanceMatrices.push_back( new float[ instanceFloatCount * count]);	// B
		lod1_instanceMatrices.push_back( new float[ instanceFloatCount * count]);	// C
		instancesInRenderQueues.push_back ( new DTreeInstanceData * [count] );		// LOD0
		instancesInRenderQueues.push_back ( new DTreeInstanceData * [count] );		// LOD0-LOD1
		instancesInRenderQueues.push_back ( new DTreeInstanceData * [count] );		// LOD1
		instancesInRenderQueues.push_back ( new DTreeInstanceData * [count] );		// LOD1-LOD2
		instancesInRenderQueues.push_back ( new DTreeInstanceData * [count] );		// LOD2
	}

	
	/*****
	* LOD1 is made of 3 slice sets... due to blending problems we need to 
	* switch the rendering order of the sliceSets -> there are 3 types of
	* meshes [A,B,C] (just different EBO), from which the best is chosen on the fly...
	*/
	// init instances
	DTreeInstanceData * instance;
	float cosA, sinA;
	for (i=0; i< count; i++){
		instance = new DTreeInstanceData();
		instance->position	= positions_rotations[i].xyz();
		instance->rotation_y= positions_rotations[i].w;
		instance->index = i;
		cosA = cos(DEG_TO_RAD*instance->rotation_y);
		sinA = sin(DEG_TO_RAD*instance->rotation_y);
		// this matrix is passed to each instanced geometry and vertex shader 
		// ensures the proper positioning...
		// note, that by changing the order of matrices passed to VS,
		// we can influence the order of rendered instances
		m4 matrix(cosA			,	0			,	-sinA	,	0	,
				  0				,	1			,	0		,	0	,
				  sinA			,	0			,	cosA	,	0	,
				  instance->position.x,	instance->position.y, instance->position.z,	1);
		instance->transformMatrix = matrix; // copy matrix
		//instance->transformMatrix3= m3(matrix);
		// to be able to determine the proper type to display,
		// we need to know from which angle we are looking at the geometry
		// so we precalculate all we can now, to spare time in draw calls
		instance->dirA = v3( -1.0, 0.0, 0.0).getRotated( instance->rotation_y*DEG_TO_RAD, v3(0.0, 1.0, 0.0));	// A
		instance->dirB = instance->dirA.getRotated( 60 * DEG_TO_RAD, v3(0.0, 1.0, 0.0));				// B

		// add some instance attributes
		instance->colorVariance = v3( randomf(.8f, 1.f), randomf(.8f, 1.f), randomf(.9f, 1.f)) *  randomf(.8f, 1.f);
		instance->time_offset = randomf(0.f, 1000.f); 

		instance->alpha = 1.0;
		instance->index = i;
		tree_instances.push_back(instance);
	}
	
	// init instance matrices VBO
	if (i_matricesBuffID>0){
		glDeleteBuffers(1, &i_matricesBuffID);
	}
	glGenBuffers(1, &i_matricesBuffID);
	glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
		// load data
		glBufferData(GL_ARRAY_BUFFER, count * instanceFloatCount* sizeof(float), NULL, GL_STREAM_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	instancesLocked = false;	
}
void DTree::init(){
	// COUNT INDEPENDENT
	countRenderQueues.push_back(0);							// LOD0
	countRenderQueues.push_back(0);							// LOD0-LOD1
	countRenderQueues.push_back(0);							// LOD1
	countRenderQueues.push_back(0);							// LOD1-LOD2
	countRenderQueues.push_back(0);							// LOD2

	
	lod2_typeIndices.push_back(0);
	lod2_typeIndices.push_back(1);

	lod1_typeIndices.push_back(0);
	lod1_typeIndices.push_back(1);
	lod1_typeIndices.push_back(2);

	initLOD0();
	initLOD1();
	initLOD2b();
}
/*
void DTree::init(){
	initLOD0();
	initLOD1();
	//initLOD1b();
	initLOD2();
}/*/

void DTree::update(double time){
	prepareForRender();
}

void DTree::bakeToVBO(){

}

void DTree::fixTexType(){

}

v3	 DTree::transformTexCoords(v3 &origTexCoords){
	return origTexCoords;
}

BBox * DTree::getBBox()
{
	if (bbox==NULL){
		bbox = new BBox(v3(-0.5, -1.0, -0.5),v3(0.5, 0.0, 0.5), v3(0.0, 1.0, 0.0));
	}
	return bbox;
}

void DTree::createSlices(vector<DTreeSlice*>* slices, int count, int resolution_x, int resolution_y, v3 & direction, v3 & rightVector, Matrix4x4 *mvp, bool half)
{
	// init data pre-processing shader
	time_offset = 0.0;
	Shader * dataProcessShader = new Shader("data_pre-processor");
	dataProcessShader->loadShader(DYN_TREE::SHADER_PREPROCESS_V, DYN_TREE::SHADER_PREPROCESS_F);
	GLint	gl_location		= dataProcessShader->getGLLocation("branchMap");
	int		loc_win_size	= dataProcessShader->getLocation("window_size");
	//int		loc_cam_dir		= dataProcessShader->getLocation("cam_dir");
	//v3 d = direction.getRotated(90*DEG_TO_RAD, v3(0.0, 1.0, 0.0));
	//v3 r = rightVector.getRotated(90*DEG_TO_RAD, v3(0.0, 1.0, 0.0));
	//bLODShader->registerUniform("cam_dir",   UniformType::F3, & d);
	//lLODShader->registerUniform("cam_dir",   UniformType::F3, & d);
	//bLODShader->registerUniform("cam_right", UniformType::F3, & r);
	//lLODShader->registerUniform("cam_right", UniformType::F3, & r);
	// dummy depth map
	//Texture * depthmap = new Texture(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, resolution_x, resolution_y, "dummy_depthMap");
	//depthmap->textureUnit = GL_TEXTURE7;
	//depthmap->textureUnitNumber = 7;
	// dummy color map

	//Texture * colormap = new Texture(GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, NULL, resolution_x, resolution_y, "dummy_colorMap");
	//colormap->textureUnit = GL_TEXTURE8;
	//colormap->textureUnitNumber = 8;

// get "slice thickness"
	BBox * box = getBBox();
	float distance = -0.5f;
	v3 position = direction * distance;
	float diameter = 0.7*box->getMinSize();
	float radius = diameter*0.5f;
	float thickness = diameter/(float(count));
	if (half) thickness*=0.5;
	float left = -0.5, right= 0.5, bottom= 0.0, top= 1.0, near, far;
	float positionDist = position.length();
	DTreeSlice * slice;
	int i;

	// get MVP matrix:
	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		near = 0.0;
		far = 1.0;
		glOrtho(left, right, bottom, top, near, far);
		// get P matrix:
		Matrix4x4 P;
		
		glGetFloatv(GL_PROJECTION_MATRIX, (P.m) );
		//P.printOut();
		glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		gluLookAt( position.x, position.y, position.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		// get MV matrix:
		Matrix4x4 MV;
		glGetFloatv(GL_MODELVIEW_MATRIX, (MV.m));
		//MV.printOut();
		glPopMatrix();
	/// compute ModelViewProjection matrix
		Matrix4x4 LocMVP = P*MV;
		memcpy(mvp->m, LocMVP.m, sizeof(float)*16);


	
	// clear previous slices
	/*
	for ( i = 0; i < slices.size(); i++){
			delete slices[i];
		}
	*/
	slices->clear();
	GLuint fbo = 0;

	for ( i = 0; i< count; i++){
		// create FBO
			glGenFramebuffersEXT(1, &fbo);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		// each slice
			slice = new DTreeSlice();
		// add slice
			slices->push_back(slice);
		// create & setup textures
			slice->colormap = new Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, resolution_x, resolution_y, "colorMap");
			slice->colormap->textureUnit = GL_TEXTURE0;
			slice->colormap->textureUnitNumber = 0;
			slice->colormap->setParameterI(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			slice->colormap->setParameterI(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			char buff[100];
			ctr++;
			sprintf(buff, "color texture %i", ctr);
			slice->colormap->description = buff;

			slice->depthmap = new Texture(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, resolution_x, resolution_y, "depthMap");
			slice->depthmap->textureUnit = GL_TEXTURE2;
			slice->depthmap->textureUnitNumber = 2;


			slice->normalmap = new Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, resolution_x, resolution_y, "normalMap");
			slice->normalmap->textureUnit = GL_TEXTURE3;
			slice->normalmap->textureUnitNumber = 3;
			
			slice->branchmap = new Texture(GL_TEXTURE_2D, eBranchmapInternalFormat, eBranchmapFormat, eBranchmapDataType, NULL, resolution_x, resolution_y, "branchMap");
			
			//slice->branchmap = new Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, resolution_x, resolution_y, "branchMap");
			slice->branchmap->textureUnit = GL_TEXTURE4;
			slice->branchmap->textureUnitNumber = 4;

		// attach textures to FBO attachments

			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, slice->colormap->id  , 0);
			//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, colormap->id , 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, slice->normalmap->id , 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT3_EXT, GL_TEXTURE_2D, slice->branchmap->id , 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, slice->depthmap->id  , 0);
			//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			

			glClearColor(0.f, 0.f, 0.f, 0.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//GLenum buffers[4] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT};
			//glDrawBuffersARB(4, buffers);
			GLenum buffers[3] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_COLOR_ATTACHMENT3_EXT};
			glDrawBuffersARB(3, buffers);
			assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT);
			assert( glGetError() == GL_NO_ERROR );
			//printf("TREE_SLICE %i framebuffer initialized successfully\n", i);
			/*
			GLenum buffers[3] = {GL_COLOR_ATTACHMENT0_EXT,GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
			glDrawBuffersARB(3, buffers);
			*/

		// setup near & far plane
			near = positionDist-radius + i * thickness;
			far = near + thickness;
			// last interval must continue to infinity in case of half slicing
			if (half && i==count-1)
			{
				far = positionDist-radius + 2*(i+1) * thickness;
			}
		// setup camera to orthoprojection with respect to slice interval
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
				glLoadIdentity();
				g_window_sizes.x = resolution_x;
				g_window_sizes.y = resolution_y;
				glViewport(0, 0, g_window_sizes.x, g_window_sizes.y);
				//system("PAUSE");
				//glMultMatrixf(P.m);
				glOrtho(left, right, bottom, top, near, far);
				
			glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glLoadIdentity();
				//glMultMatrixf(MV.m);
				gluLookAt( position.x, position.y, position.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		// render offscreen
		// draw tree now...
			drawForLOD();
			glFinish();
			


		glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		//
	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		// prepare data texture from branch texture
		glDeleteFramebuffersEXT(1, &fbo);
		glGenFramebuffersEXT(1, &fbo);
		glDisable(GL_DEPTH_TEST);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
			slice->datamap = new Texture(GL_TEXTURE_2D, eBranchmapInternalFormat, eBranchmapFormat, eBranchmapDataType, NULL, resolution_x, resolution_y, "dataMap");
			slice->datamap->textureUnit = GL_TEXTURE5;
			slice->datamap->textureUnitNumber = 5;
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, slice->datamap->id , 0);
			//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D, depthmap->id , 0);
			assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT);
			assert( glGetError() == GL_NO_ERROR );
			//printf("TREE_SLICE %i data framebuffer initialized successfully\n", i);

			glClearColor(1.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);
			//glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
			GLenum buffers2[1] = {GL_COLOR_ATTACHMENT0_EXT};
			glDrawBuffersARB(1, buffers2);
			
			// activate shader
			//slice->branchmap->bind(GL_TEXTURE0);
			dataProcessShader->use(true);
			slice->branchmap->bind(slice->branchmap->textureUnit);
			dataProcessShader->setTexture(gl_location, slice->branchmap->textureUnitNumber);			
			dataProcessShader->setUniform2f(loc_win_size,	g_window_sizes.x, g_window_sizes.y);
			slice->branchmap->show(0,0, g_window_sizes.x, g_window_sizes.y);
			dataProcessShader->use(false);
			//slice->branchmap->unbind();
			glFinish();
			glEnable(GL_DEPTH_TEST);
		// return to normal screen rendering	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
		glDeleteFramebuffersEXT(1, &fbo);
		//*/
		glDrawBuffer(GL_BACK);

		g_window_sizes.x = g_WinWidth;
		g_window_sizes.y = g_WinHeight;
		glViewport(0, 0, g_window_sizes.x, g_window_sizes.y);
		
		// delete branch texture
		SAFE_DELETE_PTR ( slice->branchmap );

	} // for each slice
	//SAFE_DELETE_PTR ( depthmap );
	SAFE_DELETE_PTR ( dataProcessShader );
}

void DTree::joinSliceSetsTextures(
					Texture ** jColorMap,
					Texture ** jDataMap,
					Texture ** jDepthMap,
					Texture ** jNormalMap,
					vector<DTreeSliceSet*>* sliceSets,
					int sliceset_count,
					int slice_count,
					int resolution_x,
					int resolution_y
					)
{

	Shader * joinShader = new Shader("join");
	joinShader->loadShader(DYN_TREE::SHADER_JOIN_V, DYN_TREE::SHADER_JOIN_F);
	int loc_colormap	= joinShader->getGLLocation("colorMap");	
	int loc_datamap		= joinShader->getGLLocation("dataMap");			
	int loc_normalmap	= joinShader->getGLLocation("normalMap");		
	int loc_depthmap	= joinShader->getGLLocation("depthMap");		
	//int sliceSetCnt = lod1_sliceSets.size();
	GLuint fbo = 0;
	int jResX = slice_count		* resolution_x;
	int jResY = sliceset_count  * resolution_y;

	// render offscreen
	g_window_sizes.x = jResX;
	g_window_sizes.y = jResY;
	glViewport(0, 0, g_window_sizes.x, g_window_sizes.y);

	(*jColorMap)	= new Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, jResX, jResY, "colorMap");
	(*jColorMap)->setParameterI(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//lod1_jColorMap->setParameterI(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	(*jColorMap)->textureUnit =  GL_TEXTURE1;
	(*jColorMap)->textureUnitNumber =		1;
	(*jDataMap)	= new Texture(	GL_TEXTURE_2D, 
								eBranchmapInternalFormat,
								eBranchmapFormat,
								eBranchmapDataType,
								NULL, jResX, jResY, "dataMap");
	(*jDataMap)->textureUnit =	  GL_TEXTURE2;
	(*jDataMap)->textureUnitNumber =		2;
	(*jDepthMap)	= new Texture(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, NULL, jResX, jResY, "depthMap");
	(*jDepthMap)->textureUnit =  GL_TEXTURE3;
	(*jDepthMap)->textureUnitNumber =		3;
	(*jNormalMap)	= new Texture(GL_TEXTURE_2D, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, NULL, jResX, jResY, "normalMap");
	(*jNormalMap)->setParameterI(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//lod1_jNormalMap->setParameterI(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	(*jNormalMap)->textureUnit = GL_TEXTURE4;
	(*jNormalMap)->textureUnitNumber =     4;

	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,	(*jColorMap	)->id  , 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D,	(*jNormalMap)->id  , 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D,	(*jDataMap	)->id  , 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT , GL_TEXTURE_2D,	(*jDepthMap	)->id  , 0);
		assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT);
		assert( glGetError() == GL_NO_ERROR );

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum buffers[3] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
		glDrawBuffersARB(3, buffers);
		DTreeSlice* slice;
		// draw slices
		//glDisable(GL_DEPTH_TEST);

		joinShader->use(true);
		int x,y,width,height;
		int i,j;
		for (j=0; j<sliceset_count; j++){
			// draw slice sets
			for (i=0; i<slice_count; i++){
				// draw on proper position
				slice	= (*sliceSets)[j]->getSlice(i); 
				x		= i*resolution_x;
				y		= j*resolution_y;
				width	= resolution_x;
				height	= resolution_y;

				//slice->colormap->	bind(slice->colormap	->textureUnit);
				slice->datamap->	bind(slice->datamap		->textureUnit);
				slice->normalmap->	bind(slice->normalmap	->textureUnit);
				slice->depthmap->	bind(slice->depthmap	->textureUnit);

				joinShader->setTexture(loc_colormap,	slice->colormap->	textureUnitNumber);
				joinShader->setTexture(loc_datamap,		slice->datamap->	textureUnitNumber);
				joinShader->setTexture(loc_normalmap,	slice->normalmap->	textureUnitNumber);
				joinShader->setTexture(loc_depthmap,	slice->depthmap->	textureUnitNumber);
								
				slice->colormap->show(x, y, width, height, false);	

				//slice->colormap->	unbind();
				slice->datamap->	unbind();
				slice->normalmap->	unbind();
				slice->depthmap->	unbind();
				glFinish();
			}
		}
		
		joinShader->use(false);
		
		//glEnable(GL_DEPTH_TEST);
	/*glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
		glPopMatrix();*/
		
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &fbo);
	glDrawBuffer(GL_BACK);

	g_window_sizes.x = g_WinWidth;
	g_window_sizes.y = g_WinHeight;
	glViewport(0, 0, g_window_sizes.x, g_window_sizes.y);

	// system("PAUSE");
}
