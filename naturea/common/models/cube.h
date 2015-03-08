//-----------------------------------------------------------------------------
//  [PGR2] Cube model
//  27/02/2008
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

#ifndef __CUBE_MODEL_H__
#define __CUBE_MODEL_H__

#include "settings.h"

const GLfloat CUBE_VERTICES_ARRAY[] =
{
	 -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f
};
const GLfloat CUBE_COLORS_ARRAY[] =
{
	 1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
	 1.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f
};

const GLfloat CUBE_VERTEX_ARRAY[] =
{
	 -1.0f,-1.0f, 1.0f,  1.0f,-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f, -1.0f, 1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  1.0f,-1.0f,-1.0f,
	 -1.0f, 1.0f,-1.0f, -1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
	 -1.0f,-1.0f,-1.0f,  1.0f,-1.0f,-1.0f,  1.0f,-1.0f, 1.0f, -1.0f,-1.0f, 1.0f,
	  1.0f,-1.0f,-1.0f,  1.0f, 1.0f,-1.0f,  1.0f, 1.0f, 1.0f,  1.0f,-1.0f, 1.0f,
	 -1.0f,-1.0f,-1.0f, -1.0f,-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f
};
const GLfloat CUBE_VERTEX_ARRAY2[] =
{
	 0.0f,0.0f,1.0f, 1.0f,0.0f,1.0f, 1.0f,1.0f,1.0f, 0.0f,1.0f,1.0f,
	 0.0f,0.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,0.0f,0.0f,
	 0.0f,1.0f,0.0f, 0.0f,1.0f,1.0f, 1.0f,1.0f,1.0f, 1.0f,1.0f,0.0f,
	 0.0f,0.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,0.0f,1.0f, 0.0f,0.0f,1.0f,
	 1.0f,0.0f,0.0f, 1.0f,1.0f,0.0f, 1.0f,1.0f,1.0f, 1.0f,0.0f,1.0f,
	 0.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,1.0f,1.0f, 0.0f,1.0f,0.0f
};

const GLfloat CUBE_COLOR_ARRAY[] =
{
	1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 1.0f,
};

const GLfloat CUBE_TEX_COORD_ARRAY[] =
{
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
	 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
};

const GLfloat CUBE_NORMAL_ARRAY[] =
{
    0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,-1.0f,
    0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,  0.0f,-1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
};

const GLfloat CUBE_COLOR_AND_VERTEX_ARRAY[] =
{
   // <cr, cg, cb>, <vx, vy, vx>
    1.0f, 0.0f, 0.0f, -1.0f,-1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f,-1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, -1.0f,-1.0f,-1.0f,   0.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,
    1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,   1.0f, 0.0f, 0.0f,  1.0f,-1.0f,-1.0f,  
    0.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,   1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
    1.0f, 0.0f, 1.0f, -1.0f,-1.0f,-1.0f,   1.0f, 0.0f, 0.0f,  1.0f,-1.0f,-1.0f,
    0.0f, 1.0f, 0.0f,  1.0f,-1.0f, 1.0f,   1.0f, 0.0f, 0.0f, -1.0f,-1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,  1.0f,-1.0f,-1.0f,   1.0f, 1.0f, 1.0f,  1.0f, 1.0f,-1.0f,
    0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  1.0f,-1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, -1.0f,-1.0f,-1.0f,   1.0f, 0.0f, 0.0f, -1.0f,-1.0f, 1.0f,
    1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f, -1.0f, 1.0f,-1.0f,
};


const GLuint CUBE_INDEX_ARRAY[] =
{
   0, 1, 2, 3,
	4, 5, 6, 7,
	5, 3, 2, 6,
	4, 7, 1, 0,
	7, 6, 2, 1,
	4, 0, 3, 5
};

const int NUM_CUBE_VERTICES = sizeof(CUBE_VERTEX_ARRAY) / (3*sizeof(GLfloat));
const int NUM_CUBE_INDICES  = sizeof(CUBE_INDEX_ARRAY) / sizeof(GLuint);

extern GLuint cube_vbo_id;
extern GLuint cube_ebo_id;
extern GLuint plane_vbo_id;
extern GLuint plane_ebo_id;

static void deleteCube(){
	glDeleteBuffers(1,&cube_ebo_id);
	glDeleteBuffers(1,&cube_vbo_id);
}
static void initCube(){
	glGenBuffers(1,&cube_ebo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_CUBE_INDICES*sizeof(GLubyte), CUBE_INDEX_ARRAY, GL_STATIC_DRAW);
	glGenBuffers(1,&cube_vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_id);
		// alloc space
		GLsizeiptr size3 =  3*NUM_CUBE_VERTICES*sizeof(GLfloat);
		//int size2 =  2*NUM_CUBE_VERTICES*sizeof(GLfloat);
		glBufferData(GL_ARRAY_BUFFER, 2*size3, 0, GL_STATIC_DRAW); 
		// fill vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, size3, CUBE_VERTEX_ARRAY2); 

		// fill normals
		glBufferSubData(GL_ARRAY_BUFFER, size3, size3, CUBE_NORMAL_ARRAY);
		// fill texcoords
		//glBufferSubData(GL_ARRAY_BUFFER, 2*size3, size2, CUBE_NORMAL_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
};
//-----------------------------------------------------------------------------
// Name: drawCube()
// Desc: 
//-----------------------------------------------------------------------------
static void drawCube()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo_id);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo_id);
	   glEnableClientState(GL_VERTEX_ARRAY);
	   glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(3*NUM_CUBE_VERTICES*sizeof(GLfloat)));
		//glDrawArrays(GL_QUADS, 0, NUM_CUBE_VERTICES);
	   glDrawElements(GL_QUADS, NUM_CUBE_INDICES, GL_UNSIGNED_INT, BUFFER_OFFSET(0)); 
	//                  CUBE_INDEX_ARRAY);

	   glDisableClientState(GL_VERTEX_ARRAY);
	   glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
};

const GLfloat PLANE_VERTEX_ARRAY2[] =
{
	 -0.5f, 0.0f,  0.0f,
	  0.5f, 0.0f,  0.0f,
	  0.5f, 1.0f,  0.0f,
	 -0.5f, 1.0f,  0.0f	
};


const GLfloat PLANE_VERTEX_ARRAY[] =
{
	 -0.5f, 0.0f, -0.5f,
	  0.5f, 0.0f, -0.5f,
	  0.5f, 0.0f,  0.5f,
	 -0.5f, 0.0f,  0.5f	
};

const GLfloat PLANE_TEX_COORD_ARRAY[] =
{
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f
};
const GLfloat PLANE_TEX_COORD_ARRAY2[] =
{
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 1.0f, 1.0f,
	 0.0f, 1.0f
};


const GLfloat PLANE_NORMAL_ARRAY[] =
{
    0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
};
const GLfloat PLANE_NORMAL_ARRAY2[] =
{
    0.0f, 0.0f,-1.0f,
	0.0f, 0.0f,-1.0f,
	0.0f, 0.0f,-1.0f,
	0.0f, 0.0f,-1.0f
};
const GLfloat PLANE_TANGENT_ARRAY[] =
{
    1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f
};
const GLubyte PLANE_INDEX_ARRAY[] =
{
   0, 1, 2, 3
};

const int NUM_PLANE_VERTICES = sizeof(PLANE_VERTEX_ARRAY) / (3*sizeof(GLfloat));
const int NUM_PLANE_INDICES  = sizeof(PLANE_INDEX_ARRAY) / sizeof(GLubyte);

static void drawPlane(){
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_ebo_id);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo_id);
	   glEnableClientState(GL_VERTEX_ARRAY);
	   glEnableClientState(GL_NORMAL_ARRAY);
	   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
		glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(3*NUM_PLANE_VERTICES*sizeof(GLfloat)));
		glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(6*NUM_PLANE_VERTICES*sizeof(GLfloat)));
	    glDrawElements(GL_QUADS, NUM_PLANE_INDICES, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0)); 
	   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	   glDisableClientState(GL_VERTEX_ARRAY);
	   glDisableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


};
static void deletePlane(){
	glDeleteBuffers(1,&plane_ebo_id);
	glDeleteBuffers(1,&plane_vbo_id);
}

static void initPlane(){
	glGenBuffers(1,&plane_ebo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_ebo_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_PLANE_INDICES*sizeof(GLubyte), PLANE_INDEX_ARRAY, GL_STATIC_DRAW);
	glGenBuffers(1,&plane_vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo_id);
		// alloc space
		GLsizeiptr size3 =  3*NUM_PLANE_VERTICES*sizeof(GLfloat);
		GLsizeiptr size2 =  2*NUM_PLANE_VERTICES*sizeof(GLfloat);
		glBufferData(GL_ARRAY_BUFFER, 2*size3+size2, 0, GL_STATIC_DRAW); 
		// fill vertices
		glBufferSubData(GL_ARRAY_BUFFER, 0, size3, PLANE_VERTEX_ARRAY); 

		// fill normals
		glBufferSubData(GL_ARRAY_BUFFER, size3, size3, PLANE_NORMAL_ARRAY);
		// fill texcoords
		glBufferSubData(GL_ARRAY_BUFFER, 2*size3, size2, PLANE_TEX_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
};

#endif // __CUBE_MODEL_H__