//-----------------------------------------------------------------------------
//  [PGR2] Common function definitions
//  27/02/2008
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

#include <math.h>



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% READ FILE                                              %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void* pgr2ReadFile(const char* file_name)
{
   char* buffer = NULL;

   // Read input file
   if (file_name != NULL) 
   {
      FILE* fin = fopen(file_name,"rb");
      if (fin != NULL) 
      {
         fseek(fin, 0, SEEK_END);
         long file_size = ftell(fin);
         rewind(fin);

         if (file_size > 0) 
         {
            buffer = (char*) malloc(file_size);
            int count = fread(buffer, sizeof(char), file_size, fin);
//          assert(count == file_size);
         }
         fclose(fin);
      }
   }

   return buffer;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% SOME GL FUNCTIONS                                      %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2AddQuaternionRotationToTransformation()
// Desc: 
//-----------------------------------------------------------------------------
inline void pgr2AddQuaternionRotationToTransformation(const float* quat)
{
#ifdef USE_ANTTWEAKBAR
   // quat = q(s, v)
   // A rotation about the unit vector U by an angle theta is defined:
   // s = cos(theta / 2), v = U.sin(theta / 2)
   const double THETA     = 2.0 * acos(quat[3]);
   const float  SIN_THETA = float(sin(THETA * 0.5));
   const float  THETA_DEG = float(180 * (THETA / 3.14159265));

   glRotatef(THETA_DEG, quat[0]*SIN_THETA, quat[1]*SIN_THETA, quat[2]*SIN_THETA);
#else
   // Fake implementation, but sufficient
   glRotatef(quat[0], 1.0f, 0.0f, 0.0f);
   glRotatef(quat[1], 0.0f, 1.0f, 0.0f);
   glRotatef(quat[2], 0.0f, 0.0f, 1.0f);
#endif
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% MESH GENERATORS                                        %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2CreatePlaneMesh()
// Desc: 
//-----------------------------------------------------------------------------
GLfloat* pgr2CreatePlaneMesh(float size, int slice_x, int slice_y, int* num_vertices)
{
   assert(num_vertices != NULL);

   // Compute position deltas for moving down the X, and Z axis during mesh creation
   const float dX = size / slice_x;
   const float dY = size / slice_y;
   const float startX = -size * 0.5f;
   const float startY = -size * 0.5f;

   const int NUM_VERTICES = 4*slice_x*slice_y;
   if (NUM_VERTICES < 4)
   {
      return NULL;
   }
   
   GLfloat* mesh = (GLfloat*) malloc((3*NUM_VERTICES)*sizeof(GLfloat));
   GLfloat* ptr  = mesh;

   for (int x = 0; x < slice_x; x++)
   {
      for (int y = 0; y < slice_y; y++)
      {
         *ptr++ = startX + x * dX;
         *ptr++ = startY + y * dY;
         *ptr++ = 0.0f;
         *ptr++ = startX + (x + 1) * dX;
         *ptr++ = startY + y * dY;
         *ptr++ = 0.0f;
         *ptr++ = startX + (x + 1) * dX;
         *ptr++ = startY + (y + 1) * dY;
         *ptr++ = 0.0f;
         *ptr++ = startX + x * dX;
         *ptr++ = startY + (y + 1) * dY;
         *ptr++ = 0.0f;
      }
   }

   if (num_vertices != NULL)
   {
      *num_vertices = NUM_VERTICES;
   }
   return mesh;
}


//-----------------------------------------------------------------------------
// Name: pgr2DrawSimpleMesh()
// Desc: draw simple planar mesh
//-----------------------------------------------------------------------------
bool pgr2DrawSimpleMesh(float size, int slice_x, int slice_y)
{
   int num_vertices = 0;
   GLfloat* vertices = pgr2CreatePlaneMesh(size, slice_x, slice_y, &num_vertices);
   if ((vertices != NULL) && (num_vertices > 0))
   {
      const float INV_SIZE = 1.0f/size;
      GLfloat*  tex_coords = (GLfloat*) malloc((2*num_vertices)*sizeof(GLfloat));
      for (int iVertex = 0; iVertex < num_vertices; iVertex++)
      {
         tex_coords[2*iVertex + 0] = vertices[3*iVertex + 0]*INV_SIZE + 0.5f;
         tex_coords[2*iVertex + 1] = vertices[3*iVertex + 1]*INV_SIZE + 0.5f;
      }

      GLboolean va_enabled = glIsEnabled(GL_VERTEX_ARRAY);
      GLboolean ta_enabled = glIsEnabled(GL_TEXTURE_COORD_ARRAY);
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, vertices);
      glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
      glDrawArrays(GL_QUADS, 0, num_vertices);
      free(vertices);
      free(tex_coords);

      if (va_enabled != GL_TRUE) glDisableClientState(GL_VERTEX_ARRAY);
      if (ta_enabled != GL_TRUE) glDisableClientState(GL_TEXTURE_COORD_ARRAY);

      return true;
   }
   return false;
}


//-----------------------------------------------------------------------------
// Name: pgr2CreateSphereMesh()
// Desc: based on OpenGL SuperBible example by Richard S. Wright Jr.
//-----------------------------------------------------------------------------
GLfloat* pgr2CreateSphereMesh(float radius, int slices, int stacks, 
                              int* num_vertices)
{
   assert(num_vertices != NULL);

   const float DRHO = 3.1415926536f / stacks;
   const float DTHETA = 2.0f * 3.1415926536f / slices;
   
   const int NUM_VERTICES = stacks*(2*slices+2);
   if (NUM_VERTICES < 1)
   {
      return 0;
   }

   GLfloat* mesh = (GLfloat*) malloc((3*NUM_VERTICES) * sizeof(GLfloat));
   GLfloat* ptr   = mesh;
   for (int iStack = 0; iStack < stacks; iStack++)
   {
      const GLfloat RHO = iStack * DRHO;
      const GLfloat SRHO = (GLfloat)(sin(RHO));
      const GLfloat CRHO = (GLfloat)(cos(RHO));
      const GLfloat SRHODRHO = (GLfloat)(sin(RHO + DRHO));
      const GLfloat CRHODRHO = (GLfloat)(cos(RHO + DRHO));

      for (int iSlice = 0; iSlice <= slices; iSlice++)
      {
         const GLfloat THETA  = (iSlice == slices) ? 0.0f : iSlice * DTHETA;
         const GLfloat STHETA = (GLfloat)(-sin(THETA));
         const GLfloat CTHETA = (GLfloat)(cos(THETA));

         *ptr++ = STHETA * SRHODRHO * radius;
         *ptr++ = CTHETA * SRHODRHO * radius;
         *ptr++ = CRHODRHO * radius;
         *ptr++ = STHETA * SRHO * radius;
         *ptr++ = CTHETA * SRHO * radius;
         *ptr++ = CRHO * radius;
      }
   }

   *num_vertices = NUM_VERTICES;
   return mesh;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% TEXTURE GENERATORS                                     %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
inline GLuint prg2CreateSimpleTexture(GLint width, GLint height)
{
   if (width * height <= 0)
   {
      return 0;
   }

   GLubyte* data = new GLubyte[width*height*4];
   assert(data != NULL);

   const GLubyte pattern_width = 16;
   GLubyte* ptr = data; 
   GLubyte pattern = 0xFF;

   for (int h = 0; h < height; h++) 
   {              
      for (int w = 0; w < width; w++) 
      {
         *ptr++ = pattern; 
         *ptr++ = pattern;
         *ptr++ = pattern;
         *ptr++ = 255;
         if (w % pattern_width == 0) pattern = ~pattern;
      }

      if (h % pattern_width == 0) pattern = ~pattern;
   }

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

   GLuint tex_id = 0;
   glGenTextures(1, &tex_id);
   glBindTexture(GL_TEXTURE_2D, tex_id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

   delete [] data;

   return tex_id;
} 


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%% SHADER RELATED FUNCTIONS                               %%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderInfoLog()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2CheckShaderInfoLog(GLuint shader_id)
{
   if (shader_id == 0)
   {
      return;
   }

   int log_length = 0;
   glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

   if (log_length > 0)
   {
      char* buffer  = (char*) malloc(log_length*sizeof(char));
      int   written = 0;
      glGetShaderInfoLog(shader_id, log_length, &written, buffer);
      printf("%s\n", buffer);
      free(buffer);
  }
}


//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderInfoLog()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2CheckProgramInfoLog(GLuint program_id)
{
   if (program_id == 0)
   {
      return;
   }

   int log_length = 0;
   glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

   if (log_length > 0)
   {
      char* buffer  = (char*) malloc(log_length*sizeof(char));
      int   written = 0;
      glGetProgramInfoLog(program_id, log_length, &written, buffer);
      printf("%s\n", buffer);
      free(buffer);
   }
}


//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderCompileStatus()
// Desc: 
//-----------------------------------------------------------------------------
GLint pgr2CheckShaderCompileStatus(GLuint shader_id)
{
   GLint status = GL_FALSE;
   glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
   return status;
}


//-----------------------------------------------------------------------------
// Name: pgr2CheckShaderCompileStatus()
// Desc: 
//-----------------------------------------------------------------------------
GLint pgr2CheckProgramLinkStatus(GLuint program_id)
{
   GLint status = GL_FALSE;
   glGetProgramiv(program_id, GL_LINK_STATUS, &status);
   return status;
}


//-----------------------------------------------------------------------------
// Name: pgr2CreateShaderFromSource()
// Desc: 
//-----------------------------------------------------------------------------
GLuint pgr2CreateShaderFromSource(GLenum shader_type, const char* source)
{
   if (source == NULL)
   {
      return 0;
   }

   switch (shader_type)
   {
      case GL_VERTEX_SHADER  :
      case GL_FRAGMENT_SHADER:
      case GL_GEOMETRY_SHADER:   break;
      default                :   return 0;
   }

   assert(glGetError() == GL_NO_ERROR);

   GLuint shader_id = glCreateShader(shader_type);
   if (shader_id == 0)
   {
      return 0;
   }

   glShaderSource(shader_id, 1, &source, NULL);
   glCompileShader(shader_id);

   if (pgr2CheckShaderCompileStatus(shader_id) != GL_TRUE)
   {
      printf("shader compilation failed.\n");
      pgr2CheckShaderInfoLog(shader_id);
  }

  assert(glGetError() == GL_NO_ERROR);
  return shader_id;
}

//-----------------------------------------------------------------------------
// Name: pgr2CreateShaderFromFile()
// Desc: 
//-----------------------------------------------------------------------------
GLuint pgr2CreateShaderFromFile(GLenum shader_type, const char* file_name)
{
   if (file_name == NULL)
   {
      return 0;
   }

   char* buffer = NULL;

   // Read input file
   if (file_name != NULL) 
   {
      FILE* fin = fopen(file_name,"rb");
      if (fin != NULL) 
      {
         fseek(fin, 0, SEEK_END);
         long file_size = ftell(fin);
         rewind(fin);

         if (file_size > 0) 
         {
            buffer = (char*) malloc(file_size + sizeof(char));
            int count = fread(buffer, sizeof(char), file_size, fin);
//            assert(count == file_size);
            buffer[count] = '\0';
         }
         fclose(fin);
      }
      else
      {
         return 0;
      }
   }

   assert(buffer != NULL);

   GLuint shader_id = pgr2CreateShaderFromSource(shader_type, buffer);
   free(buffer);

   return shader_id;
}


//-----------------------------------------------------------------------------
// Name: pgr2DestroyShaderProgram()
// Desc: 
//-----------------------------------------------------------------------------
void pgr2DestroyShaderProgram(GLuint* program_Id, GLuint* vs_Id, 
                                GLuint* gs_Id, GLuint* fs_Id)
{
   if ((program_Id != NULL) && (*program_Id > 0))
   {
      if ((vs_Id != NULL) && (*vs_Id > 0))
      {
         glDeleteShader(*vs_Id);
         *vs_Id = 0;
      }
      if ((gs_Id != NULL) && (*gs_Id != 0))
      {
         glDeleteShader(*gs_Id);
         *gs_Id = 0;
      }
      if ((fs_Id != NULL) && (*fs_Id  != 0))
      {
         glDeleteShader(*fs_Id);
         *fs_Id = 0;
      }

      glDeleteProgram(*program_Id);
      *program_Id = 0;
   }
}