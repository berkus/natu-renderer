#ifndef _VBOFILE_H
#define _VBOFILE_H




#include "iofile.h"
class VBOfile :
	public IOfile
{
public:
	VBOfile(void);
	~VBOfile(void);

	virtual void loadFromFile(string filename, VertexBuffer &mVertexBuffer, bool &errorFlag);
	virtual void saveToFile(string filename, VertexBuffer &mVertexBuffer, bool &errorFlag);

};

#endif
