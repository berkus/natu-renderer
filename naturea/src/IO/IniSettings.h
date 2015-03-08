#ifndef _INISETTINGS_H
#define _INISETTINGS_H

#include "..\..\src\settings.h"



class IniSettings
{
public:
	IniSettings(void);
	~IniSettings(void);

	map<string, void*> data;



};

#endif

