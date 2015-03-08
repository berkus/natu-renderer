#ifndef __VERTEX__
#define __VERTEX__

#include "../common/Vector2.h"
#include "../common/Vector4.h"
#include "../common/Vector3.h"
#include "../src/settings.h"
#include <cstdlib>
#include <iostream>
#include <iomanip>


class Vertex{
public:
	Vertex(v3 _position, v3 _tangent, v3 _normal, v2 _texCoord, float _weight);
	Vertex(v3 _oPos, v3 _bPos, v3 _normal, v3 _tangent);
	Vertex(const Vertex &ve);
	Vertex();
	~Vertex();
	
	void	printOut(void);

	// nahradit vektorem w x-ovych souradnic v hierarchii  
	float	x[DYN_TREE::MAX_HIERARCHY_DEPTH];
	v3		oPos;
	v3		bPos;
	v3		normal;
	v3		tangent;
	v3		textureCoords;
	float   branchId;
	

};



#endif