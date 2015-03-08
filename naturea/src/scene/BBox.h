#ifndef _BBOX_H
#define _BBOX_H

#include "utility\utils.h"
#include "settings.h"
#include "models\cube.h"
#include "../utility/Ray.h"

class BBox
{
public:
	BBox(void);
	BBox(v3 &_minCorner, v3 &_maxCorner, v3 &_color = v3(1.0,0.f,0.f)); 
	~BBox(void);
	
	void draw();

	bool intersect(Ray *pRay);
	
	float getMinSize();
	float getDiameter();

	float getLongestInsidePathFromDir(v3 &dir);

	v3 sizes;
	v3 center;
	v3 minCorner;
	v3 color;
	GLenum drawMode;
private:
	v3 bounds[2];
};



#endif
