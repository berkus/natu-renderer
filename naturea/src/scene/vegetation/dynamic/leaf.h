#ifndef __LEAF_H__
#define __LEAF_H__


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

class Leaf: public Branch
{
	Leaf(){
		isLeaf = true;
	}
}
#endif 