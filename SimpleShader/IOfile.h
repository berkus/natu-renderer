#ifndef _IOFILE_H
#define _IOFILE_H

#include <string>
#include "VertexBuffer.h"

using namespace std;



class IOfile
{
public:
	IOfile(void);
	~IOfile(void);
	virtual void loadFromFile(string filename, VertexBuffer &mVertexBuffer, bool &errorFlag)=0;
	virtual void saveToFile(string filename, VertexBuffer &mVertexBuffer, bool &errorFlag)=0;
};

#endif
