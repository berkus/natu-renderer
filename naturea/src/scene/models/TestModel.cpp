#include "TestModel.h"


TestModel::TestModel(void)
{
	shader				= NULL;
	vbo					= NULL;
	ebo					= NULL;
	
}


TestModel::~TestModel(void)
{
	SAFE_DELETE_PTR (shader					);
	SAFE_DELETE_PTR (vbo					);
	SAFE_DELETE_PTR (ebo					);
	SAFE_DELETE_ARRAY_PTR(	type1Matrices	);
	SAFE_DELETE_ARRAY_PTR(	type2Matrices	);
	SAFE_DELETE_ARRAY_PTR(	type1Param1		);
	SAFE_DELETE_ARRAY_PTR(	type2Param1		);
}

void TestModel::enqueueInRenderList(TestInstance * instance){
	//if(instance->discrepacy>0.5){
		if (instance->distance>10.0){
			memcpy( type1Matrices+type1Index*16, instance->transformMatrix.m, 16*sizeof(float));
			type1Param1[type1Index] = 0.3; //instance->param1;
			type1Index += 1;
		} else {
			memcpy( type2Matrices+type2Index*16, instance->transformMatrix.m, 16*sizeof(float));
			type2Param1[type2Index] = 0.8; //instance->param1;
			type2Index += 1;
		}
	//}
}

void TestModel::prepareForRender(){
	type1Index = 0;
	type2Index = 0;
	
	// go through instances, perform one bubble sort walktrough & split into different render queues
	TestInstance * instance;
	int i;
	int instanceCount = instances.size();
	TestInstance * act_instance = instances[0] ;
	TestInstance * next_instance;
	act_instance->eye_dir = act_instance->position - *g_viewer_position;
	act_instance->distance =act_instance->eye_dir.length();
	act_instance->eye_dir.normalize();
	act_instance->discrepacy = g_viewer_direction->dot(act_instance->eye_dir);
	
	TestInstance* done_instance;
	// go through instances, perform one bubble sort walktrough & split into different render queues
	
	//int i=1;
	TestInstance * orphan = act_instance;
	for (int i=1; i<instances.size(); i++){
		next_instance =instances[i];
		next_instance->eye_dir = next_instance->position - *g_viewer_position;
		next_instance->distance= next_instance->eye_dir.length();
		next_instance->eye_dir.normalize();
		next_instance->discrepacy = g_viewer_direction->dot(next_instance->eye_dir);
		if (next_instance->distance>act_instance->distance){
			// swap
			instances[i-1]	= next_instance;
			instances[i]	= act_instance;
			orphan = NULL;
			enqueueInRenderList( next_instance );
		} else {
			enqueueInRenderList( act_instance );
			act_instance	= next_instance;
			orphan = act_instance;
		}
	} // loop over instances
	if (orphan!=NULL){
		enqueueInRenderList( orphan );
	}
	printf("grid: %i, count: %i\n", g_tree_gridSize* g_tree_gridSize, type1Index + type2Index);
}



void TestModel::render(){
	glDisable(GL_CULL_FACE);
	glUseProgram(shader->programID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_indicesBuffID);
	// bind positions data
	glBindBuffer(GL_ARRAY_BUFFER, v_positionsBuffID);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));   //The starting point of the VBO, for the vertices
	
	glEnableVertexAttribArray(p1Loc);
	glEnableVertexAttribArray(tmLoc0);
	glEnableVertexAttribArray(tmLoc1);
	glEnableVertexAttribArray(tmLoc2);
	glEnableVertexAttribArray(tmLoc3);
	// bind instance data

	// draw type1
	glBindBuffer(GL_ARRAY_BUFFER, i_paramBuffID);
	glBufferData(GL_ARRAY_BUFFER, type1Index * 1 * sizeof(float), type1Param1, GL_STREAM_DRAW);
	glVertexAttribPointer(p1Loc, 1, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glVertexAttribDivisor(p1Loc, 1);
	glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
	
	glVertexAttribPointer(tmLoc0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(0));
	glVertexAttribPointer(tmLoc1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(tmLoc2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(tmLoc3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4 * 4, (void*)(sizeof(float) * 12));
	glVertexAttribDivisor(tmLoc0, 1);
	glVertexAttribDivisor(tmLoc1, 1);
	glVertexAttribDivisor(tmLoc2, 1);
	glVertexAttribDivisor(tmLoc3, 1);	

	glBufferData(GL_ARRAY_BUFFER, type1Index * 16 * sizeof(float), type1Matrices, GL_STREAM_DRAW);
	glDrawElementsInstanced(GL_QUADS, 8, GL_UNSIGNED_INT, BUFFER_OFFSET(0), type1Index);

	// draw type2
	glBufferData(GL_ARRAY_BUFFER, type2Index * 16* sizeof(float), type2Matrices, GL_STREAM_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, i_paramBuffID);
	glBufferData(GL_ARRAY_BUFFER, type2Index * 1 * sizeof(float), type2Param1, GL_STREAM_DRAW);
	glVertexAttribPointer(p1Loc, 1, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glVertexAttribDivisor(p1Loc, 1);
	glDrawElementsInstanced(GL_QUADS, 8, GL_UNSIGNED_INT, BUFFER_OFFSET(8*sizeof(unsigned int)), type2Index);  


	// disable all...
	glDisableVertexAttribArray(tmLoc0);
	glDisableVertexAttribArray(tmLoc1);
	glDisableVertexAttribArray(tmLoc2);
	glDisableVertexAttribArray(tmLoc3);
	glDisableVertexAttribArray(p1Loc);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glEnable(GL_CULL_FACE);
	glUseProgram(0);
}

void TestModel::draw()
{
	prepareForRender();
	render();	
}

void TestModel::drawForLOD()
{

}

void TestModel::init()
{
	// init instances
	int grid_size = g_tree_gridSize; // squared
	int instanceCount = grid_size*grid_size;
	matricesBufferData = new float[instanceCount*16];
	// create instances
	TestInstance * instance;
	float step = g_tree_mean_distance;
	float cosA, sinA;
	for (int i=0; i<grid_size; i++){
		for (int j=0; j<grid_size; j++){
			instance = new TestInstance();
			instance->position.x = i*step - 0.5*grid_size*step + randomf(-g_tree_dither, g_tree_dither);
			instance->position.z = j*step - 0.5*grid_size*step + randomf(-g_tree_dither, g_tree_dither);
			float xt	= instance->position.x + terrain->sz_x/2.0;
			float yt	= instance->position.z + terrain->sz_y/2.0;
			instance->position.y	= terrain->getHeightAt(xt,yt);


			instance->rotation_y = randomf(0.0, 90.0);
			cosA = cos(DEG_TO_RAD*instance->rotation_y);
			sinA = sin(DEG_TO_RAD*instance->rotation_y);

			m4 matrix(cosA			,	0			,	-sinA	,	0	,
					  0				,	1			,	0		,	0	,
					  sinA			,	0			,	cosA	,	0	,
					  instance->position.x	,	instance->position.y	,	instance->position.z		,	1	);

			instance->transformMatrix = matrix; // copy matrix
			instance->param1 = float(i)/float(grid_size);
			instances.push_back( instance );
		}
	}

	// init shaders

	shader = new Shader("test");
	shader->loadShader("shaders/test_instancing_vs.glsl", "shaders/test_instancing_fs.glsl");

	// init VBOs
	const float CROSS_ARRAY[3*8] = {	-1	,0	,0	,
										1	,0	,0	,
										1	,2	,0	,
										-1	,2	,0	,
										0	,0	,-1	,
										0	,0	,1	,
										0	,2	,1	,
										0	,2	,-1	,
									};
	const unsigned int INDICES[16] ={0,1,2,3,4,5,6,7,
									 7,6,5,4,3,2,1,0};
	// instance matrices
	i_matricesBuffID = 0;
	glGenBuffers(1, &i_matricesBuffID);
	glBindBuffer(GL_ARRAY_BUFFER, i_matricesBuffID);
		// load data
		glBufferData(GL_ARRAY_BUFFER, instanceCount * 16* sizeof(float), NULL, GL_STREAM_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// instance params
	glGenBuffers(1, &i_paramBuffID);
	glBindBuffer(GL_ARRAY_BUFFER, i_paramBuffID);
		// load data
		glBufferData(GL_ARRAY_BUFFER, instanceCount * 1 * sizeof(float), NULL, GL_STREAM_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &v_positionsBuffID);
	glBindBuffer(GL_ARRAY_BUFFER, v_positionsBuffID);
		// load data
		glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), CROSS_ARRAY, GL_STATIC_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glGenBuffers(1, &v_indicesBuffID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v_indicesBuffID);
		// load data
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 16 * sizeof(unsigned int), INDICES, GL_STATIC_DRAW);  
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	p1Loc			= glGetAttribLocation(shader->programID, "param1");
	tmLoc			= glGetAttribLocation(shader->programID, "transformMatrix");
	tmLoc0			= tmLoc + 0;
	tmLoc1			= tmLoc + 1;
	tmLoc2			= tmLoc + 2;
	tmLoc3			= tmLoc + 3;
	type1Matrices	= new float[16*instances.size()];
	type2Matrices	= new float[16*instances.size()];
	type1Param1		= new float[instances.size()];
	type2Param1		= new float[instances.size()];
}

void TestModel::update(double time)
{

}
