//-----------------------------------------------------------------------------
//  [PGR2] 3D model class
//  14/02/2011
//-----------------------------------------------------------------------------

#ifndef __PGR2_MODEL__
#define __PGR2_MODEL__

#include "scene\SceneModel.h"
#include "GLEW/glew.h"
#include "settings.h"
#include "utility\utils.h"
#include "utility\Shader.h"
#include "scene\BBox.h"


#define TEXTURE_NAME_SIZE 50

class PGR2Model: 
	public SceneModel
{
   protected:
		struct MaterialData
		{
		   GLfloat ambient[4];
		   GLfloat diffuse[4];
		   GLfloat specular[4];
		   GLfloat shininess;
		   GLfloat refraction;
		   GLfloat visibility;
		   bool    highlights;

		   short diffuse_tex_index;
		   short specular_tex_index;
		   short bump_tex_index;
		   short alpha_tex_index;
		   short height_tex_index;
		};

		struct TextureData
		{
		   GLuint id;
		   char file_name[TEXTURE_NAME_SIZE];
		};
   
		struct FaceData
		{
		   unsigned int ivertex[3];
		   unsigned int inormal[3];
		   unsigned int itexture[3];
		   GLfloat      normal[3];
		};

		struct MeshData
		{
		   unsigned short material_index;
		   unsigned long  num_faces;
		   FaceData*      faces;
		};
		
		struct VertexData
		{
			v3	position;
			v3	normal;
			v3	binormal;
			v3	facenormal;
			v2	texcoord;
		};
		
		struct Mesh
		{
			GLuint			eboId;
			int				eboCount;
			// textures
			int				texCount;
			int				*texIds;
			// material
			MaterialData	*material;
			GLint			binormalLoc;
			GLint			facenormalLoc;
			GLint			diffuse_tex_loc;
			GLint			height_tex_loc;
			GLint			bump_tex_loc;
			GLint			specular_tex_loc;
			int				enabledLoc;
			int				scaleBiasLoc;

			int				diffuse_tex_i;
			int				height_tex_i;
			int				bump_tex_i;
			int				specular_tex_i;
			Shader			*shader;
			bool			hasHeightMap;
			bool			hasBumpMap;
			bool			hasSpecularMap;
			bool			isTransparent;
		};


   protected:
		PGR2Model();
		PGR2Model(TextureManager *texManager);
   public:
		
		
		virtual ~PGR2Model();

		static PGR2Model* loadFromFile(const char* file_name,  TextureManager *texManager, ShaderManager * shManager, int level=1);
		       bool       saveToFile(const char* file_name);
		
		// =======================================
		// SceneModel Interface...

		virtual void draw();

		virtual void drawForLOD();

		virtual void init();

		virtual void update(double time);

		// ====================================

		void render(bool transparent_meshes);

		void renderVBO(bool transparent_meshes);


		void renderVertexNormals(float scale);
		void renderFaceNormals(float scale);

   protected:
		TextureData*   m_Textures;
		MaterialData*  m_Materials;
		MeshData*      m_Meshes;

		// Array of 3D vertices {x0, y0, z0}, {x1, y1, z1}, ...
		GLfloat*       m_Vertices;
		// Array of vertices' 2D texture coordinates {s0, t0}, {s1, t1}, ...
		GLfloat*       m_TexCoords;
		// Array of vertices' normals {n0x, n0y, n0z}, {n1x, n1y, n1z}
		GLfloat*       m_Normals;
		// Array of vertices' binormals {b0x, b0y, b0z}, {b1x, b1y, b1z}
		GLfloat*       m_Binormals;
		// Array of vertices' tangents {t0x, t0y, t0z, t0w}, {t1x, t1y, t1z, t1w}
		GLfloat*       m_Tangents; // Warning! Tangents are 4D vectors!!!

		unsigned short m_NumTextures;
		unsigned short m_NumMaterials;
		unsigned short m_NumMeshes;
		unsigned long  m_NumVertices;
		unsigned long  m_NumTexCoords;
		unsigned long  m_NumNormals;
		unsigned long  m_NumBinormals;
		
		GLuint	vboId;
		GLuint	vboCount;
		GLint	offsets	[VBO_ATR_COUNT];
		GLint	sizes	[VBO_ATR_COUNT];

		Mesh	**meshes;
		int		meshCount;

		

		// transparent
		GLuint  teboId;
		GLuint  teboCount;

public:
		int getNumTextures();
		bool compare(const PGR2Model& m);

		v3 minCorner;
		v3 maxCorner;

		int			level;
};


#endif // __PGR2_MODEL__
