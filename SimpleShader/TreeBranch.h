#ifndef _TREEBRANCH_H
#define _TREEBRANCH_H

#include "TreeComponent.h"
#include "OBJTfile.h"

class TreeBranch: public TreeComponent
{
public:
	TreeBranch(			tc* _parent,
						CoordSystem &_cs,
						float _x,
						TextureManager * _texMan,
						float length,
						float radiusAtBegin,
						float radiusAtEnd,
						int _divT,
						int _divR,
						float _c2,
						float _c4,
						v3 &_motionVector);

	TreeBranch(			tc* _parent,
						CoordSystem &_cs,
						CoordSystem &_objectCS,
						float _x,
						TextureManager * _texMan,
						float length,
						float radiusAtBegin,
						float radiusAtEnd,
						int _divT,
						int _divR,
						float _c2,
						float _c4,
						v3 &_motionVector);

	~TreeBranch(void);

	// init
	void			init();
	
	// draw
	void			draw();
	
	//	leaf?
	bool			isLeaf(){ return false;	}

	int				getVertexCount();
	int				getIndicesCount();

// variables
	int				divT;
	int				divR;
	float			r1, r2;
	float			c2, c4;
	float			L;
	int				id;
	int				level;

	v4				phases;
	v3				motionVectors[MAX_HIERARCHY_DEPTH];
	v3      		upVectors	[MAX_HIERARCHY_DEPTH];
	v3				rightVectors[MAX_HIERARCHY_DEPTH];
	v3				tVectors    [MAX_HIERARCHY_DEPTH];
	v3				origins		[MAX_HIERARCHY_DEPTH];
	v4				xvals;
	v4				lengths;

	float			phase;
	v3				upVector;
	v3				rightVector;
	v3				tVector;
	v3				origin;

	v3				motionVector;

	int				motionTextureID;


	GLuint			indicesCount;
	GLuint			verticesCount;			
};

#endif
