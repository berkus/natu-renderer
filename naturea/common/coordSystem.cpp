#include "coordSystem.h"

// contructor
CoordSystem::CoordSystem(v3 _origin, v3 _x, v3 _y, v3 _z):origin(_origin),r(_x),s(_y),t(_z){
	//origin.normalize();
	r.normalize();
	s.normalize();
	t.normalize();
};
//copy contructor
CoordSystem::CoordSystem(const CoordSystem& copy){
	origin = copy.origin;
	r = copy.r;
	s = copy.s;
	t = copy.t;
};

CoordSystem CoordSystem::getSystemInThisSystem(CoordSystem &cs)
{
	CoordSystem out(cs);
	out.r = getCoordsInThisSystem(cs.r);
	out.s = getCoordsInThisSystem(cs.s);
	out.t = getCoordsInThisSystem(cs.t);

	out.origin = getCoordsInThisSystem(cs.origin-origin);

	return out;
}

// get same vector but in different basis
v3 CoordSystem::getCoordsInThisSystem(v3 &v){
	v3 outVector;
	outVector.x = v.dot(r);
	outVector.y = v.dot(s);
	outVector.z = v.dot(t);
	return outVector;
}

// rotate
void CoordSystem::rotate(v3 &axis, float angle){
	r.rotate(angle, axis);
	s.rotate(angle, axis);
	t.rotate(angle, axis);
};
CoordSystem CoordSystem::getRotated(v3 &axis, float angle){
	CoordSystem out;
	out.origin = origin;
	out.r = r.getRotated(angle, axis);
	out.s = s.getRotated(angle, axis);
	out.t = t.getRotated(angle, axis);
	return out;
}


// translate
void CoordSystem::translate(v3 &trans){
	origin+=trans;
};
// normalize axis vectors
void CoordSystem::normalize(){
	r.normalize();
	s.normalize();
	t.normalize();
};

void CoordSystem::printOut(){
	printf("Origin: %f %f %f\n", origin.x, origin.y, origin.z);
	printf("R: %f %f %f\n",r.x,r.y,r.z);
	printf("S: %f %f %f\n",s.x,s.y,s.z);
	printf("T: %f %f %f\n",t.x,t.y,t.z);

};

void CoordSystem::draw(){
	// draw axis
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);

		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x+r.x, origin.y+r.y, origin.z+r.z);
	glEnd();
	glBegin(GL_LINES);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x+s.x, origin.y+s.y, origin.z+s.z);
	glEnd();
	glBegin(GL_LINES);

		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(origin.x, origin.y, origin.z);
		glVertex3f(origin.x+t.x, origin.y+t.y, origin.z+t.z);
	glEnd();
	glEnable(GL_LIGHTING);
};

// destructor
CoordSystem::~CoordSystem(void){

};

void CoordSystem::repair(){
	t = r.cross(s);
	s = t.cross(r);
}

bool CoordSystem::check(){
	float EPSILON = 0.00000001;
	float a = r.dot(s);
	float b = s.dot(t);
	float c = t.dot(r);

	if (abs(a)<EPSILON && abs(b)<EPSILON && abs(c)<EPSILON){
		return true;
	}
	return false;
}