#include "TreeComponent.h"


TreeComponent::TreeComponent(tc* _parent, CoordSystem &_cs, float _x, TextureManager * _texMan):
	x(_x),
	originalCS(_cs),
	cs(_cs),
	parent(_parent),
	texManager(_texMan)
{
	if (parent!=NULL){
		parent->children.push_back(this);
	}

	shaderProgramID = NULL;
	binormalID= -1;
	tangentID = -1;
	vertPtr = normalPtr = binormalPtr = NULL;
	indexPtr = NULL;

}
TreeComponent::TreeComponent(tc* _parent, CoordSystem &_cs, CoordSystem &_objectCS, float _x, TextureManager * _texMan):
	x(_x),
	originalCS(_cs),
	//objectCS(_objectCS),
	cs(_cs),
	parent(_parent),
	texManager(_texMan)
{
	if (parent!=NULL){
		parent->children.push_back(this);
	}

	shaderProgramID = NULL;
	binormalID= -1;
	tangentID = -1;
	vertPtr = normalPtr = binormalPtr = NULL;
	indexPtr = NULL;

}


TreeComponent::~TreeComponent(void)
{
	/*DEL( vertPtr		);
	DEL( normalPtr		);
	DEL( binormalPtr	);
	DEL( tangentPtr		);
	DEL( indexPtr		);*/
	children.clear();
}

// methods
	
		// set bending
void TreeComponent::setBending(float _Ar, float _As)
{
	Ar = _Ar;
	As = _As;
}

void TreeComponent::write(float * arr, v3 & vec3in, int index){
	arr[3*index + 0] = vec3in.x;
	arr[3*index + 1] = vec3in.y;
	arr[3*index + 2] = vec3in.z;
}
void TreeComponent::writeTex(float * texArr, float x, float y, int index){
	texArr[2*index + 0] = x;
	texArr[2*index + 1] = y;
}

void TreeComponent::print(float * arr, int cnt, int dataCnt){
	int i;
	for (int k=0; k<cnt*dataCnt; k+=dataCnt){
		printf("[%i]",k/dataCnt);
		for (i=0; i<dataCnt; i++){
			printf("%f ", arr[k+i]);

		}
		printf("\n");
	}
}
		
		
