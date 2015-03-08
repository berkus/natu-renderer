//-----------------------------------------------------------------------------
//  [PGR2] 3D model class
//  14/02/2011
//-----------------------------------------------------------------------------
#include "pgr2model.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string>

// Data file structure ------------------------------------------------->

/*
   PGR2 file structure
   +-------------------------------+
   | PGR2Model_Header              |   -> main file header
   +-------------------------------+
   | PGR2Model_TextureHeader       |   -> texture 0 descriptor
   +-------------------------------+
   | PGR2Model_TextureHeader       |   -> texture 1 descriptor
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | PGR2Model_TextureHeader       |   -> texture N descriptor
   +-------------------------------+      (N = PGR2Model_Header.num_textures)
   | MaterialData                  |   -> material 0 data
   +-------------------------------+
   | MaterialData                  |   -> material 1 data
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | MaterialData                  |   -> material N data
   +-------------------------------+      (N = PGR2Model_Header.num_materials)
   | PGR2Model_MeshHeader          |   -> mesh 0 data
   +-------------------------------+
   | FaceData                      |   -> mesh 0 first face
   +-------------------------------+
   | FaceData                      |   -> mesh 0 second face
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | FaceData N                    |   -> mesh 0 N-th face
   +-------------------------------+      (N = PGR2Model_MeshHeader.num_faces)
   | PGR2Model_MeshHeader          |   -> mesh 1 data
   +-------------------------------+
   | FaceData                      |   -> mesh 1 first face
   +-------------------------------+
   | FaceData                      |   -> mesh 1 second face
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | FaceData                      |   -> mesh N N-th face
   +-------------------------------+      
   | ...                           |
   +-------------------------------+
   | PGR2Model_MeshHeader          |   -> mesh N data
   +-------------------------------+      (N = PGR2Model_Header.num_meshes)
   | FaceData                      |   -> mesh N first face
   +-------------------------------+
   | FaceData                      |   -> mesh N second face
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | FaceData                      |   -> mesh N N-th face
   +-------------------------------+
   | GLfloat[0..m_NumVertices]     |   -> vertices data (m_Vertex)
   +-------------------------------+
   | GLfloat[0..m_NumTexCoords]    |   -> texture coordinates (m_TexCoords)
   +-------------------------------+
   | GLfloat[0..m_NumNormals]      |   -> normal vectors (m_Normals)
   +-------------------------------+
   | GLfloat[0..m_NumNormals]      |   -> binormal vectors (m_Binormals)
   +-------------------------------+
   | GLfloat[0..m_NumNormals]      |   -> tangent vectors (m_Tangents)
   +-------------------------------+
*/

// Main file header
struct PGR2Model_Header
{
   unsigned short num_textures;
   unsigned short num_materials;
   unsigned short num_meshes;
   unsigned long  num_vertices;
   unsigned long  num_tex_coords;
   unsigned long  num_normals;
   unsigned long  num_binormals;
};

// Texture data header
struct PGR2Model_TextureHeader
{
   char file_name[TEXTURE_NAME_SIZE];
};
struct PGR2Texture_Header
{
   GLsizei        width;
   GLsizei        height;
   GLenum         type;
   unsigned int   size;
};


// Mesh data header
struct PGR2Model_MeshHeader
{
   unsigned short material_index;
   unsigned long  num_faces;
};

// Data file structure ------------------------------------------------->


//---------------------------------------------------------------------------
//	Description: Gets file path from full file name
//
//---------------------------------------------------------------------------
inline std::string getFilePath(const std::string& file_name)
{
   std::string file_path         = std::string(file_name);
   std::string::size_type offset = file_path.find_last_of('\\');
   if (offset != std::string::npos)
   {
      file_path = file_path.substr(0, ++offset);
   }
   else
   {
      offset = file_path.find_last_of('/');
      if (offset != std::string::npos)
      {
         file_path = file_path.substr(0, ++offset);
      }
   }

   return file_path;
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
PGR2Model::PGR2Model() : 
   m_NumTextures(0), m_NumMaterials(0), m_NumMeshes(0), 
   m_NumVertices(0), m_NumTexCoords(0), m_NumNormals(0), m_NumBinormals(0),
   m_Textures(NULL), m_Materials(NULL), m_Meshes(NULL),
   m_Vertices(NULL), m_TexCoords(NULL), 
   m_Normals(NULL), m_Binormals(NULL), m_Tangents(NULL)
{
}

 PGR2Model::PGR2Model(TextureManager *texManager)
 {
	 textureManager = texManager;
 }

//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
PGR2Model::~PGR2Model()
{
	// delete ebos
	for (int i=0; i<m_NumMeshes; i++){
		glDeleteBuffers(1, & meshes[i]->eboId);
	}
	// delete vbo
	glDeleteBuffers(1, & vboId);

   if (m_Textures)   delete [] m_Textures;
   if (m_Materials)  delete [] m_Materials;
   if (m_Meshes)
   {
      for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
      {
         delete [] m_Meshes[iMesh].faces;
      }
      delete [] m_Meshes;
   }
   if (m_Vertices)   delete [] m_Vertices;
   if (m_TexCoords)  delete [] m_TexCoords;
   if (m_Normals)    delete [] m_Normals;
   if (m_Binormals)  delete [] m_Binormals;
   if (m_Tangents)   delete [] m_Tangents;
   





}

void PGR2Model::init()
{
	v3 x(1.0,0.0,0.0);
	v3 y(0.0,1.0,0.0);
	v3 z(0.0,0.0,1.0);
	// init min & max corners
	minCorner = v3(FLT_MAX);
	maxCorner = v3(-FLT_MAX);
	
	// assemble VBO & EBO for transparent & non transparent meshes
	

	// non transparent
	vboId		= 0; 
	vboCount	= 0;
	int iMesh;
	

	vector<int> v_indices;
	vector<VertexData> v_vertices;
	for (iMesh = 0; iMesh < m_NumMeshes; iMesh++)
	{
		v_indices.clear();
		const MeshData& meshData = m_Meshes[iMesh];
		Mesh * mesh = meshes[iMesh];

		mesh->hasHeightMap		= m_Materials[meshData.material_index].height_tex_index>=0;
		mesh->hasBumpMap		= m_Materials[meshData.material_index].bump_tex_index>=0;
		mesh->hasSpecularMap	= m_Materials[meshData.material_index].specular_tex_index>=0;
		mesh->material			= & m_Materials[meshData.material_index];
		if (level==0){
			mesh->enabledLoc	= mesh->shader->getLocation("enabled");
			mesh->scaleBiasLoc	= mesh->shader->getLocation("scaleBias");
		}
		//mesh->shader = NULL;
		/*
		if (mesh->hasHeightMap && mesh->hasBumpMap && mesh->hasSpecularMap){
			mesh->shader = parallaxShader;
		} else if (mesh->hasBumpMap && mesh->hasSpecularMap){
			mesh->shader = bumpShader;
		} else if (mesh->hasSpecularMap){
			mesh->shader = specularShader;
		} else {
			mesh->shader = NULL;
		}
		*/

		for (unsigned int iFace = 0; iFace < meshData.num_faces; iFace++)
		{
			const FaceData& face = meshData.faces[iFace];
			for (int iVertex = 0; iVertex < 3; iVertex++)
			{
				VertexData vertex;
				vertex.position		= v3(	m_Vertices[3*face.ivertex[iVertex] + 0],
											m_Vertices[3*face.ivertex[iVertex] + 1],
											m_Vertices[3*face.ivertex[iVertex] + 2]);
				minCorner.min(vertex.position);
				maxCorner.max(vertex.position);
				vertex.normal =  v3(face.normal[0], face.normal[1], face.normal[2]) ;
				
				/*vertex.normal		= v3(	m_Normals[3*face.inormal[iVertex] + 0],
											m_Normals[3*face.inormal[iVertex] + 1],
											m_Normals[3*face.inormal[iVertex] + 2]);
				*/							
				// calc binormal from normal
				float angle = vertex.normal.angleTo(x);
				v3 axis = vertex.normal.cross(x);
				vertex.binormal = y.getRotated(-angle, axis);


				/*
				vertex.binormal		= v3(	m_Binormals[3*face.inormal[iVertex] + 0],
											m_Binormals[3*face.inormal[iVertex] + 1],
											m_Binormals[3*face.inormal[iVertex] + 2]);
				*/
				vertex.texcoord		= v2(	m_TexCoords[2*face.itexture[iVertex] + 0],
											m_TexCoords[2*face.itexture[iVertex] + 1]);
				vertex.facenormal	= v3(face.normal[0], face.normal[1], face.normal[2]) ;
				v_indices.push_back(v_vertices.size());
				v_vertices.push_back(vertex);
				 
			} // for each vertex on face
		} // for each face in mesh
		int maximum = INT_MIN;
		mesh->eboCount = v_indices.size();
		GLuint *elements = new GLuint[mesh->eboCount];
		for (int i=0; i<mesh->eboCount; i++){
			elements[i] = (GLuint) v_indices[i];
			if (maximum<v_indices[i]){
				maximum = v_indices[i];
			}
		}
		glGenBuffers(1, &(mesh->eboId));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->eboId);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->eboCount*sizeof(GL_UNSIGNED_INT), elements, GL_STATIC_DRAW);	
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		delete [] elements;
		elements = NULL;

	} // for each mesh

	int size, i;
	// create arrays for VBO
	size = v_vertices.size();
	GLfloat * p_Vertices	= new GLfloat[3*size];
	GLfloat * p_Normals		= new GLfloat[3*size];
	GLfloat * p_Binormals	= new GLfloat[3*size];
	GLfloat * p_TexCoords	= new GLfloat[2*size];
	GLfloat * p_FaceNormals	= new GLfloat[3*size];

	offsets[VERTEX] = 0;
	sizes[VERTEX]   = size * 3 * sizeof(GLfloat);
	int vboSize = sizes[VERTEX];
	
	offsets[NORMAL] = vboSize;
	sizes[NORMAL]	= size * 3 * sizeof(GLfloat);
	vboSize+= sizes[NORMAL];
	
	offsets[BINORMAL] = vboSize;
	sizes[BINORMAL]	= size * 3 * sizeof(GLfloat);
	vboSize+= sizes[BINORMAL];

	/*
	offsets[TANGENT] = vboSize;
	sizes[TANGENT]	= m_NumVertices * 4 * sizeof(GLfloat);
	vboSize+= sizes[TANGENT];
	*/
	
	offsets[TEXCOORD0] = vboSize;
	sizes[TEXCOORD0]	= size * 2 * sizeof(GLfloat);
	vboSize+= sizes[TEXCOORD0];
	
	offsets[FACENORMAL] = vboSize;
	sizes[FACENORMAL]	= size * 3 * sizeof(GLfloat);
	vboSize+= sizes[FACENORMAL];
	
	for (i=0; i<size; i++){
		p_Vertices	 [3*i + 0]	= v_vertices[i].position.data[0];
		p_Vertices	 [3*i + 1]	= v_vertices[i].position.data[1];
		p_Vertices	 [3*i + 2]	= v_vertices[i].position.data[2];

		p_Normals	 [3*i + 0]	= v_vertices[i].normal.data[0];
		p_Normals	 [3*i + 1]	= v_vertices[i].normal.data[1];
		p_Normals	 [3*i + 2]	= v_vertices[i].normal.data[2];

		p_Binormals	 [3*i + 0]	= v_vertices[i].binormal.data[0];
		p_Binormals	 [3*i + 1]	= v_vertices[i].binormal.data[1];
		p_Binormals	 [3*i + 2]	= v_vertices[i].binormal.data[2];

		p_TexCoords	 [2*i + 0]	= v_vertices[i].texcoord.data[0];
		p_TexCoords	 [2*i + 1]	= v_vertices[i].texcoord.data[1];

		p_FaceNormals[3*i + 0]	= v_vertices[i].facenormal.data[0];
		p_FaceNormals[3*i + 1]	= v_vertices[i].facenormal.data[1];
		p_FaceNormals[3*i + 2]	= v_vertices[i].facenormal.data[2];
	}


	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glBufferData(GL_ARRAY_BUFFER, vboSize, 0, GL_STATIC_DRAW);
		// vertices
		glBufferSubData(GL_ARRAY_BUFFER, offsets[VERTEX], sizes[VERTEX], p_Vertices);
		// normals
		glBufferSubData(GL_ARRAY_BUFFER, offsets[NORMAL], sizes[NORMAL], p_Normals);
		//glBufferSubData(GL_ARRAY_BUFFER, offsets[NORMAL], sizes[NORMAL], p_Normals);
		// binormals
		glBufferSubData(GL_ARRAY_BUFFER, offsets[BINORMAL], sizes[BINORMAL], p_Binormals);
		// tangents
		// glBufferSubData(GL_ARRAY_BUFFER, offsets[TANGENT], sizes[TANGENT], p_Tangents);
		// texcoords
		glBufferSubData(GL_ARRAY_BUFFER, offsets[TEXCOORD0], sizes[TEXCOORD0], p_TexCoords);
		
		// facenormals
		glBufferSubData(GL_ARRAY_BUFFER, offsets[FACENORMAL], sizes[FACENORMAL], p_FaceNormals);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	// delete arrays for VBO
	delete [] p_Binormals;
	delete [] p_Normals;
	delete [] p_FaceNormals;
	delete [] p_Vertices;
	delete [] p_TexCoords;
	bbox = new BBox(minCorner, maxCorner, v3(0.0, 0.0, 0.0));
}

void PGR2Model::drawForLOD()
{
	glDisable(GL_CULL_FACE);
	bbox->drawMode = GL_FILL;
	bbox->draw();
	bbox->drawMode = GL_LINE;
	glEnable(GL_CULL_FACE);
}

void PGR2Model::draw()
{
	renderVBO(true);
}
	 
void PGR2Model::update(double time)
{
}

int PGR2Model::getNumTextures(){
	return m_NumTextures;
}

void PGR2Model::renderVBO(bool transparent_meshes)
{
	glPushAttrib(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	// render meshes
	int i;
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	for (int iMesh=0; iMesh < m_NumMeshes; iMesh++ ){
		Mesh * mesh = meshes[iMesh];
		if (mesh->isTransparent){
			glEnable(GL_BLEND);
			glEnable(GL_ALPHA_TEST);
			glDisable(GL_LIGHTING);
		}
		glMaterialfv(GL_FRONT_AND_BACK,   GL_AMBIENT, mesh->material->ambient);
        glMaterialfv(GL_FRONT_AND_BACK,   GL_DIFFUSE, mesh->material->diffuse);
        glMaterialfv(GL_FRONT_AND_BACK,  GL_SPECULAR, mesh->material->specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, mesh->material->shininess * 128.0f);

		// bind textures
		
		for (i=0; i<mesh->texCount; i++){
			textureManager->bindTexture(mesh->texIds[i], GL_TEXTURE0+GLuint(i));
		}
		
		// own pipeline
		if (mesh->shader!=NULL){
			mesh->shader->use(true);
			mesh->shader->setTexture(mesh->diffuse_tex_loc, mesh->diffuse_tex_i);
			if (level==0){
				mesh->shader->setUniform1i(mesh->enabledLoc, g_ParallaxMappingEnabled?1:0);
				mesh->shader->setTexture(mesh->bump_tex_loc, mesh->bump_tex_i);
				mesh->shader->setUniform2f(mesh->scaleBiasLoc, g_ParallaxScale, g_ParallaxBias);
			}
		}
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->eboId);
		 
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			
			
			// draw VBOs...
			// positions
			glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET( offsets[VERTEX]));
			// normals
			glNormalPointer(GL_FLOAT,	 0, BUFFER_OFFSET( offsets[NORMAL] ) );
			

			// own pipeline
			if (mesh->shader!=NULL){
				glEnableVertexAttribArray(mesh->binormalLoc	);
				glEnableVertexAttribArray(mesh->facenormalLoc);
				// binormals
				glVertexAttribPointer(mesh->binormalLoc	, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[BINORMAL]));
				// facenormals
				glVertexAttribPointer(mesh->facenormalLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsets[FACENORMAL]));
			}
			
			
			// for each texture ACTIVATE & set coordinates
			
			for (i=0; i<mesh->texCount; i++){
				textureManager->activateTexture(mesh->texIds[i]);
				glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(offsets[TEXCOORD0]));
			}	
			
			glDrawElements(GL_TRIANGLES, mesh->eboCount, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
			
			// for each texture DEACTIVATE
			
			for (i=0; i<mesh->texCount; i++){
				textureManager->deactivateTexture(mesh->texIds[i]);
			}
			
			if (mesh->shader!=NULL){
				glDisableVertexAttribArray(mesh->binormalLoc	);
				glDisableVertexAttribArray(mesh->facenormalLoc	);
			}
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_VERTEX_ARRAY);

		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		if (mesh->isTransparent){
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
			glEnable(GL_LIGHTING);
		}
		
		// fixed pipeline
		if (mesh->shader!=NULL){
			mesh->shader->use(false);
		}
		// unbind textures
		for (i=0; i<mesh->texCount; i++){
			textureManager->unbindTexture(mesh->texIds[i]);
		}
		

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glPopAttrib();
	

}
//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
void PGR2Model::render(bool transparent_meshes)
{
   // 1st pass - render meshes with no visibility information (mesh.material.alpha_tex_id == 0)
   for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
   {
      // DA - render transparent meshes with glDepthMask(0)
      unsigned short num_textures_used = 0;

      const MeshData& mesh = m_Meshes[iMesh];
      if (mesh.material_index >= 0)
      {
         const MaterialData& material = m_Materials[mesh.material_index];

         // Render non-transparent meshes only -> skip meshes with alpha texture
         if (material.alpha_tex_index >= 0) continue;

         glMaterialfv(GL_FRONT_AND_BACK,   GL_AMBIENT, material.ambient);
         glMaterialfv(GL_FRONT_AND_BACK,   GL_DIFFUSE, material.diffuse);
         glMaterialfv(GL_FRONT_AND_BACK,  GL_SPECULAR, material.specular);
         glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.shininess * 128.0f);

         // Use diffuse texture if exists
         if (material.diffuse_tex_index >= 0)
         {
            glActiveTexture(GL_TEXTURE0 + num_textures_used);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_Textures[material.diffuse_tex_index].id);
            num_textures_used++;
         }
         // Use specular texture if exists
         if (material.specular_tex_index >= 0)
         {
//          glActiveTexture(GL_TEXTURE0 + num_textures_used);
//          glEnable(GL_TEXTURE_2D);
//          glBindTexture(GL_TEXTURE_2D, m_Textures[material.specular_tex_index].id);
//          num_textures_used++;
         }
         // Use bump texture if exists
         if (material.bump_tex_index >= 0) 
         {
//          glActiveTexture(GL_TEXTURE0 + num_textures_used);
//          glEnable(GL_TEXTURE_2D);
//          glBindTexture(GL_TEXTURE_2D, m_Textures[material.bump_tex_index].id);
//          num_textures_used++;
         }
         // Use height texture if exists
         if (material.height_tex_index >= 0) 
         {
//          glActiveTexture(GL_TEXTURE0 + num_textures_used);
//          glEnable(GL_TEXTURE_2D);
//          glBindTexture(GL_TEXTURE_2D, m_Textures[material.height_tex_index].id);
//          num_textures_used++;
         }
      }

//    glActiveTexture(GL_TEXTURE0);
      glBegin(GL_TRIANGLES);
      for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++)
      {
         const FaceData& face = mesh.faces[iFace];

         for (int iVertex = 0; iVertex < 3; iVertex++)
         {
            for (int iTex = 0; iTex < num_textures_used; iTex++)
            {
               glMultiTexCoord2fv(GL_TEXTURE0 + iTex, &m_TexCoords[2*face.itexture[iVertex]]);
            }
            glNormal3fv(&m_Normals[3*face.inormal[iVertex]]);
            glVertex3fv(&m_Vertices[3*face.ivertex[iVertex]]);
         }
      }
      glEnd();

      // Disable texturing
      for (int iTex = num_textures_used-1; iTex >= 0; iTex--)
      {
         glActiveTexture(GL_TEXTURE0 + iTex);
         glDisable(GL_TEXTURE_2D);
      }
   }

   if (transparent_meshes)
   {
      // 2nd pass - render meshes with visibility information (mesh.material.alpha_tex_index > 0)
      glEnable(GL_BLEND);
      glEnable(GL_ALPHA_TEST);
      glDisable(GL_LIGHTING);

      for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
      {
         int num_textures_used = 0;
           
         const MeshData& mesh = m_Meshes[iMesh];
         if (mesh.material_index >= 0)
         {
            const MaterialData& material = m_Materials[mesh.material_index];

            // Render meshes with alpha texture only
            if (material.alpha_tex_index < 0) continue;

            glMaterialfv(GL_FRONT_AND_BACK,   GL_AMBIENT, material.ambient);
            glMaterialfv(GL_FRONT_AND_BACK,   GL_DIFFUSE, material.diffuse);
            glMaterialfv(GL_FRONT_AND_BACK,  GL_SPECULAR, material.specular);
            glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, material.shininess * 128.0f);

            // Use diffuse texture if exists
            if (material.diffuse_tex_index >= 0)
            {
               glActiveTexture(GL_TEXTURE0 + num_textures_used);
               glEnable(GL_TEXTURE_2D);
               glBindTexture(GL_TEXTURE_2D, m_Textures[material.diffuse_tex_index].id);
               num_textures_used++;
            }
            // Use specular texture if exists
            if (material.specular_tex_index >= 0)
            {
//             glActiveTexture(GL_TEXTURE0 + num_textures_used);
//             glEnable(GL_TEXTURE_2D);
//             glBindTexture(GL_TEXTURE_2D, m_Textures[material.specular_tex_index].id);
//             num_textures_used++;
            }
            // Use bump texture if exists
            if (material.bump_tex_index >= 0) 
            {
//             glActiveTexture(GL_TEXTURE0 + num_textures_used);
//             glEnable(GL_TEXTURE_2D);
//             glBindTexture(GL_TEXTURE_2D, m_Textures[material.bump_tex_index].id);
//             num_textures_used++;
           }
            // Use height texture if exists
            if (material.height_tex_index >= 0) 
            {
//             glActiveTexture(GL_TEXTURE0 + num_textures_used);
//             glEnable(GL_TEXTURE_2D);
//             glBindTexture(GL_TEXTURE_2D, m_Textures[material.height_tex_index].id);
//             num_textures_used++;
            }
            // Setup alpha texture
            glActiveTexture(GL_TEXTURE0 + num_textures_used);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, m_Textures[material.alpha_tex_index].id);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            num_textures_used++;
         }

//       glActiveTexture(GL_TEXTURE0);
         glBegin(GL_TRIANGLES);
         for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++)
         {
            const FaceData& face = mesh.faces[iFace];

            for (int iVertex = 0; iVertex < 3; iVertex++)
            {
               for (int iTex = 0; iTex < num_textures_used; iTex++)
               {
                  glMultiTexCoord2fv(GL_TEXTURE0 + iTex, &m_TexCoords[2*face.itexture[iVertex]]);
               }
               glNormal3fv(&m_Normals[3*face.inormal[iVertex]]);
               glVertex3fv(&m_Vertices[3*face.ivertex[iVertex]]);
            }
         }
         glEnd();

         // Disable texturing
         for (int iTex = num_textures_used-1; iTex >= 0; iTex--)
         {
            glActiveTexture(GL_TEXTURE0 + iTex);
            glDisable(GL_TEXTURE_2D);
         }
      }

      glDisable(GL_BLEND);
      glDisable(GL_ALPHA_TEST);
      glEnable(GL_LIGHTING);
   }
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
void PGR2Model::renderFaceNormals(float scale)
{
   const float normals_scale = 2*scale;

   glDisable(GL_LIGHTING); // DA remove lighting at all from this class
   glBegin(GL_LINES);
   glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
   for (unsigned int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
   {
      const MeshData& mesh = m_Meshes[iMesh];
      for (unsigned int iFace = 0; iFace < mesh.num_faces; iFace++)
      {
         const FaceData& face = mesh.faces[iFace];
         const unsigned int v0i = 3*face.ivertex[0];
         const unsigned int v1i = 3*face.ivertex[1];
         const unsigned int v2i = 3*face.ivertex[2];

         const GLfloat center[3] = 
         {
            (m_Vertices[v0i + 0] + m_Vertices[v1i + 0] + m_Vertices[v2i + 0]) * 0.33f,
            (m_Vertices[v0i + 1] + m_Vertices[v1i + 1] + m_Vertices[v2i + 1]) * 0.33f,
            (m_Vertices[v0i + 2] + m_Vertices[v1i + 2] + m_Vertices[v2i + 2]) * 0.33f
         };
         const GLfloat end[3]    = 
         {
            center[0] + face.normal[0]*normals_scale,
            center[1] + face.normal[1]*normals_scale,
            center[2] + face.normal[2]*normals_scale
         };
         glVertex3fv(center);
         glVertex3fv(end);
      }
   }
   glEnd();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   glEnable(GL_LIGHTING);
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
void PGR2Model::renderVertexNormals(float scale)
{
   if ((m_Binormals != NULL) && (m_NumBinormals > 0))
   {
      const float normals_scale = 2*scale;

      glDisable(GL_LIGHTING); // DA remove lighting at all from this class
      glBegin(GL_LINES);
         for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
         {
            const MeshData& mesh = m_Meshes[iMesh];
            for (int iFace = 0; iFace < mesh.num_faces; iFace++)
            {
               const FaceData& face = mesh.faces[iFace];
               for (int iVertex = 0; iVertex < 3; iVertex++)
               {
                  const GLfloat* pVertex     = m_Vertices + 3*face.ivertex[iVertex];
                  const GLfloat* pNormal     = m_Normals + 3*face.inormal[iVertex];
                  const GLfloat* pBinormal   = m_Binormals + 3*face.inormal[iVertex];
                  const GLfloat* pTangent    = m_Tangents + 4*face.inormal[iVertex];

                  const GLfloat end_normal[3]   = {  pVertex[0] + pNormal[0]*scale,   pVertex[1] + pNormal[1]*scale,    pVertex[2] + pNormal[2]*scale};
                  const GLfloat end_binormal[3] = {pVertex[0] + pBinormal[0]*scale, pVertex[1] + pBinormal[1]*scale,  pVertex[2] + pBinormal[2]*scale};
                  const GLfloat end_tangent[3]  = { pVertex[0] + pTangent[0]*scale,  pVertex[1] + pTangent[1]*scale,   pVertex[2] + pTangent[2]*scale};

                  // Normal
                  glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
                  glVertex3fv(pVertex);
                  glVertex3fv(end_normal);
                  // Binormal
                  glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
                  glVertex3fv(pVertex);
                  glVertex3fv(end_binormal);
                  // Tangent
                  glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                  glVertex3fv(pVertex);
                  glVertex3fv(end_tangent);
               }
            }
         }
      glEnd();

      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glEnable(GL_LIGHTING);
   }
}


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
PGR2Model* PGR2Model::loadFromFile(const char* file_name, TextureManager *texManager, ShaderManager *shManager, int level)
{
   bool load_failed = true;

   FILE* fin = fopen(file_name, "rb");
   if (!fin)
   {
      return NULL;
   }

   // Get file path
   std::string file_path = getFilePath(file_name);

   PGR2Model* pNewModel = new PGR2Model(texManager);
   pNewModel->level = level;
   if (!pNewModel) goto end_of_load;

// Read model header ---------------------------------------------->
   PGR2Model_Header header = {0};
   if (fread(&header, sizeof(PGR2Model_Header), 1, fin) != 1) goto end_of_load;

   if ((header.num_materials == 0) || (header.num_meshes == 0) ||
       (header.num_normals == 0) || (header.num_vertices == 0))
   {
      goto end_of_load;
   }

// Read textures -------------------------------------------------->
   pNewModel->m_NumTextures = header.num_textures;
   if (pNewModel->m_NumTextures > 0)
   {
      // Allocate array for texture objects id
      pNewModel->m_Textures = new TextureData[header.num_textures];
      memset(pNewModel->m_Textures, 0, header.num_textures*sizeof(GLubyte));

      // Create texture objects
      for (int iTexture = 0; iTexture < header.num_textures; iTexture++)
      {
         // Read texture header from file
         PGR2Model_TextureHeader tex_header = {0};
         if (fread(&tex_header, sizeof(PGR2Model_TextureHeader), 1, fin) != 1) 
            goto end_of_load;

      // Read texture data from file ---------------------------->
         FILE* file_texture = fopen((file_path + tex_header.file_name).c_str(), "rb");
         if (file_texture == NULL)
            goto end_of_load;

         PGR2Texture_Header tex_data_header = {0};
         if (fread(&tex_data_header, sizeof(PGR2Texture_Header), 1, file_texture) != 1) 
            goto end_of_load;

         assert(tex_data_header.size != 0);
         GLubyte* pixels = new GLubyte[tex_data_header.size];
         if (fread(pixels, sizeof(GLubyte), tex_data_header.size, file_texture) != tex_data_header.size)
            goto end_of_load;

         // save texture file name
   //      strcpy(pNewModel->m_Textures[iTexture].file_name, tex_header.file_name, TEXTURE_NAME_SIZE);

         // Create texture object
         GLenum error = glGetError();
         glGenTextures(1, &pNewModel->m_Textures[iTexture].id);
         glBindTexture(GL_TEXTURE_2D, pNewModel->m_Textures[iTexture].id);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
         gluBuild2DMipmaps(GL_TEXTURE_2D, tex_data_header.type, tex_data_header.width, 
                           tex_data_header.height, tex_data_header.type, GL_UNSIGNED_BYTE, pixels);
         delete [] pixels;
         fclose(file_texture);

         if ((error = glGetError()) != GL_NO_ERROR) 
            goto end_of_load;
      // Read texture data from file ---------------------------->
      }
   }

// Read materials ------------------------------------------------->
   pNewModel->m_NumMaterials = header.num_materials;

   // Allocate array for materials
   pNewModel->m_Materials = new MaterialData[header.num_materials];
   memset(pNewModel->m_Materials, 0, header.num_materials*sizeof(MaterialData));

   // Load materials from file
   for (int iMaterial = 0; iMaterial < header.num_materials; iMaterial++)
   {
      if (fread(&pNewModel->m_Materials[iMaterial], sizeof(MaterialData), 1, fin) != 1)
         goto end_of_load;
   }

// Read meshes ---------------------------------------------------->
   pNewModel->m_NumMeshes = header.num_meshes;

   // Allocate array for meshes
   pNewModel->m_Meshes = new MeshData[header.num_meshes];
   memset(pNewModel->m_Meshes, 0, header.num_meshes*sizeof(MeshData));
// ===========================================================================
// own mesh handling
   pNewModel->meshes = new Mesh*[header.num_meshes];

// ===========================================================================


   for (int iMesh = 0; iMesh < header.num_meshes; iMesh++)
   {
      // Read mesh header from file
      PGR2Model_MeshHeader mes_header = {0};
      if (fread(&mes_header, sizeof(PGR2Model_MeshHeader), 1, fin) != 1) 
         goto end_of_load;

      pNewModel->m_Meshes[iMesh].material_index = mes_header.material_index;
// ===========================================================================
		pNewModel->meshes[iMesh] = new Mesh;
		MaterialData* materialData = & pNewModel->m_Materials[mes_header.material_index];
		Mesh * mesh = pNewModel->meshes[iMesh];
		mesh->isTransparent = false;
		mesh->texCount = 0;
		mesh->shader = shManager->getPhongShader();
		if (level==0){
			mesh->shader = shManager->getParallaxShader();
		}
		mesh->binormalLoc	= mesh->shader->getAttributeLocation("a_binormal");
		mesh->facenormalLoc = mesh->shader->getAttributeLocation("a_facenormal");

		vector<int> tex_ids;
		if (materialData->diffuse_tex_index		>= 0 ){
			Texture * tex = new Texture();
			tex->id = pNewModel->m_Textures[materialData->diffuse_tex_index].id;
			tex->inShaderName = "diffuse_texture";
			mesh->diffuse_tex_i = tex_ids.size();
			tex_ids.push_back(pNewModel->textureManager->addTexture(tex));
			mesh->diffuse_tex_loc = mesh->shader->getLocation(tex->inShaderName);

		}
		if (materialData->specular_tex_index	>= 0 ){
			Texture * tex = new Texture();
			tex->id = pNewModel->m_Textures[materialData->specular_tex_index].id;
			tex->inShaderName = "specular_texture";
			mesh->specular_tex_i = tex_ids.size();
			tex_ids.push_back(pNewModel->textureManager->addTexture(tex));
			mesh->specular_tex_loc = mesh->shader->getGLLocation(tex->inShaderName);

			g_Specularmaps ++;

		}
		if (materialData->bump_tex_index		>= 0 ){
			Texture * tex = new Texture();
			tex->id = pNewModel->m_Textures[materialData->bump_tex_index].id;
			tex->inShaderName = "bump_texture";
			mesh->bump_tex_i = tex_ids.size();
			tex_ids.push_back(pNewModel->textureManager->addTexture(tex));
			mesh->bump_tex_loc = mesh->shader->getGLLocation(tex->inShaderName);

			g_Bumpmaps++;

		}
		if (materialData->alpha_tex_index		>= 0 ){
			mesh->isTransparent = true;
			Texture * tex = new Texture();
			tex->id = pNewModel->m_Textures[materialData->alpha_tex_index].id;
			tex->inShaderName = "alpha_texture";
			tex_ids.push_back(pNewModel->textureManager->addTexture(tex));
			g_Alphamaps++;
		}
		if (materialData->height_tex_index		>= 0 ){
			Texture * tex = new Texture();
			tex->id = pNewModel->m_Textures[materialData->height_tex_index].id;
			tex->inShaderName = "height_texture";
			mesh->height_tex_i = tex_ids.size();
			tex_ids.push_back(pNewModel->textureManager->addTexture(tex));
			mesh->height_tex_loc = mesh->shader->getGLLocation(tex->inShaderName);

			g_Heightmaps++;
		}
		mesh->texCount = tex_ids.size();
		// textures in texmanager... 
		mesh->texIds = new int[mesh->texCount];
		for (int i=0; i<mesh->texCount; i++){
			mesh->texIds[i] = tex_ids[i];
		}
// ===========================================================================
      pNewModel->m_Meshes[iMesh].num_faces      = mes_header.num_faces;
      if (mes_header.num_faces > 0)
      {
         // Read mesh face data from file
         pNewModel->m_Meshes[iMesh].faces = new FaceData[mes_header.num_faces];
         if (fread(pNewModel->m_Meshes[iMesh].faces, sizeof(FaceData),
                   mes_header.num_faces, fin) != mes_header.num_faces)
            goto end_of_load;
      }
      else
      {
         goto end_of_load;
      }
   }


// Read model vertices -------------------------------------------->
   pNewModel->m_NumVertices = header.num_vertices;
   // Allocate array for vertices
   pNewModel->m_Vertices = new GLfloat[3*header.num_vertices];
   if (fread(pNewModel->m_Vertices, 3*sizeof(GLfloat), header.num_vertices, fin) != header.num_vertices)
      goto end_of_load;

// Read model texture coordinates --------------------------------->
   pNewModel->m_NumTexCoords = header.num_tex_coords;
   if (pNewModel->m_NumTexCoords > 0)
   {
      // Allocate array for texture coordinates
      pNewModel->m_TexCoords = new GLfloat[2*header.num_tex_coords];
      if (fread(pNewModel->m_TexCoords, 2*sizeof(GLfloat), header.num_tex_coords, fin) != header.num_tex_coords)
         goto end_of_load;
   }

// Read model normals --------------------------------------------->
   pNewModel->m_NumNormals = header.num_normals;
   // Allocate array for normals
   pNewModel->m_Normals = new GLfloat[3*header.num_normals];
   if (fread(pNewModel->m_Normals, 3*sizeof(GLfloat), header.num_normals, fin) != header.num_normals)
      goto end_of_load;

// Read model binormals ------------------------------------------->
   pNewModel->m_NumBinormals = header.num_binormals;
   if (pNewModel->m_NumBinormals > 0)
   {
      // Allocate array for binormals
      pNewModel->m_Binormals = new GLfloat[3*header.num_binormals];
      if (fread(pNewModel->m_Binormals, 3*sizeof(GLfloat), header.num_normals, fin) != header.num_normals)
         goto end_of_load;

   // Read model tangents -------------------------------------------->
      // Allocate array for tangents
      pNewModel->m_Tangents = new GLfloat[4*header.num_binormals];
      if (fread(pNewModel->m_Tangents, 4*sizeof(GLfloat), header.num_normals, fin) != header.num_normals)
         goto end_of_load;
   }

   // Model was loaded successfully
   load_failed = false;

end_of_load:
   if (load_failed)
   {
      delete pNewModel;
      pNewModel = NULL;
   }
   fclose(fin);

   return pNewModel;
};


//---------------------------------------------------------------------------
//	Description: 
//
//---------------------------------------------------------------------------
bool PGR2Model::saveToFile(const char* file_name)
{
   bool result = false;

   FILE* fout = fopen(file_name, "wb");

   if (!fout)
   {
      return false;
   }

   // Get file path
   std::string file_path = getFilePath(file_name);

// File header start ---------------------------------------------->
   PGR2Model_Header header = 
   {
      m_NumTextures,
      m_NumMaterials,
      m_NumMeshes, 
      m_NumVertices,
      m_NumTexCoords,
      m_NumNormals,
      m_NumBinormals
   };

   if (fwrite(&header, sizeof(PGR2Model_Header), 1, fout) != 1)
   {
      goto end_of_save;
   }
// File header end ------------------------------------------------>

// Texture headers start ------------------------------------------>
   GLenum error = glGetError();
   for (int iTexture = 0; iTexture < m_NumTextures; iTexture++)
   {
      glBindTexture(GL_TEXTURE_2D, m_Textures[iTexture].id);
      assert(glGetError() == GL_NO_ERROR);
      GLint width      = 0;
      GLint height     = 0;
      GLint format     = 0;
      GLint compressed = 0;
      GLint red = 0, green = 0, blue = 0, alpha = 0;
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &format);
      glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
      if (compressed > 0)
      {
         assert(0);  // Not supported now !!!
         goto end_of_save;
      }

      switch(format)
      {
         case GL_ALPHA: 
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
         break;
         case GL_RGBA :
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, &alpha);
         case GL_RGB  :
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE,   &red);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, &green);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE,  &blue);
         break;
         default      :
            goto end_of_save;
         break;
      }

      GLint texel_size = (red + green + blue + alpha) / 8;
      if (((width <= 0) || (height <= 0)) || (texel_size <= 0))
      {
         goto end_of_save;
      }

      const unsigned int data_size = texel_size * width * height;
      GLubyte* texels = new GLubyte[data_size];
      memset(texels, 0, data_size);
      glGetTexImage(GL_TEXTURE_2D, 0, format, GL_UNSIGNED_BYTE, texels);
      assert(glGetError() == GL_NO_ERROR);

   // Save texture to separate file --------------->
      FILE* file_texture = fopen((file_path + m_Textures[iTexture].file_name).c_str(), "wb");
      if (file_texture == NULL) 
         goto end_of_save;

         PGR2Texture_Header texture_header = 
         {
            width, height, format, data_size
         };
         // Write texture header
         if (fwrite(&texture_header, sizeof(PGR2Texture_Header), 1, file_texture) != 1)
         {
            goto end_of_save;
         }
         // Write texture data
         if (fwrite(texels, data_size, 1, file_texture) != 1)
         {
            goto end_of_save;
         }
      fclose(file_texture);

      glBindTexture(GL_TEXTURE_2D, 0);
   // Save texture to separate file --------------->

   // Save texture header to model file ----------->
      PGR2Model_TextureHeader model_tex_header;
      strcpy(model_tex_header.file_name, m_Textures[iTexture].file_name);
      if (fwrite(&model_tex_header, sizeof(PGR2Model_TextureHeader), 1, fout) != 1)
      {
         goto end_of_save;
      }
   // Save texture header to model file ----------->
   }
// Texture headers end -------------------------------------------->

// Material headers start ----------------------------------------->
   for (int iMaterial = 0; iMaterial < m_NumMaterials; iMaterial++)
   {
      // Write material header
      if (fwrite(&m_Materials[iMaterial], sizeof(MaterialData), 1, fout) != 1)
      {
         goto end_of_save;
      }
   }
// Material headers end ------------------------------------------->

// Mesh headers start --------------------------------------------->
   for (int iMesh = 0; iMesh < m_NumMeshes; iMesh++)
   {
      const MeshData& mesh = m_Meshes[iMesh];

      PGR2Model_MeshHeader mesh_header = 
      {
         (unsigned short)mesh.material_index,
         mesh.num_faces
      };

      // Write face header
      if (fwrite(&mesh_header, sizeof(PGR2Model_MeshHeader), 1, fout) != 1)
      {
         goto end_of_save;
      }

      for (int iFace = 0; iFace < mesh.num_faces; iFace++)
      {
         // Write face
         if (fwrite(&mesh.faces[iFace], sizeof(FaceData), 1, fout) != 1)
         {
            goto end_of_save;
         }
      }
   }
// Mesh headers end ----------------------------------------------->

// Vertices ------------------------------------------------------->
   if (fwrite(m_Vertices, 3*sizeof(GLfloat), m_NumVertices, fout) != m_NumVertices)
   {
      goto end_of_save;
   }
// TexCoords ------------------------------------------------------>
   if (m_TexCoords != NULL)
   {
      if (fwrite(m_TexCoords, 2*sizeof(GLfloat), m_NumTexCoords, fout) != m_NumTexCoords)
      {
         goto end_of_save;
      }
   }
// Normals -------------------------------------------------------->
   if (fwrite(m_Normals, 3*sizeof(GLfloat), m_NumNormals, fout) != m_NumNormals)
   {
      goto end_of_save;
   }
// Binormals ------------------------------------------------------>
   if (m_Binormals != NULL)
   {
      if (fwrite(m_Binormals, 3*sizeof(GLfloat), m_NumNormals, fout) != m_NumNormals)
      {
         goto end_of_save;
      }
// Tangents ------------------------------------------------------->
      if (fwrite(m_Tangents, 4*sizeof(GLfloat), m_NumNormals, fout) !=  m_NumNormals)
      {
         goto end_of_save;
      }
   }

   result = true;

end_of_save:
   fclose(fout);

   return result;
}
