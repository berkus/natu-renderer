#include "Tree2.h"


Tree2::Tree2(TextureManager *texManager, ShaderManager *shManager):
	Vegetation(texManager, shManager)
{
	textureId	= 0;
	vboId		= 0;
	shader		= NULL;
}
Tree2::Tree2(Tree2* copy):
	Vegetation(copy->textureManager, copy->shaderManager)
{
	textureId	= copy->textureId;
	vboId		= copy->vboId;
	shader		= copy->shader;
	shaderId	= copy->shaderId;
	LCmatrixLoc = copy->LCmatrixLoc;
	fastModeLoc	= copy->fastModeLoc;
	shadowMappingEnabledLoc = copy->shadowMappingEnabledLoc;
}

Tree2::~Tree2(void)
{
	glDeleteBuffers(1, &vboId);
	printf("Tree2 is deleted\n");
}

void Tree2::bakeToVBO()
{
	if (vboId>0){
		glDeleteBuffers(1, &vboId);
	}
	// vbo
	VBOdataCount = vertices.size();
	int vboSize = VBOdataCount*(VertexInfo::sizePosition+VertexInfo::sizeNormal+VertexInfo::sizeTexCoord);
	VBOdataSize  = vboSize*sizeof(GLfloat);
	pVBOdata = new float[vboSize];//CROSS_VERTEX_ARRAY;

	// fill vbo array
	for (int i=0; i<VBOdataCount; i++)
	{
		VertexInfo v = vertices[i];
		int p = v.sizePosition*i;
		pVBOdata[p + 0] = v.position.x;
		pVBOdata[p + 1] = v.position.y;
		pVBOdata[p + 2] = v.position.z;

		int o = v.sizePosition*VBOdataCount;
		pVBOdata[o + p + 0] = v.normal.x;
		pVBOdata[o + p + 1] = v.normal.y;
		pVBOdata[o + p + 2] = v.normal.z;

		o += v.sizeNormal*VBOdataCount;
		p = v.sizeTexCoord*i;
		pVBOdata[o + p + 0] = v.texCoord.x;
		pVBOdata[o + p + 1] = v.texCoord.y;
	}
	// offsets
	offsets.position = 0;
	offsets.normal	 = offsets.position + VBOdataCount * VertexInfo::sizePosition * sizeof(GLfloat);
	offsets.texCoord = offsets.normal   + VBOdataCount * VertexInfo::sizeNormal * sizeof(GLfloat);

	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, VBOdataSize, pVBOdata, GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*
	printf("VBO\n");
	int size = VBOdataSize/sizeof(GLfloat);
	for (int i=0; i<size; i++)
	{
		if (i==offsets.position/sizeof(GLfloat)){
			printf ("\npositions\n");
		}
		if (i==offsets.normal/sizeof(GLfloat)){
			printf ("\nnormals\n");
		}
		if (i==offsets.texCoord/sizeof(GLfloat)){
			printf ("\ntexcoords\n");
		} 
		printf(" %f", pVBOdata[i]);
	} 
	*/
	// ebo
	pEBOdata = NULL;
	EBOdataCount= 0;
}

void Tree2::init()
{
	// shader
	shaderId = shaderManager->loadShader("Tree2", TREE1_VS_FILENAME, TREE1_FS_FILENAME);
	shader = shaderManager->getShader(shaderId);
	// shadow map
	shader->linkTexture(textureManager->getTexture(textureManager->shadowMapID));
	LCmatrixLoc					= shader->getLocation("LightMVPCameraVInverseMatrix");
	shadowMappingEnabledLoc		= shader->getLocation("shadowMappingEnabled");
	fastModeLoc					= shader->getLocation("fastMode");
	
	// textury
	textureId = textureManager->loadTexture(TREE2_TEX_NAME, "tree1_tex", 0, false);
	//textureWaveId = textureManager->loadTexture(TREE1_TEX_NAME, "tree_wave_tex", 1, false, GL_REPEAT, GL_NEAREST);
	shader->linkTexture(textureManager->getTexture(textureId));
	//shader->linkTexture(textureManager->getTexture(textureWaveId));
	VBOdataCount = cross_vertexCount;
	// fill vertices...
	float scale = TREE2_SCALE;
	for (int i=0; i<cross_vertexCount; i++)
	{
		VertexInfo v;

		int p = v.sizePosition*i;
		v.position.x = CROSS_VERTEX_ARRAY[p + 0]*scale;
		v.position.y = CROSS_VERTEX_ARRAY[p + 1]*scale;
		v.position.z = CROSS_VERTEX_ARRAY[p + 2]*scale;
		v.position.w = 1.f;

		int o = v.sizePosition*cross_vertexCount;
		v.normal.x = CROSS_VERTEX_ARRAY[o + p + 0];
		v.normal.y = CROSS_VERTEX_ARRAY[o + p + 1];
		v.normal.z = CROSS_VERTEX_ARRAY[o + p + 2];
		v.normal.w = 0.f;

		p = v.sizeTexCoord*i;
		o += v.sizeNormal*cross_vertexCount;
		v.texCoord.x = CROSS_VERTEX_ARRAY[o + p + 0];
		v.texCoord.y = CROSS_VERTEX_ARRAY[o + p + 1];

		/*
		printf("VERTEX: \n");
		printf("\tpos: %f %f %f\n", v.position.x, v.position.y, v.position.z);
		printf("\tnor: %f %f %f\n", v.normal.x,   v.normal.y,   v.normal.z);
		printf("\ttex: %f %f\n",v.texCoord.x, v.texCoord.y);
		*/
		vertices.push_back(v);
	}
	
}

void Tree2::update(double time)
{

}

Vegetation* Tree2::getCopy()
{
	Tree2 * copy = new Tree2(this);
	return copy;
}

void Tree2::draw()
{
	textureManager->bindTexture(textureId, GL_TEXTURE0);
	//textureManager->bindTexture(textureWaveId, GL_TEXTURE1);
	shader->use(true);
	//shader->setTime(g_time);
	shader->setBoolean(fastModeLoc, g_fastMode);
	shader->setBoolean(shadowMappingEnabledLoc, g_ShadowMappingEnabled);
//	shader->setUniformMatrix(LCmatrixLoc, g_LightMVPCameraVInverseMatrix );

	glDisable(GL_CULL_FACE);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glVertexPointer(3,GL_FLOAT, 0, BUFFER_OFFSET(offsets.position));
			glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(offsets.normal));
			
			//glActiveTexture(GL_TEXTURE0);
			//glClientActiveTexture(GL_TEXTURE0);
			
			glTexCoordPointer(2,GL_FLOAT, 0, BUFFER_OFFSET(offsets.texCoord));
			glDrawArrays(GL_QUADS, 0, vertices.size());
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnable(GL_CULL_FACE);
	shader->use(false);
	//textureManager->unbindTexture(textureWaveId);
	textureManager->unbindTexture(textureId);
}

void Tree2::fixTexType()
{
}

v3	 Tree2::transformTexCoords(v3 &origTexCoords)
{
	// for given type return proper texcoords
	return origTexCoords;
}
