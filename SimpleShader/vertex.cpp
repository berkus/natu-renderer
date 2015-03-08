#include "vertex.h"

Vertex::Vertex(v3 _oPos, v3 _bPos, v3 _normal, v3 _tangent){
	oPos = _oPos;
	bPos = _bPos;
	normal = _normal;
	tangent = _tangent;
}
Vertex::Vertex(const Vertex &ve){
	oPos = ve.oPos;
	bPos = ve.bPos;
	normal = ve.normal;
	tangent = ve.tangent;
	int i;
	for (i=0; i<MAX_HIERARCHY_DEPTH; i++){
		x[i] = ve.x[i];
	}
	textureCoords = ve.textureCoords;
	branchId = ve.branchId;
}
Vertex::Vertex(){

}

Vertex::~Vertex(){
}

void Vertex::printOut(void){
	printf ( "Vertex [%f, %f, %f]\n", oPos.x, oPos.y, oPos.z);
	
}