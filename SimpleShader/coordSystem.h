#ifndef __COORDSYSTEM_H__
#define __COORDSYSTEM_H__

#include "../_utils/GLUT/glut.h"
#include "../_utils/Vector3.h"
#include <cstdio>

//typedef Vector3 v3;

class CoordSystem
{
	public:
		// contructor
		CoordSystem(v3 _origin, v3 _x, v3 _y, v3 _z);

		// single vector constructor
		CoordSystem(const v3 &vector);


		// copy contructor
		CoordSystem(const CoordSystem& copy);

		// default
		CoordSystem(){
			
		};
		
		// destructor
		~CoordSystem(void);

		bool check();

		void repair();

		// rotate
		void rotate(v3 &axis, float angle);

		CoordSystem getRotated(v3 &axis, float angle);


		// translate
		void translate(v3 &trans);

		// draw
		void draw();

		void normalize();

		void printOut();
		
		CoordSystem getSystemInThisSystem(CoordSystem &cs);

		v3 getCoordsInThisSystem(v3 &v);
		// origin
		v3 origin;
		// x,y,z axis
		v3 r,s,t;
};
#endif