#ifndef _TREECOMPONENT_H
#define _TREECOMPONENT_H

#include "coordSystem.h"
#include <cstdlib>
#include <string>
#include <vector>
#include "vertex.h"
#include "Matrix3x3.h"
#include "utility/utils.h"
#include "utility/TextureManager.h"
#include "settings.h"

// type of component
typedef enum {
	LEAF,
	BRANCH
} ComponentType;

class TreeComponent;
typedef TreeComponent tc; 

using namespace std;

class TreeComponent
{
public:
	// constuctor
	TreeComponent(tc* _parent, CoordSystem &_cs, float _x, TextureManager * _texMan);
	TreeComponent(tc* _parent, CoordSystem &_cs, CoordSystem &_objectCS, float _x, TextureManager * _texMan);
	
	// destructor
	~TreeComponent(void);
	
	// methods
		virtual void init()=0;
		// draw
		virtual void draw()=0;
		// leaf?
		virtual bool isLeaf()=0;
		
		// set bending
		void setBending(float _Ar, float _As);
		
	// variables
		ComponentType	type;
	
		tc*				parent;

		vector<tc*>		children;
		vector<Vertex*> vertices;
		
		int *			indexPtr;
		float *			vertPtr;		
		float *			normalPtr;
		float *			binormalPtr;
		float *			tangentPtr;
		float *			dataTextureCoords;
		float *			col1TextureCoords;
		float *			col2TextureCoords;

		GLint			binormalID;
		GLint			tangentID;
		int				colorTextureID;
		int				bumpTextureID;
		int				dataTextureID;
		GLuint			indicesCount;

		float			x;
		float			Ar;
		float			As;

		CoordSystem		cs;		
		CoordSystem		originalCS;
//		CoordSystem		objectCS;

		GLuint			shaderProgramID;

		TextureManager * texManager;


		

protected:
	void write(float * arr, v3 & vec3in, int index);
	void writeTex(float * texArr, float x, float y, int index);
	void print(float * arr, int cnt, int dataCnt);
};

#endif // _TREECOMPONENT_H