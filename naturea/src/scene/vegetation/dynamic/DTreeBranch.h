#ifndef _DTREEBRANCH_H
#define _DTREEBRANCH_H

#include "DTreeComponent.h"
#include "IO/OBJTfile.h"

class DTreeBranch: public DTreeComponent
{
public:
	DTreeBranch(		tc* _parent,
						CoordSystem &_cs,
						float _x,
						float length,
						float radiusAtBegin,
						float radiusAtEnd,
						int _divT = 8,
						int _divR = 8
						);

	~DTreeBranch(void);

	// init
	void			init();
	
	//	leaf?
	bool			isLeaf(){ return false;	}

	int				getVertexCount();
	int				getIndicesCount();
	int				getLevel();

// variables
	int				divT;
	int				divR;
	float			r1, r2;
	float			L;
	int				id;


	v4				phases;
	v3				motionVectors	[DYN_TREE::MAX_HIERARCHY_DEPTH];
	v3      		upVectors		[DYN_TREE::MAX_HIERARCHY_DEPTH];
	v3				rightVectors	[DYN_TREE::MAX_HIERARCHY_DEPTH];
	v3				tVectors		[DYN_TREE::MAX_HIERARCHY_DEPTH];
	v3				origins			[DYN_TREE::MAX_HIERARCHY_DEPTH];
	v4				lengths;
	int				parentIDs		[DYN_TREE::MAX_HIERARCHY_DEPTH];

	float			phase;
	v3				upVector;
	v3				rightVector;
	v3				tVector;
	v3				origin;

	v3				motionVector;

	GLuint			indicesCount;
	GLuint			verticesCount;			
};

#endif
