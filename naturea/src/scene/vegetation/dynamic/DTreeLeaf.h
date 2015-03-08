#ifndef _DTREELEAF_H
#define _DTREELEAF_H


#include "DTreeComponent.h"
#include "DTreeBranch.h"

class DTreeLeaf :
	public DTreeComponent
{
public:
	DTreeLeaf(tc* _parent, CoordSystem &_cs, float _x, float _size);

	~DTreeLeaf(void);
	// init
	void			init();
	
	//	leaf?
	bool			isLeaf(){ return true;	}	
	int				getVertexCount();
// variables
	float			size;
	int				parentID;
	
};

#endif
