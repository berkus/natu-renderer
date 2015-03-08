#ifndef __BRANCH_H__
#define __BRANCH_H__


#include "../_utils/GLEE/glee.h" 
#include "../_utils/GLUT/glut.h"
#include "coordSystem.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include "vertex.h"
#include "../_utils/Matrix3x3.h"

using namespace std;

class Branch
{
	public:
		// contructor
		Branch(Branch * _parent, float _xval, CoordSystem & _cs, float length, float radiusAtBegin, float radiusAtEnd, int _divT, int _divR, float _c2, float _c4);
		// destructor
		~Branch(void);
		// init/reset
		void init();

		// draw
		void draw();
		// update
		void update();
		
		// set bending
		void setBending(float Ar, float As);
		
		// bend coord system
		CoordSystem bendCoordSystem(const CoordSystem &cs2bend, float x, bool upFlag=true) const;

		// add child branch
		void addChildBranch(Branch * branch, float _x);
		
		// get num of all childs
		int childCnt();
		Branch		*	parent;
		vector<Branch*> childs;
		float			x;
		CoordSystem		bCs;

		
		CoordSystem		originalCS;

		float		*	vertices;
		float		*	normals;
		int			*	indices;
		float		*	textureCoords;
		vector<Vertex>	vertArr;		
		float			L;
		int				indicesCount;
		float			Ar;
		float			As;
		void			setId(int id);
		int				id;
		float			r1, r2;
		float			c2, c4;
private:
	Vertex			bendVertex(Vertex &v);
	CoordSystem		getCoordSystemIn(float x);
	int				divT;
	int				divR;
};

#endif
