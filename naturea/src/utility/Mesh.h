#pragma once

#include "../settings.h"
#include "../../common/vertex.h"
class Mesh
{
public:
	Mesh(void);
	~Mesh(void);
	
	vector<Vertex *> vertices;

};

