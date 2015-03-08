#include "Terrain.h"


Terrain::Terrain(TextureManager *texManager, ShaderManager *shManager):
	SceneModel(texManager, shManager)
{
	heightMap		= NULL;
	dim_x			= 0;
	dim_y			= 0;
	drawingMethod	= GL_TRIANGLE_STRIP;
	vboId			= 0;
	vboCount		= 0;
	eboId			= 0;
	eboCount		= 0;
	vertices 		= NULL;
	normals  		= NULL;
	elements 		= NULL;
	shader			= NULL;
	cut				= false;
	flip			= false;
}


Terrain::~Terrain(void)
{
	glDeleteBuffers(1, &vboId);
	glDeleteBuffers(1, &eboId);
	SAFE_DELETE_ARRAY_PTR( heightMap );
	SAFE_DELETE_ARRAY_PTR(vertices);
	SAFE_DELETE_ARRAY_PTR(normals);
	SAFE_DELETE_ARRAY_PTR(elements);
}


void  Terrain::drawForLOD()
{
	//material->use();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId); 
		// enable states
		glEnableClientState(GL_VERTEX_ARRAY);
		// draw VBOs...
		glVertexPointer(channels[VERTEX], glTypes[VERTEX], 0, BUFFER_OFFSET(offsets[VERTEX]));
		glDrawElements(GL_TRIANGLE_STRIP, eboCount, GL_UNSIGNED_INT, BUFFER_OFFSET(offsets[INDEX]));
		// disable
		glDisableClientState(GL_VERTEX_ARRAY);
	// unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Terrain::draw()
{	
	//material->use();
	// bind textures
	int i;
	for (i=0; i<TERRAIN_TEX_COUNT; i++){
		textureManager->bindTexture(textureIds[i], GL_TEXTURE0+GLuint(i));
	}
	g_shadowmap1->bind(GL_TEXTURE0+GLuint(i+1));
	shader->use(true);
	shader->setUniform4v(border_values_location, g_terrain_border_values);
	shader->setUniform4v(border_widths_location, g_terrain_border_widths);
	shader->setBoolean(fastModeLoc, g_fastMode);
	//shader->setBoolean(shadowMappingEnabledLoc, g_ShadowMappingEnabled);
	//shader->setUniformMatrix(LCmatrixLoc, g_LightMVPCameraVInverseMatrix );
	//shader->setUniformMatrix(LMV_CVImatrixLoc, g_LightMVCameraVInverseMatrix );
	//shader->setUniformMatrix(LPmatrixLoc, g_LightPMatrix );
	if (cut){
		if (flip){
			shader->setUniform2f(heightInterval_location, WATER_HEIGHT, 1000.f);
		}
		else{
			shader->setUniform2f(heightInterval_location, -1000.f, WATER_HEIGHT);
		} 
	} else {
		shader->setUniform2f(heightInterval_location, -1000.f, 1000.f);
	}
	cut		= false;
	flip	= false;
		// bind index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId); 
		   // enable states
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		   // draw VBOs...
			glVertexPointer(channels[VERTEX], glTypes[VERTEX], 0, BUFFER_OFFSET(offsets[VERTEX]));
			glNormalPointer(glTypes[NORMAL], 0, BUFFER_OFFSET( offsets[NORMAL] ) );
			
			glActiveTexture(GL_TEXTURE0);
			glClientActiveTexture(GL_TEXTURE0);
			
			glTexCoordPointer(channels[TEXCOORD0], glTypes[TEXCOORD0], 0, BUFFER_OFFSET(offsets[TEXCOORD0]));
			
			glDrawElements(GL_TRIANGLE_STRIP, eboCount, GL_UNSIGNED_INT, BUFFER_OFFSET(offsets[INDEX]));
			
		   // disable
		   
		   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		   glDisableClientState(GL_NORMAL_ARRAY);
		   glDisableClientState(GL_VERTEX_ARRAY);
		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// unbind textures
		for (int i=0; i<TERRAIN_TEX_COUNT; i++){
			textureManager->unbindTexture(textureIds[i]);
		}
		g_shadowmap1->unbind();
	// turn off shader
	shader->use(false);
}

void Terrain::drawOverWater(){
	// bind textures
	material->use();
	for (int i=0; i<TERRAIN_TEX_COUNT; i++){
		textureManager->bindTexture(textureIds[i], GL_TEXTURE0+GLuint(i));
	}
	textureManager->bindTexture(textureManager->shadowMapID, GL_TEXTURE0+GLuint(7));
	shader->use(true);
	shader->setUniform4v(border_values_location, g_terrain_border_values);
	shader->setUniform4v(border_widths_location, g_terrain_border_widths);
	shader->setUniform2f(heightInterval_location, WATER_HEIGHT, 1000.f);
	shader->setBoolean(fastModeLoc, g_fastMode);
	shader->setBoolean(shadowMappingEnabledLoc, g_ShadowMappingEnabled);
	//shader->setUniformMatrix(LCmatrixLoc, g_LightMVPCameraVInverseMatrix );
	//shader->setUniformMatrix(LMV_CVImatrixLoc, g_LightMVCameraVInverseMatrix );
	//shader->setUniformMatrix(LPmatrixLoc, g_LightPMatrix );
		// bind index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId); 
		   // enable states
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		   // draw VBOs...
			glVertexPointer(channels[VERTEX], glTypes[VERTEX], 0, BUFFER_OFFSET(offsets[VERTEX]));
			glNormalPointer(glTypes[NORMAL], 0, BUFFER_OFFSET( offsets[NORMAL] ) );
			
			//glActiveTexture(GL_TEXTURE0);
			//glClientActiveTexture(GL_TEXTURE0);
			
			glTexCoordPointer(channels[TEXCOORD0], glTypes[TEXCOORD0], 0, BUFFER_OFFSET(offsets[TEXCOORD0]));
			
			glDrawElements(GL_TRIANGLE_STRIP, eboCount, GL_UNSIGNED_INT, BUFFER_OFFSET(offsets[INDEX]));
			
		   // disable
		   
		   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		   glDisableClientState(GL_NORMAL_ARRAY);
		   glDisableClientState(GL_VERTEX_ARRAY);
		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// unbind textures
		for (int i=0; i<TERRAIN_TEX_COUNT; i++){
			textureManager->unbindTexture(textureIds[i]);
		}
	// turn off shader
	shader->use(false);
	flip = false;
}

void Terrain::drawUnderWater(){
	// bind textures
	material->use();
	for (int i=0; i<TERRAIN_TEX_COUNT; i++){
		textureManager->bindTexture(textureIds[i], GL_TEXTURE0+GLuint(i));
	}
	shader->use(true);
	shader->setUniform4v(border_values_location, g_terrain_border_values);
	shader->setUniform4v(border_widths_location, g_terrain_border_widths);
	
	shader->setUniform2f(heightInterval_location,-1000.f, WATER_HEIGHT+1.f);
	shader->setUniformMatrix(LMV_CVImatrixLoc, g_LightMVCameraVInverseMatrix );
	shader->setUniformMatrix(LPmatrixLoc, g_LightPMatrix );
		
		// bind index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboId);
		glBindBuffer(GL_ARRAY_BUFFER, vboId); 
		   // enable states
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		   // draw VBOs...
			glVertexPointer(channels[VERTEX], glTypes[VERTEX], 0, BUFFER_OFFSET(offsets[VERTEX]));
			glNormalPointer(glTypes[NORMAL], 0, BUFFER_OFFSET( offsets[NORMAL] ) );
			
			//glActiveTexture(GL_TEXTURE0);
			//glClientActiveTexture(GL_TEXTURE0);
			
			glTexCoordPointer(channels[TEXCOORD0], glTypes[TEXCOORD0], 0, BUFFER_OFFSET(offsets[TEXCOORD0]));
			
			glDrawElements(GL_TRIANGLE_STRIP, eboCount, GL_UNSIGNED_INT, BUFFER_OFFSET(offsets[INDEX]));
			
		   // disable
		   
		   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		   glDisableClientState(GL_NORMAL_ARRAY);
		   glDisableClientState(GL_VERTEX_ARRAY);
		// unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		// unbind textures
		for (int i=0; i<TERRAIN_TEX_COUNT; i++){
			textureManager->unbindTexture(textureIds[i]);
		}
	// turn off shader
	shader->use(false);
}

void Terrain::drawNormals()
{
	int numVertices = vboCount/3;
	glDisable(GL_LIGHTING);
	glColor(v3(1.f,0.f,0.f));
	glBegin(GL_LINES);
	float x,y,z;
	for (int i=0; i<numVertices; i++){
		x = vertices[i*3 + 0];
		y = vertices[i*3 + 1];
		z = vertices[i*3 + 2];
		glVertex3f(x,y,z);
		x += normals[i*3 + 0];
		y += normals[i*3 + 1];
		z += normals[i*3 + 2];
		glVertex3f(x,y,z);
	}
	glEnd();
	glEnable(GL_LIGHTING);
}
	 
void Terrain::init()
{
	// material
	material = new Material(v4(0.2), v4(0.7), v4(0.0), 0.5);

	// load & create shaders
	int shaderID = shaderManager->loadShader("Terrain", TERRAIN_VS_FILENAME, TERRAIN_FS_FILENAME);
	shader = shaderManager->getShader(shaderID);
	border_values_location = shader->getLocation("border_values");
	border_widths_location = shader->getLocation("border_widths");
	heightInterval_location = shader->getLocation("visibleHeightInterval");
	if (g_shadowmap1!=NULL){
		shader->linkTexture(g_shadowmap1);

	}
	// load heightmap
	int resolution_x = TERRAIN_RESOLUTION_X;
	int resolution_y = TERRAIN_RESOLUTION_Y;
	float size_x = TERRAIN_SIZE_X;
	float size_y = TERRAIN_SIZE_Y;
	float step_x =size_x/float(resolution_x);
	float step_y =size_y/float(resolution_y);
	red_x = 1.f/step_x;
	red_y = 1.f/step_y;

	loadHeightMap(HEIGHTMAP_SOURCE, resolution_x, resolution_y);

	// load textures
	loadTextures(TERRAIN_TEX_NAME, TERRAIN_TEX_COUNT);

	// shadow map
	shader->linkTexture(g_shadowmap1);
	shader->registerUniform("LightMVPCameraVInverseMatrix", UniformType::M4, g_LightMVPCameraVInverseMatrix);
	shader->registerUniform("shadowMappingEnabled", UniformType::B1, & g_ShadowMappingEnabled);
	shader->registerUniform("shadowmap_size", UniformType::F2, & g_ShadowMapSize.data);	
	shader->registerUniform("samples", UniformType::I1, & g_shadowMapSamples);
	
	//LCmatrixLoc					= shader->getLocation("LightMVPCameraVInverseMatrix");
	//shadowMappingEnabledLoc		= shader->getLocation("shadowMappingEnabled");
	fastModeLoc					= shader->getLocation("fastMode");
	//LMV_CVImatrixLoc			= shader->getLocation("LightMViewCameraViewInverseMatrix");
	//LPmatrixLoc					= shader->getLocation("LightProjectionMatrix");


	dim_x = resolution_x;
	dim_y = resolution_y;
	drawingMethod = GL_TRIANGLE_STRIP;
	
	// create grid of triangles
	glGenBuffers(1, &eboId);
	glGenBuffers(1, &vboId);
	int ch = 3;

	
	vertices = new GLfloat[dim_x*dim_y*ch];
	normals  = new GLfloat[dim_x*dim_y*ch];
	texCoords  = new GLfloat[dim_x*dim_y*2];
	elements = new GLuint[(dim_x-1)*2*dim_y];

	channels	[INDEX]		= 1;
	channels	[VERTEX]	= 3;
	channels	[NORMAL]	= 3;
	channels	[TEXCOORD0] = 2;

	typeSizes	[INDEX]		= sizeof(GLuint);
	typeSizes	[VERTEX]	= sizeof(GLfloat);
	typeSizes	[NORMAL]	= sizeof(GLfloat);
	typeSizes	[TEXCOORD0] = sizeof(GLfloat);

	glTypes		[INDEX]		= GL_UNSIGNED_INT;
	glTypes		[VERTEX]	= GL_FLOAT;
	glTypes		[NORMAL]	= GL_FLOAT;
	glTypes		[TEXCOORD0] = GL_FLOAT;

	for (int k = 0; k < VBO_ATR_COUNT; k++){
			sizes[k]=0;
	}
	sizes		[INDEX]		= (dim_x-1)*2*dim_y * channels[INDEX]	* typeSizes[INDEX];
	sizes		[VERTEX]	= dim_x * dim_y * channels[VERTEX]	* typeSizes[VERTEX];
	sizes		[NORMAL]	= dim_x * dim_y * channels[NORMAL]	* typeSizes[NORMAL];
	sizes		[TEXCOORD0] = dim_x * dim_y * channels[TEXCOORD0]* typeSizes[TEXCOORD0];

	offsets		[INDEX]		= 0;
	offsets		[VERTEX]	= 0;
	offsets		[NORMAL]	= offsets [VERTEX] + sizes [VERTEX];
	offsets		[TEXCOORD0] = offsets [NORMAL] + sizes [NORMAL];
	
	int x,y;
	vboCount = 0;
	float tex_cnt_x = 100.f;
	float tex_cnt_y = 100.f;

	sz_x = size_x;
	sz_y = size_y;
	float sx2 = sz_x/2.f;
	float sy2 = sz_y/2.f;
	for (x=0; x<dim_x; x++){
		for (y=0; y<dim_y; y++){

			vertices[(x*dim_y + y)*ch + 0] = x*step_x - sx2;//x
			vertices[(x*dim_y + y)*ch + 1] = getHeightAt(x,y);//height
			vertices[(x*dim_y + y)*ch + 2] = y*step_y - sy2;//y
			
			// normals
			v3 normal;
			
			normal.x = getHeightAt(x-1,y) - getHeightAt(x+1,y);
			normal.y = step_x;
			normal.z = getHeightAt(x,y-1) - getHeightAt(x,y+1);
			normal.normalize();

			if (normal.y<0){
				normal = -normal;
			}
			
			normals[(x*dim_y + y)*ch + 0] = normal.x;
			normals[(x*dim_y + y)*ch + 1] = normal.y;
			normals[(x*dim_y + y)*ch + 2] = normal.z;

			// texCoords
			texCoords[(x*dim_y + y)*2 + 0] = x * tex_cnt_x / dim_x;
			texCoords[(x*dim_y + y)*2 + 1] = y * tex_cnt_y / dim_y;

			vboCount += 3;
		}
	}
	int eli = 0;
	bool reverse = false;
	for (x=0; x<dim_x-1; x++){
		if (!reverse){
			for (y=0; y<dim_y; y++){
				elements[eli] = (x+1)*dim_y + y;
				eli++;
				elements[eli] = (x)*dim_y + y;
				eli++;
			}
		} else {
			for (y=dim_y-1; y>=0; y--){
				elements[eli] = (x)*dim_y + y;
				eli++;
				elements[eli] = (x+1)*dim_y + y;
				eli++;
			}
		}
		reverse = !reverse;
	}
	eboCount = eli;

	// total vbo buffer size
	int vboSize=0;
	for (int b = 0; b < VBO_ATR_COUNT; b++){
		if (b!=INDEX){
			vboSize+=sizes[b];
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizes[INDEX], elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
		// alloc space
		glBufferData(GL_ARRAY_BUFFER, vboSize, 0, GL_STATIC_DRAW); 
		// fill vertices
		glBufferSubData(GL_ARRAY_BUFFER, offsets[VERTEX], sizes[VERTEX], vertices); 
		// fill normals
		glBufferSubData(GL_ARRAY_BUFFER, offsets[NORMAL], sizes[NORMAL], normals);
		// fill texcoords
		glBufferSubData(GL_ARRAY_BUFFER, offsets[TEXCOORD0], sizes[TEXCOORD0], texCoords);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::update(double time)
{
	
}
	 
void Terrain::translate(v3 &movVector)
{

}
	 
void Terrain::rotate(v3 &axis, float angleRad)
{
	
}
	 
void Terrain::scale(v3 &scaleVector)
{

}
	 

void Terrain::loadTextures(string filename, int count){
	char def_filename [100];
	char sh_name [100];
	int texId;
	for (int i=0; i<count; i++){
		sprintf(def_filename, filename.c_str(), i+1);
		sprintf(sh_name, "terrain_tex_%02i", i+1);
		texId = textureManager->loadTexture( string(def_filename) , string(sh_name), true, false);
		Texture * tex = textureManager->getTexture(texId);
		tex->textureUnit = GL_TEXTURE0 + i;
		tex->textureUnitNumber = i;
		shader->linkTexture(textureManager->getTexture(texId));
		textureIds.push_back(texId);
	}

}

void Terrain::loadHeightMap(string filename, int res_x, int res_y)
{
	PNG inputFile;
	inputFile.load(filename);
	dim_x = inputFile.width;
	dim_y = inputFile.height;
	unsigned char * data = inputFile.getData();

	int size = dim_x*dim_y;
	float * originalHeightMap = new float[size];
	int i,x,y;

	for (i=0; i<size; i++){
		originalHeightMap[i] = 256*data[2*i]+data[2*i + 1];
	}
	// recalculate to the given resolution (use linear interpolation...)
	size = res_x*res_y;
	heightMap = new float[size];
	float step_x = float(dim_x)/float(res_x);
	float step_y = float(dim_y)/float(res_y);
	
	for(x=0; x<res_x; x++)
	{
		for(y=0; y<res_y; y++)
		{
			heightMap[x*res_y + y] = getHeightAt(originalHeightMap, dim_x, dim_y, float(x*step_x), float(y*step_y));
		}
	}
	SAFE_DELETE_ARRAY_PTR(originalHeightMap);

}

float Terrain::getHeightAt(float *map, int res_x, int res_y, float x, float y)
{
	// interpolate height
	int ix,iy;
	ix = (int) x;
	iy = (int) y;
	
	
	float tx = x-ix;
	float ty = y-iy;
	float h3 = (1-ty)*getHeightAt(map, res_x, res_y, ix,iy) + ty*getHeightAt(map, res_x, res_y, ix, iy+1);
	float h4 = (1-ty)*getHeightAt(map, res_x, res_y, ix+1,iy) + ty*getHeightAt(map, res_x, res_y, ix+1, iy+1); 
	return (1-tx)*h3 + (tx)*h4;
}

float Terrain::getHeightAt(float *map, int res_x, int res_y, int x, int y)
{
	int dx = res_x;
	int dy = res_y;
	x = min(max(x,0),dx-1);
	y = min(max(y,0),dy-1);
	float hCorrection = map[dx/2*dy + dy/2];
	return HEIGHTMAP_SCALE*(map[x*dy + y]-hCorrection)+HEIGHTMAP_INITHEIGHT;	

}

float Terrain::getHeightAt(float x, float y)
{
	//x+=5.f;
	//y+=5.f;
	// recalc in raster position from space coords
	x *= red_x;
	y *= red_y;
	
	// interpolate height
	int ix,iy;
	ix = (int) x;
	iy = (int) y;
	
	//return getHeightAt(ix,iy);	
	
	float tx = x-ix;
	float ty = y-iy;
	float h3 = (1-ty)*getHeightAt(ix,iy)   + ty*getHeightAt(ix, iy+1);
	float h4 = (1-ty)*getHeightAt(ix+1,iy) + ty*getHeightAt(ix+1, iy+1); 
	return (1-tx)*h3 + (tx)*h4;
}

float Terrain::getHeightAt(int x, int y)
{
	int dx = dim_x;
	int dy = dim_y;
	x = min(max(x,0),dx-1);
	y = min(max(y,0),dy-1);
	return heightMap[x*dy + y];
	/*
	float hCorrection = heightMap[dx/2*dy + dy/2];
	return HEIGHTMAP_SCALE*(heightMap[x*dy + y]-hCorrection)+HEIGHTMAP_INITHEIGHT;	
	*/
}
