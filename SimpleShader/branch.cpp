#include "branch.h"
#include "data.h"
extern bool g_bUseShaders;
Branch::Branch(Branch * _parent, float _xval, CoordSystem & _cs, float length, float radiusAtBegin, float radiusAtEnd, int _divT, int _divR, float _c2, float _c4):c2(_c2),c4(_c4){
	
	parent = NULL;
	bCs = _cs;
	originalCS = _cs;
	x =_xval;
	divT = _divT;
	divR = _divR;
	if (_parent!=NULL) {
		_parent->addChildBranch(this, _xval);
	}
	L  = length;
	r1 = radiusAtBegin;
	r2 = radiusAtEnd;
	vertices = new float[(divT+2)*divR*3];
	normals  = new float[(divT+2)*divR*3];
	indices  = new int [4*(divR)*(divT+1)];
	textureCoords = new float [(divT+2)*divR*2];
	/*vertices = new float[(divT+2)*3];
	normals  = new float[(divT+2)*3];
	indices  = new int [(divT+1)];
	textureCoords = new float [(divT+2)*2];*/
	// generate vertices
	init();
};

void Branch::init(){
	// generate vertices
	int i,j;
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
			v3 pos, oTangent, oNormal;
			r = R*cos(angle);
			s = R*sin(angle);
			pos+= (originalCS.r * r); // r
			pos+= (originalCS.s * s); // s
			pos+= (originalCS.t * t); // t
		
			oTangent += originalCS.t * t;
			oNormal = originalCS.r;
			oNormal.normalize();
			oTangent.normalize();
		
			pos+=  originalCS.origin;
			v3 bPos(r,s,t);

			// add in vertices array
			Vertex v(pos, bPos, oNormal, oTangent);
			//Vertex v(pos, bPos, oNormal, oTangent, t/L);
			vertArr.push_back(v);

			// fill texture coords [x-vals, branch ids]
			textureCoords[2*vi/3] = v.x[0];
			textureCoords[2*vi/3+1] = float(this->id);
			normals[vi]	 = oNormal.x;	
			vertices[vi] = bPos.x;
			vi++;
			normals[vi]	 = oNormal.y;
			vertices[vi] = bPos.y;
			vi++;
			normals[vi]	 = oNormal.z;
			vertices[vi] = bPos.z;
			vi++;

			// fill indices array
			if (i<divT+1){
				// not for the last ring
				if (j!=0){
					indices[ii] = ((i+1)*divR+j);
					ii++;

					indices[ii] = ((i)*divR+j);
					ii++;

				} 
				indices[ii] = (i*divR+j); 
				ii++;

				indices[ii] = ((i+1)*divR+j); 
				ii++;
			}

		}
		if (i<divT+1){
			indices[ii] = ((i+1)*divR); 
			ii++;

			indices[ii] = ((i)*divR);
			ii++;
		}
	}			
	indicesCount = ii;
	/*/
	for (i=0; i<divT+2; i++){
		// position along the branch
		t = i*stepL;
		// interpolate radius along the branch
//		R = (1-t/L)*(radiusAtBegin - radiusAtEnd)+radiusAtEnd;
R=0;
		// each vertex on ring:
//		for (j=0; j<divR; j++){
		
		angle = TWO_PI/divR;
		v3 pos, oTangent, oNormal;
		r = R*cos(angle);
		s = R*sin(angle);
		pos+= (bCs.r * r); // r
		pos+= (bCs.s * s); // s
		pos+= (bCs.t * t); // t
		
		oTangent += bCs.t * t;
		oNormal = bCs.r;
		oNormal.normalize();
		oTangent.normalize();
		
		pos+=  bCs.origin;
		v3 bPos(r,s,t); 

		// add in vertices array
		Vertex v(pos, bPos, oNormal, oTangent, t/L);
		vertArr.push_back(v);

		// fill texture coords [x-vals]
		textureCoords[i*2]   = v.x;
		textureCoords[i*2+1] = 0.0f;

		normals[vi]	 = oNormal.x;	
		vertices[vi] = bPos.x;
		vi++;
		normals[vi]	 = oNormal.y;
		vertices[vi] = bPos.y;
		vi++;
		normals[vi]	 = oNormal.z;
		vertices[vi] = bPos.z;
		vi++;
		indices[ii] = ii; 
		ii++;	
	}
	indicesCount = ii;
	*/
	// print out arrays
	for (i=0; i<(divT+2)*divR*2; i+=2){
		printf("tc[%i]= %f , %f\n", i/2, textureCoords[i], textureCoords[i+1]);
	}
	for (i=0; i<(divT+2)*divR*3; i+=3){
		printf("v[%i]= %f , %f , %f\n", i/3, vertices[i], vertices[i+1], vertices[i+2]);
		//printf("n[%i]= %f , %f , %f\n", i/3, normals[i], normals[i+1], normals[i+2]);
	}
	for (i=0; i<4*(divR)*(divT+1); i++){
		printf("ind[%i]= %i\n", i, indices[i]);
	}

}

void Branch::draw(){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices);
		glNormalPointer(GL_FLOAT, 0, normals);
		glClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
		glDrawElements(GL_LINE_STRIP, indicesCount, GL_UNSIGNED_INT, indices);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		fprintf (stderr, "OpenGL Error: %s\n", errString);
	}
};

void Branch::update(){
	if( !g_bUseShaders ) {
		// bend coordinate system
		CoordSystem cs = originalCS;
		cs.origin = cs.origin * 0.f;
		bCs = bendCoordSystem(cs, 0);
		// recalc positions of vertices
	
		Vertex v;
		for (int i=0; i<vertArr.size(); i++){
			v = bendVertex(vertArr[i]);

			normals [3*i    ] = v.normal.x;	
			vertices[3*i    ] = v.oPos.x;
			normals [3*i + 1] = v.normal.y;	
			vertices[3*i + 1] = v.oPos.y;
			normals [3*i + 2] = v.normal.z;	
			vertices[3*i + 2] = v.oPos.z;

		}
	}

};

// bend vertex
Vertex	Branch::bendVertex(Vertex &v){
	CoordSystem vCS;
	vCS.origin	= v.bPos;
	vCS.r		= v.tangent;
	vCS.s		= v.tangent.cross(v.normal);
	vCS.t		= v.normal;
	vCS = bendCoordSystem(vCS, v.x[0]);
	Vertex out;
	out.oPos = vCS.origin;
	out.normal = vCS.t;
	out.tangent = vCS.r;
	return out;
}

CoordSystem	Branch::getCoordSystemIn(float x){
	CoordSystem cs;
	if (parent!=NULL){
		cs = parent->getCoordSystemIn(this->x);
	}
	return parent->bCs;

}

// bend coord system
CoordSystem Branch::bendCoordSystem(const CoordSystem &cs2bend, float inX, bool upFlag) const{
	CoordSystem cs(originalCS);
	v3 org;
	float rr,rs,rt,sr,ss,st,tr,ts,tt;
	rr = cs2bend.r.dot(cs.r);
	rs = cs2bend.r.dot(cs.s);
	rt = cs2bend.r.dot(cs.t);

	sr = cs2bend.s.dot(cs.r);
	ss = cs2bend.s.dot(cs.s);
	st = cs2bend.s.dot(cs.t);

	tr = cs2bend.t.dot(cs.r);
	ts = cs2bend.t.dot(cs.s);
	tt = cs2bend.t.dot(cs.t);	
	
	if (parent!=NULL){
		cs = parent->bendCoordSystem(cs, x);
		org = parent->bCs.origin;
	} else {
		org = v3(0.f, 0.f, 0.f);
	}
	
	// tranform branch coord system
	float xr,xs,ux, uxs, uxr, uxsd, uxrd, uxd, sxr, sxs, dxr, dxs;	
	ux		= c2*inX*inX + c4*inX*inX*inX*inX; // u(x)
	uxd		= 2.0*c2*inX + 3.0*c4*inX*inX*inX; // du(x)/dx
	uxrd	= Ar*uxd/L;
	uxsd	= As*uxd/L;
	uxr		= Ar*ux;
	uxs		= As*ux;
	sxr		= sqrt(1.0+uxrd*uxrd);
	sxs		= sqrt(1.0+uxsd*uxsd);
	dxr		= uxrd!=0.0?uxr/uxrd*(sxr-1.0):0.0;
	dxs		= uxsd!=0.0?uxs/uxsd*(sxs-1.0):0.0;
	
	// tangent & normal recalculation
	xr		= inX - dxr/sxr;
	xs		= inX - dxs/sxs;
	uxrd	= Ar*(2.0*c2*xr + 3.0*c4*xr*xr*xr)/L;
	uxsd	= As*(2.0*c2*xs + 3.0*c4*xs*xs*xs)/L;
	
	v3 t2	= cs.t + (cs.r*uxrd + cs.s*uxsd);
	v3 r2	= cs.r - cs.t*uxrd;
	v3 s2	= cs.s - cs.t*uxsd;
	t2.normalize();
	r2.normalize();
	s2.normalize();


	CoordSystem out(cs);
	//out.origin	= cs.origin + cs.t*cs2bend.origin.t - (cs.t * dxr - cs.r * uxr)/sxr - (cs.t * dxs - cs.s * uxs)/sxs;
	out.origin	= cs.origin + r2*cs2bend.origin.r + s2*cs2bend.origin.s + cs.t*cs2bend.origin.t - (cs.t * dxr - cs.r * uxr)/sxr - (cs.t * dxs - cs.s * uxs)/sxs;
	
	out.r = r2*rr + s2*rs + t2*rt;
	out.s = r2*sr + s2*ss + t2*st;
	out.t = r2*tr + s2*ts + t2*tt;
	out.normalize();
	
	return out;
};

void Branch::setBending(float _Ar, float _As){
	Ar = _Ar;
	As = _As;
};

void Branch::setId(int _id){
	this->id = _id;
	for (int i=0; i<vertArr.size(); i++){
		textureCoords[2*i+1] = _id;
	}
}

// add child branch
void Branch::addChildBranch(Branch * branch, float _x){
	branch->x = _x;
	branch->parent = this;
	childs.push_back(branch);
};

int Branch::childCnt(){
	int o = childs.size(); 
	int out = o;
	for (int i=0; i<o; i++){
		out+=childs[i]->childCnt();
	}
	return out;
};

Branch::~Branch(void){
	delete [] vertices;
	delete [] normals;
	delete [] indices;
};