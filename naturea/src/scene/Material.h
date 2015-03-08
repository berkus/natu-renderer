#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "settings.h"

class Material
{
public:
	Material(v4 _ambient,
			 v4 _diffuse,
			 v4 _specular,
			 GLfloat _shininess);
	~Material(void);

	void use();

	v4 ambient;
	v4 diffuse;
	v4 specular;
	GLfloat shininess;


};

#endif
