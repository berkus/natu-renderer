#include "ioutils.h"

const char* readFile(const char* fileName) 
{
	FILE* fin;
	char* pText = NULL;

	int count = 0;

	if (fileName != NULL) 
	{
		fin = fopen(fileName,"rt");

		if (fin != NULL) 
		{
			fseek(fin, 0, SEEK_END);
			int fileSize = ftell(fin);
			rewind(fin);

			if (fileSize >= 0) 
			{
				pText = new char[fileSize + 1];
				fileSize = fread(pText, sizeof(char), fileSize, fin);
				pText[fileSize] = '\0';
			}
			fclose(fin);
		}
		else
		{
			return fileName;
		}
	}

	return pText;
};