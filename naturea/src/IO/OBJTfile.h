#ifndef _OBJTFILE_H
#define _OBJTFILE_H

#include "Vector3.h"
#include "vertex.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>

#define BRANCH_IDENTIFIER	"B"
#define LEAF_IDENTIFIER		"L"
#define LEVEL				"l"
#define ORIGIN				"o"
#define R_VECTOR			"r"
#define S_VECTOR			"s"
#define T_VECTOR			"t"
#define PARENT_ID			"p"
#define X_VALUE				"x"
#define LENGTH				"d"
#define	RADIUS1				"a"
#define	RADIUS2				"b"
#define VPOSITION			"v"
#define VNORMAL				"vn"
#define VTEXCOORD0			"vt"
#define FACE				"f"


using namespace std;

struct StEntity{
	bool	isSetId;
	bool	isSetParent;
	bool	isSetLevel;
	bool	isSetBase;
	bool	isSetOrigin;
	bool	isSetR1;
	bool	isSetR2;
	bool	isSetLength;
	bool	isSetX;
	bool	isSetVertices;
	bool	isSetIndices;
	bool	isSetTexturesFilenames;
	int		id;
	int		parentId;
	int		level;
	v3		base[3];
	v3		origin;
	float	r1;
	float	r2;
	float	length;
	float	x;
	vector<Vertex>	vVertices;
	vector<int>		vIndices;
	vector<string>	vTexturesFilenames;
};

class OBJTfile
{
public:
	OBJTfile(void);
	~OBJTfile(void);
	void loadFromFile(const char * filename);
	map<int, StEntity> branches;
	map<int, StEntity> leaves;
	string name;
	float maxLength;

private:
	//void readKeyValue(string &key, string& value)
	void readLine(string &line);
	void readNextLine(string &line);
	void readEntity(StEntity &entity);
	void parseWords(string &line,vector<string> &words, bool &multilineComment);
	void readNextWords(vector<string> &words, bool &multilineComment);
	FILE *mFile;
	

};

#endif
