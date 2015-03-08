#include "TreeBranch.h"


TreeBranch::TreeBranch(	tc* _parent,
						CoordSystem &_cs,
						float _x,
						TextureManager * _texMan,
						float length,
						float radiusAtBegin,
						float radiusAtEnd,
						int _divT,
						int _divR,
						float _c2,
						float _c4,
						v3 &_motionVector):
	TreeComponent(_parent, _cs, _x, _texMan)
{
	type	= ComponentType::BRANCH;
	L		= length;
	r1		= radiusAtBegin;
	r2		= radiusAtEnd;
	c2		= _c2;
	c4		= _c4;
	divT	= _divT;
	divR	= _divR;
	motionVector	= v3(_motionVector);

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
	for (i=0; i<MAX_HIERARCHY_DEPTH; i++){
		upVectors[i]	= v3(0.f, 0.f, 0.f);
		rightVectors[i] = v3(0.f, 0.f, 0.f);
		origins[i]		= v3(0.f, 0.f, 0.f);
	}	
	upVector	= originalCS.r;
	rightVector = originalCS.s;
	tVector		= originalCS.t;
	origin		= originalCS.origin;
	phase		= 1.f;
	TreeBranch *p;
	if (parent!=NULL){
		level = ((TreeBranch*)parent)->level + 1;
		// copy xvals
		p = (TreeBranch *) parent;
		for (i=0; i<MAX_HIERARCHY_DEPTH; i++){
			xvals.d[i] = p->xvals[i];
		}
		xvals.d[level-1] = _x;
	} else {
		level = 0;
	}
	init();
}
TreeBranch::TreeBranch(	tc* _parent,
						CoordSystem &_cs,
						CoordSystem &_objectCS,
						float _x,
						TextureManager * _texMan,
						float length,
						float radiusAtBegin,
						float radiusAtEnd,
						int _divT,
						int _divR,
						float _c2,
						float _c4,
						v3 &_motionVector):
	TreeComponent(_parent, _cs,_objectCS, _x, _texMan)
{
	type	= ComponentType::BRANCH;
	L		= length;
	r1		= radiusAtBegin;
	r2		= radiusAtEnd;
	c2		= _c2;
	c4		= _c4;
	divT	= _divT;
	divR	= _divR;
	motionVector	= _motionVector;
	verticesCount		= (divT+2)*divR;
	vertPtr				= new float[verticesCount*3];
	normalPtr			= new float[verticesCount*3];
	binormalPtr			= new float[verticesCount*3];
	tangentPtr			= new float[verticesCount*3];
	indexPtr			= new int [3*2*(divR)*(divT+1)];
	dataTextureCoords	= new float [verticesCount*2];

	phases				= v4(0.f, 0.f, 0.f, 0.f);
	xvals				= v4(-1.f, -1.f, -1.f, -1.f);
	lengths				= v4(0.f, 0.f, 0.f, 0.f);
	int i;
	for (i=0; i<MAX_HIERARCHY_DEPTH; i++){
		upVectors[i]	= v3(0.f, 0.f, 0.f);
		rightVectors[i] = v3(0.f, 0.f, 0.f);
		origins[i]		= v3(0.f, 0.f, 0.f);
	}	
	upVector	= originalCS.r;
	rightVector = originalCS.s;
	tVector		= originalCS.t;
	origin		= originalCS.origin;
	phase		= 1.f;
	TreeBranch *p;
	if (parent!=NULL){
		level = ((TreeBranch*)parent)->level + 1;
		// copy xvals
		p = (TreeBranch *) parent;
		for (i=0; i<MAX_HIERARCHY_DEPTH; i++){
			xvals.d[i] = p->xvals[i];
		}
		xvals.d[level-1] = _x;
	} else {
		level = 0;
	}
}

TreeBranch::~TreeBranch(void)
{
	delete [] vertPtr;
	delete [] normalPtr;
	delete [] binormalPtr;
	delete [] indexPtr;
	delete [] dataTextureCoords;
}

void TreeBranch::init()
{
	if (parent!=NULL){
		level = ((TreeBranch*) parent)->level+1;
	}
	// TODO:
	float bendR = 0.1+level/5;
	setBending(bendR, 0.0);
	Vertex *v;
	// generate vertices
	int i,j,k;
	int vi=0;
	int ii=0;
	float t = 0;
	float stepL = L/(divT+1);
	float angle = 0;
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
			v->textureCoords.y = t/L;			//[0..1]

			// set X values
			for (k=0; k<MAX_HIERARCHY_DEPTH; k++){
				v->x[k] = xvals.d[k];
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
}

// draw
void TreeBranch::draw()
{	
	/*
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		if (binormalID!=-1){
			// binormal present in shader...
			glVertexAttribPointer( binormalID, 3, GL_FLOAT, false, 0, binormalPtr );
			glEnableVertexAttribArray( binormalID );
			
		}
		if (tangentID!=-1){
			// binormal present in shader...
			glVertexAttribPointer( tangentID, 3, GL_FLOAT, false, 0, tangentPtr );
			glEnableVertexAttribArray( tangentID );
			
		}		
		glVertexPointer(3, GL_FLOAT, 0, vertPtr);
		glNormalPointer(GL_FLOAT, 0, normalPtr);

		// for each texture
		// color texture
		//glClientActiveTexture(GL_TEXTURE0);
		//glTexCoordPointer(2, GL_FLOAT, 0, dataTextureCoords); //TODO

		// data texture
		//glEnable(GL_TEXTURE_RECTANGLE_ARB);
		//glActiveTexture(DATA_TEX_UNIT);
		glClientActiveTexture(DATA_TEX_UNIT);
		glTexCoordPointer(2, GL_FLOAT, 0, dataTextureCoords);
				
		glDrawElements(GL_LINE_STRIP, indicesCount, GL_UNSIGNED_INT, this->indexPtr);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	if (binormalID!=-1){
		glDisableVertexAttribArray( binormalID );
	}
	if (tangentID!=-1){
		glDisableVertexAttribArray( tangentID );
	}
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		//fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
	*/
}

int	TreeBranch::getVertexCount()
{
	return vertices.size();
}
int	TreeBranch::getIndicesCount()
{
	return indicesCount;
}