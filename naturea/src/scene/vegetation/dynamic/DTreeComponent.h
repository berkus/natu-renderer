#ifndef _DTREECOMPONENT_H
#define _DTREECOMPONENT_H

#include "settings.h"
#include "coordSystem.h"
#include <cstdlib>
#include <string>
#include <vector>
#include "vertex.h"
#include "Matrix3x3.h"
#include "utility/utils.h"
#include "utility/TextureManager.h"


// type of component
typedef enum {
	LEAF,
	BRANCH
} ComponentType;

class DTreeComponent;
typedef DTreeComponent tc; 

using namespace std;

class DTreeComponent
{
public:
	// constuctor
	DTreeComponent(tc* _parent, CoordSystem &_cs, float _x);
	
	// destructor
	~DTreeComponent(void);
	
	// leaf?
	virtual bool isLeaf()=0;
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
		GLuint			indicesCount;

		float			x;
		v4				xvals;
		CoordSystem		cs;		
		CoordSystem		originalCS;		
		int				level;
protected:
	void write(float * arr, v3 & vec3in, int index);
	void writeTex(float * texArr, float x, float y, int index);
	void print(float * arr, int cnt, int dataCnt);
};

#endif // _TREECOMPONENT_H