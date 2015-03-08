#ifndef _RAY_H
#define _RAY_H

#include "float.h"
#include "Vector4.h"

class Ray
{
public:
	Ray(v3 &_org, v3 &_dir);
	~Ray();
	v3 color;
	float min, max;

	v3 org;
	v3 dir;
	v3 inv_dir;
	int sign[3];	
	
};


#endif