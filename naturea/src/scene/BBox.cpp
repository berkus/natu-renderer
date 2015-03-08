#include "BBox.h"


BBox::BBox(void)
{
}
BBox::BBox(v3 &_minCorner, v3 &_maxCorner, v3 &_color)
{
	BBox();
	center  	= (_minCorner + _maxCorner)/2.0;
	sizes		= _maxCorner - _minCorner;
	color		= _color;
	drawMode    = GL_LINE;
	minCorner   = _minCorner;
	bounds[0]	= _minCorner;
	bounds[1]	= _maxCorner;

}

BBox::~BBox(void)
{

}

	
void BBox::draw()
{
	glMatrixMode(GL_MODELVIEW);
	glColor(color);
	glPushAttrib( GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_FOG_BIT);
		
	glPushMatrix();
	glTranslate(minCorner);
	glScale(sizes);
		glDisable(GL_LIGHTING);
		glDisable(GL_FOG);
		glPolygonMode(GL_FRONT_AND_BACK, drawMode);
		glBegin(GL_QUADS);
			//front
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,1.0,0.0);
			glVertex3f(1.0,1.0,0.0);
			glVertex3f(1.0,0.0,0.0);
			//back
			glVertex3f(0.0,0.0,1.0);
			glVertex3f(0.0,1.0,1.0);
			glVertex3f(1.0,1.0,1.0);
			glVertex3f(1.0,0.0,1.0);
			//left
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,1.0,0.0);
			glVertex3f(0.0,1.0,1.0);
			glVertex3f(0.0,0.0,1.0);
			//right
			glVertex3f(1.0,0.0,0.0);
			glVertex3f(1.0,1.0,0.0);
			glVertex3f(1.0,1.0,1.0);
			glVertex3f(1.0,0.0,1.0);
			//top
			glVertex3f(0.0,1.0,0.0);
			glVertex3f(0.0,1.0,1.0);
			glVertex3f(1.0,1.0,1.0);
			glVertex3f(1.0,1.0,0.0);
			//bottom
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(0.0,0.0,1.0);
			glVertex3f(1.0,0.0,1.0);
			glVertex3f(1.0,0.0,0.0);
		glEnd();





	glPopMatrix();
	glPopAttrib();

}

float BBox::getMinSize()
{
	return min( min(sizes.x, sizes.y), sizes.z);
}

float BBox::getDiameter()
{
	return sizes.length();
}

float BBox::getLongestInsidePathFromDir(v3 &dir){
	// from each corner shoot ray and check intersections...
	return 0.0;
}

bool BBox::intersect(Ray *pRay)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	tmin  = (bounds[ pRay->sign[0]    ].x - pRay->org.x) * pRay->inv_dir.x;
	tmax  = (bounds[ 1 - pRay->sign[0]].x - pRay->org.x) * pRay->inv_dir.x;
	tymin = (bounds[ pRay->sign[1]    ].y - pRay->org.y) * pRay->inv_dir.y;
	tymax = (bounds[ 1 - pRay->sign[1]].y - pRay->org.y) * pRay->inv_dir.y;
	if ( (tmin > tymax) || (tymin > tmax) )
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;
	tzmin = (bounds[ pRay->sign[2]     ].z - pRay->org.z) * pRay->inv_dir.z;
	tzmax = (bounds[ 1 - pRay->sign[2] ].z - pRay->org.z) * pRay->inv_dir.z;
	if ( (tmin > tzmax) || (tzmin > tmax) )
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;
	if ((tmin < pRay->max) && (tmax > pRay->min)){
		pRay->min = min2f(tmin, tmax);
		pRay->max = max2f(tmin, tmax);
		return true;
	}
	return false;
}


