#include "TreeLeaf.h"


TreeLeaf::TreeLeaf(
		tc* _parent,
		CoordSystem &_cs,
		float _x,
		TextureManager * _texMan, 
		float _size,
		v3 &_motionVector         ):
	TreeComponent(_parent, _cs, _x, _texMan)
{
	
	cs				= parent->originalCS.getSystemInThisSystem(originalCS);
	type			= ComponentType::LEAF;
	size			= _size;
	parentID		= ((TreeBranch*)parent)->id;
	xvals			= v4(-1.f, -1.f, -1.f, -1.f);
}


TreeLeaf::~TreeLeaf(void)
{
}

int TreeLeaf::getVertexCount(){
	return vertices.size();
}

void TreeLeaf::init()
{
	parentID = ((TreeBranch*)parent)->id;
	// inherit xvals
	int k;
	TreeBranch *p;
	int i;
	if (parent!=NULL){
		level = ((TreeBranch*)parent)->level + 1;
		// copy xvals
		p = (TreeBranch *) parent;
		for (i=1; i<MAX_HIERARCHY_DEPTH; i++){
			xvals.d[i-1] = p->xvals[i];
		}
		xvals.d[level-1] = x;
	} else {
		level = 0;
	}

	// create quad...

	v3 normal	= cs.t;
	v3 binormal = cs.s;
	v3 tangent	= cs.r;

	v3 p1 = v3(-size/2.f, 0.0, 0.0);
	v3 p2 = v3( size/2.f, 0.0, 0.0);
	v3 p3 = v3( size/2.f, size, 0.0);
	v3 p4 = v3(-size/2.f, size, 0.0);
	v3 t1 = v3(0.0, 0.0);
	v3 t2 = v3(1.0, 0.0);
	v3 t3 = v3(1.0, 1.0);
	v3 t4 = v3(0.0, 1.0);

	Vertex *v;
	v = new Vertex(p1, p1, normal, tangent);
	v->textureCoords = t1;
	for (k=0; k<MAX_HIERARCHY_DEPTH; k++){
		v->x[k] = xvals.d[k];
	}
	vertices.push_back(v);

	v = new Vertex(p2, p2, normal, tangent);
	v->textureCoords = t2;
	for (k=0; k<MAX_HIERARCHY_DEPTH; k++){
		v->x[k] = xvals.d[k];
	}
	vertices.push_back(v);		

	v = new Vertex(p3, p3, normal, tangent);
	v->textureCoords = t3;
	for (k=0; k<MAX_HIERARCHY_DEPTH; k++){
		v->x[k] = xvals.d[k];
	}
	vertices.push_back(v);

	v = new Vertex(p4, p4, normal, tangent);
	v->textureCoords = t4;
	for (k=0; k<MAX_HIERARCHY_DEPTH; k++){
		v->x[k] = xvals.d[k];
	}
	vertices.push_back(v);
}


void TreeLeaf::draw()
{
}