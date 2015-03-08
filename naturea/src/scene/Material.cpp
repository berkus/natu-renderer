#include "Material.h"


Material::Material(	v4 _ambient,
					v4 _diffuse,
					v4 _specular,
					GLfloat _shininess):
					ambient		(_ambient	),
					diffuse		(_diffuse	),
					specular	(_specular	),
					shininess	(_shininess	)
{

}


Material::~Material(void)
{
}

void Material::use(){
	glMaterialfv(GL_FRONT_AND_BACK,	GL_AMBIENT,		ambient.data);
	glMaterialfv(GL_FRONT_AND_BACK,	GL_DIFFUSE,		diffuse.data);
	glMaterialfv(GL_FRONT_AND_BACK,	GL_SPECULAR,	specular.data);
	glMaterialf (GL_FRONT_AND_BACK,	GL_SHININESS,	shininess * 128.0f);
}