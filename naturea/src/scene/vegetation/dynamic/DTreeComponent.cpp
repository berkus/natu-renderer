#include "DTreeComponent.h"

DTreeComponent::DTreeComponent(tc* _parent, CoordSystem &_cs, float _x):
	x(_x),
	originalCS(_cs),
	cs(_cs),
	parent(_parent)
{
	if (parent!=NULL){
		parent->children.push_back(this);
	}
	binormalID= -1;
	tangentID = -1;
	vertPtr = normalPtr = binormalPtr = tangentPtr = dataTextureCoords = col1TextureCoords = col2TextureCoords = NULL;
	indexPtr = NULL;
}

DTreeComponent::~DTreeComponent(void)
{
	int i;
	for (i = 0; i< vertices.size(); i++){
		SAFE_DELETE_PTR( vertices[i] );
	}
	SAFE_DELETE_ARRAY_PTR( vertPtr				);
	SAFE_DELETE_ARRAY_PTR( normalPtr 			);
	SAFE_DELETE_ARRAY_PTR( binormalPtr 			);
	SAFE_DELETE_ARRAY_PTR( tangentPtr 			);
	SAFE_DELETE_ARRAY_PTR( dataTextureCoords	);
	SAFE_DELETE_ARRAY_PTR( col1TextureCoords	);
	SAFE_DELETE_ARRAY_PTR( col2TextureCoords	);
	SAFE_DELETE_ARRAY_PTR( indexPtr 			);
}

// methods
	
		// set bending


void DTreeComponent::write(float * arr, v3 & vec3in, int index){
	arr[3*index + 0] = vec3in.x;
	arr[3*index + 1] = vec3in.y;
	arr[3*index + 2] = vec3in.z;
}
void DTreeComponent::writeTex(float * texArr, float x, float y, int index){
	texArr[2*index + 0] = x;
	texArr[2*index + 1] = y;
}

void DTreeComponent::print(float * arr, int cnt, int dataCnt){
	int i;
	for (int k=0; k<cnt*dataCnt; k+=dataCnt){
		printf("[%i]",k/dataCnt);
		for (i=0; i<dataCnt; i++){
			printf("%f ", arr[k+i]);

		}
		printf("\n");
	}
}
		
		
