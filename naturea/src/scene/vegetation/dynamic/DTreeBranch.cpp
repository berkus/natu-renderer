#include "DTreeBranch.h"


DTreeBranch::DTreeBranch(	tc* _parent,
						CoordSystem &_cs,
						float _x,
						float length,
						float radiusAtBegin,
						float radiusAtEnd,
						int _divT,
						int _divR):
	DTreeComponent(_parent, _cs, _x)
{
	type	= ComponentType::BRANCH;
	L		= length;
	r1		= radiusAtBegin;
	r2		= radiusAtEnd;
	divT	= _divT;
	divR	= _divR;
	vertPtr				= new float[(divT+2)*divR*3];
	normalPtr			= new float[(divT+2)*divR*3];
	binormalPtr			= new float[(divT+2)*divR*3];
	tangentPtr			= new float[(divT+2)*divR*3];
	indexPtr			= new int [3*2*(divR)*(divT+1)];
	dataTextureCoords	= new float [(divT+2)*divR*2];

	phases				= v4(0.f, 0.f, 0.f, 0.f);
	xvals				= v4(-1.f, -1.f, -1.f, -1.f);
	lengths				= v4(0.f, 0.f, 0.f, 0.f);
	int i;
	for (i=0; i<DYN_TREE::MAX_HIERARCHY_DEPTH; i++){
		upVectors[i]	= v3(0.f, 0.f, 0.f);
		rightVectors[i] = v3(0.f, 0.f, 0.f);
		origins[i]		= v3(0.f, 0.f, 0.f);
		parentIDs[i]	= 0;
	}	
	upVector	= originalCS.r;
	rightVector = originalCS.s;
	tVector		= originalCS.t;
	origin		= originalCS.origin;
	phase		= 1.f;
	DTreeBranch *p;
	if (parent!=NULL){
		level = ((DTreeBranch*)parent)->level + 1;
		// copy xvals
		p = (DTreeBranch *) parent;
		for (i=0; i<DYN_TREE::MAX_HIERARCHY_DEPTH; i++){
			xvals[i] = p->xvals[i];
		}
		xvals[level-1] = _x;
	} else {
		level = 0;
	}
	init();
}

int	DTreeBranch::getLevel()
{
	return 0;
}

DTreeBranch::~DTreeBranch(void)
{
}

void DTreeBranch::init()
{
	float angle = randomf(0.f, 2*M_PI );
	float scale = 1.0;
	motionVector = v3(scale*cos(angle), scale*sin(angle), 0.f);
	if (parent!=NULL){
		level = ((DTreeBranch*) parent)->level+1;
	}

	Vertex *v;
	// generate vertices
	int i,j,k;
	int vi=0;
	int ii=0;
	float t = 0;
	float stepL = L/(divT+1);
	float r = 0, s=0, R=0;
	// each ring:
	for (i=0; i<divT+2; i++){
		// position along the branch
		t = i*stepL;
		// interpolate radius along the branch
		R = (1-t/L)*(r1 - r2)+r2;

		// each vertex on ring:
		for (j=0; j<divR; j++){
		
			angle = TWO_PI/divR * j;
			v3 pos, oTangent, oNormal, oBinormal;
			r = R*cos(angle);
			s = R*sin(angle);
			pos+= (originalCS.r * r); // r
			pos+= (originalCS.s * s); // s
			pos+= (originalCS.t * t); // t
		
			oTangent	= originalCS.t.getNormalized();
			oNormal		= v3(r,s,0).getNormalized();//originalCS.r.getNormalized();
			oBinormal	= oTangent.cross(oNormal);
			pos+=  originalCS.origin;
			v3 bPos(r,s,t);

			// add in vertices array
			v = new Vertex(pos, bPos, oNormal, oTangent);
			// set texture coords (color texture)
			v->textureCoords.x = angle/TWO_PI;	//[0..1]
			v->textureCoords.y = t*10/L;//t/L;			//[0..1]

			// set X values
			for (k=0; k<DYN_TREE::MAX_HIERARCHY_DEPTH; k++){
				v->x[k] = xvals[k];
			}
			v->x[level] = t/L;
			vertices.push_back(v);

			// fill texture coords [x-vals, branch ids]
			dataTextureCoords[2*vi/3] = t/L;
			dataTextureCoords[2*vi/3+1] = float(this->id);
			normalPtr[vi]	 = oNormal.x;	
			vertPtr[vi]		 = bPos.x;
			binormalPtr[vi]	 = oBinormal.x;
			tangentPtr[vi]	 = oTangent.x;
			vi++;
			normalPtr[vi]	 = oNormal.y;
			vertPtr[vi]		 = bPos.y;
			binormalPtr[vi]	 = oBinormal.y;
			tangentPtr[vi]	 = oTangent.y;
			vi++;
			normalPtr[vi]	 = oNormal.z;
			vertPtr[vi]		 = bPos.z;
			binormalPtr[vi]	 = oBinormal.z;
			tangentPtr[vi]	 = oTangent.z;
			vi++;

			// fill indices array
			if (i<divT+1){
				// not for the last ring

				indexPtr[ii] = (i*divR)+j;
				ii++;
				indexPtr[ii] = (i*divR)+ divR+j;
				ii++;
				indexPtr[ii] = (i*divR)+ (j+1)%divR;
				ii++;
				
				indexPtr[ii] = (i*divR)+ j;
				ii++;
				indexPtr[ii] = (i*divR)+ divR+(divR + j - 1)%divR;
				ii++;
				indexPtr[ii] = (i*divR)+ divR + j;
				ii++;
			}

		}
	}			
	indicesCount = ii;
	

	//printf("branch indices count: %i \n", indicesCount);

	//printf("branch vertices count: %i \n", vertices.size());
}


int	DTreeBranch::getVertexCount()
{
	return vertices.size();
}
int	DTreeBranch::getIndicesCount()
{
	return indicesCount;
}