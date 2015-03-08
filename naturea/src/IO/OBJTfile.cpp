#include "OBJTfile.h"
bool getFromString1i(string &s, int *i){
	int count = sscanf(s.c_str(), "%i", i);
	if (count!=1){
		return false;
	}
	return true;
}

bool getFromString1f(string &s, float *f){
	int count = sscanf(s.c_str(), "%f", f);
	if (count!=1){
		return false;
	}
	return true;
}

OBJTfile::OBJTfile(void)
{
	maxLength = 0.f;
}


OBJTfile::~OBJTfile(void)
{
}
void OBJTfile::readNextWords(vector<string> &words, bool &multilineComment){
	string line;
	readNextLine(line);
	parseWords(line, words, multilineComment);
	while (!feof(mFile) && words.size()<1){
		readNextLine(line);
		parseWords(line, words, multilineComment);	
	}
}
	


void OBJTfile::loadFromFile(const char * filename){
	
	mFile = fopen(filename, "r+");
	if (mFile==NULL){
		// error
		printf("Error while reading file: '%s'\n", filename);
		return;
	}
	// OK
	string line;
	vector<string> words;
	bool multilineCommentFlag = false;
	// read settings
	readNextWords(words, multilineCommentFlag);
	// name
	if (words[0].compare("name")==0){
		name = words[2];
		readNextWords(words, multilineCommentFlag);
	}
	// read entities
	int bc = 0, lc = 0;
	int hb=0, hl=0;
	while (!feof(mFile)){
		if (words[0]==BRANCH_IDENTIFIER){
			bc++;
			StEntity branch;
			getFromString1i(words[1], &branch.id);
			readEntity(branch);
			branches[branch.id]=branch;
			hb = max(hb, branch.id);
		}
		if (words[0]==LEAF_IDENTIFIER){
			lc++;
			StEntity leaf;
			getFromString1i(words[1], &leaf.id);
			readEntity(leaf);
			leaves[leaf.id]=leaf;
			hl = max(hl, leaf.id);
		}
		readNextWords(words, multilineCommentFlag);
	}
	printf("Success reading file: '%s' [bc = %i, lc = %i, hb=%i, hl=%i]\n", filename, bc, lc, hb, hl);
		
	fclose(mFile);
}
void OBJTfile::readEntity(StEntity &entity){
	string line;
	vector<string> words;
	bool multilineCommentFlag = false;
	// read entities
	readNextWords(words, multilineCommentFlag);
	
	int posCnt = 0;
	int norCnt = 0;
	int texCnt = 0;
	int indCnt = 0;


	entity.isSetId			= true;
	entity.isSetParent		= false;
	entity.isSetLevel		= false;
	entity.isSetBase		= false;
	entity.isSetOrigin		= false;
	entity.isSetR1			= false;
	entity.isSetR2			= false;
	entity.isSetLength		= false;
	entity.isSetX			= false;
	entity.isSetVertices	= false;
	entity.isSetIndices		= false;
	entity.isSetTexturesFilenames = false;
	bool rSet = false;
	bool sSet = false;
	bool tSet = false;
	bool posSet = false;
	bool norSet = false;
	bool texSet = false;
	while (!feof(mFile) && !(words[0]=="}")){
		if (words[0]==LEVEL){
			getFromString1i(words[1], &(entity.level));
			entity.isSetLevel=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==ORIGIN){
			getFromString1f(words[1], &(entity.origin.x));
			getFromString1f(words[2], &(entity.origin.y));
			getFromString1f(words[3], &(entity.origin.z));
			entity.isSetOrigin=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==R_VECTOR){
			getFromString1f(words[1], &(entity.base[0].x));
			getFromString1f(words[2], &(entity.base[0].y));
			getFromString1f(words[3], &(entity.base[0].z));
			rSet=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==S_VECTOR){
			getFromString1f(words[1], &(entity.base[1].x));
			getFromString1f(words[2], &(entity.base[1].y));
			getFromString1f(words[3], &(entity.base[1].z));
			sSet=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==T_VECTOR){
			getFromString1f(words[1], &(entity.base[2].x));
			getFromString1f(words[2], &(entity.base[2].y));
			getFromString1f(words[3], &(entity.base[2].z));
			tSet=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==PARENT_ID){
			getFromString1i(words[1], &(entity.parentId));
			entity.isSetParent=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==X_VALUE){
			getFromString1f(words[1], &(entity.x));
			entity.isSetX=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==LENGTH){
			getFromString1f(words[1], &(entity.length));
			if (entity.length>maxLength){
				maxLength = entity.length;
			}
			entity.isSetLength=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==RADIUS1){
			getFromString1f(words[1], &(entity.r1));
			entity.isSetR1=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==RADIUS2){
			getFromString1f(words[1], &(entity.r2));
			entity.isSetR2=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==VPOSITION){
			getFromString1f(words[1], &(entity.vVertices[posCnt].oPos.x));
			getFromString1f(words[1], &(entity.vVertices[posCnt].oPos.y));
			getFromString1f(words[1], &(entity.vVertices[posCnt].oPos.z));
			posCnt++;
			
			posSet=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==VNORMAL){
			getFromString1f(words[1], &(entity.vVertices[norCnt].normal.x));
			getFromString1f(words[1], &(entity.vVertices[norCnt].normal.y));
			getFromString1f(words[1], &(entity.vVertices[norCnt].normal.z));
			norCnt++;
			
			norSet=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==VTEXCOORD0){
			getFromString1f(words[1], &(entity.vVertices[texCnt].textureCoords.x));
			getFromString1f(words[1], &(entity.vVertices[texCnt].textureCoords.y));
			texCnt++;
			
			texSet=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		if (words[0]==FACE){
			getFromString1i(words[1], &(entity.vIndices[indCnt]));
			getFromString1i(words[1], &(entity.vIndices[indCnt]));
			getFromString1i(words[1], &(entity.vIndices[indCnt]));
			indCnt++;
			
			entity.isSetIndices=true;			
			readNextWords(words, multilineCommentFlag);
			continue;
		}
		readNextWords(words, multilineCommentFlag);		
	}
	entity.isSetBase		= (rSet && sSet && tSet);
	entity.isSetVertices	= (posSet && norSet && texSet);
	// end of entity

}


void OBJTfile::parseWords(string &line,vector<string> &words, bool &multilineComment){
	words.clear();
	int size = line.size();
	int i;
	bool insidelongliteral = false;
	string current = "";
	bool inComment = false;
	bool wasComment = false;
	bool wasSlash = false;
	bool wasStar  = false;
	unsigned char ch;
	for (i=0; i<size; i++){
		ch = line[i];
		bool isSeparator			= (ch == ' ') | (ch == 9);
		bool isWordSeparator		= (ch == '=') | (ch == '{') | (ch == '}') | (ch == '(') | (ch == ')') | (ch == ';') | (ch == ',');
		bool isMultiWordSeparator	= (ch == '"') | (ch == '\'');
		bool isComment				= (ch == '#');
		bool isSlash				= (ch == '/');
		bool isStar					= (ch == '*');
		bool isCommentBegin			= wasSlash && isStar;
		bool isCommentEnd			= wasStar && isSlash;
		// String ends immediately with a comment marker.
		if (wasSlash && isSlash){
			// line comment
			break;
		}
		if (isCommentEnd){
			wasSlash = false;
			wasStar  = false;
			wasComment = true;
			multilineComment = false;
		}
		if (isCommentBegin){
			multilineComment = true;
		}
		if (isComment) { break; }
		if (!multilineComment){
			if (isSlash){
				wasSlash = true;
				continue;
			}
			if (wasSlash && !wasComment){
				current.push_back('/');
			}
			if (wasStar){
				current.push_back('*');
			}

			if (isMultiWordSeparator)
			{
				if (current.size() > 0)
				{
					words.push_back(current);
					current = "";
				}
				insidelongliteral = !insidelongliteral;
				isSeparator = true;
				isWordSeparator = false;
			}
			if (!insidelongliteral)
			{
				if ((isSeparator) || (isWordSeparator))
				{
					// If the char is a separator, the current literal ends here.
					if (current.size() > 0)
					{
						words.push_back(current);
						current = "";
					}

					// Now check if the separator is an individual literal.
					if (isWordSeparator)
					{
						string str = "";
						str.push_back(ch);
						words.push_back(str);
					}
				}
				else
				{
					current.push_back(ch);
				}
			}
			else
			{
				// Inside long literal
				if (!isMultiWordSeparator)
				{
					current.push_back(ch);
				}
			}
			wasComment = false;
		}
		wasSlash					= (ch == '/');
		wasStar						= (ch == '*');
	} // for each char
	if (current.size()>0){
		words.push_back(current);
	}
}

void OBJTfile::readNextLine(string &line){
	readLine(line);
	if ((line.size()>0) && (line[0]=='#')){
		line = "";
	}
	while (!feof(mFile) && line.size()<=0){
		readLine(line);
		if (line.size()>0 && (line[0]=='#')){
			line = "";
		}
	}

}

void OBJTfile::readLine(string &line){
	unsigned char ch = 0;
	line = "";

	if (feof(mFile))
	{
		return;
	}

	while (true)
	{
		ch = getc(mFile);

		// End of file reached.
		if (feof(mFile))
		{
			break;
		}
		// End of line reached.
		if ((ch == 13) || (ch == 10) || (ch == 0))
		{
			break;
		}
		else
		{
			if ((line.size()==0) && (ch==' ') || (ch=='\t') || (ch=='\n')){
				//skip whitespaces
				continue;
			} 
			// This character is part of the text line, add it to the string.
			line.push_back(ch);
		}
	}

	// If the last character was a 13, then a 10 will follow.
	if (ch == 13)
	{
		ch = getc(mFile);
	}
}


