#ifndef __VERTEX__
#define __VERTEX__

#include "../_utils/Vector3.h"
#include "../_utils/Vector4.h"
#include "../_utils/Vector3.h"
#include "settings.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>


class Vertex{
public:
	Vertex(v3 _oPos, v3 _bPos, v3 _normal, v3 _tangent);
	Vertex(const Vertex &ve);
	Vertex();
	~Vertex();
	
	void	printOut(void);

	// nahradit vektorem w x-ovych souradnic v hierarchii  
	float	x[MAX_HIERARCHY_DEPTH];
	v3		oPos;
	v3		bPos;
	v3		normal;
	v3		tangent;
	v3		textureCoords;
	float   branchId;
	

};



#endif