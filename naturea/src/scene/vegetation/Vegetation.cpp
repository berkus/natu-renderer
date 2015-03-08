#include "Vegetation.h"


Vegetation::Vegetation(TextureManager *texManager, ShaderManager *shManager):
	SceneModel(texManager, shManager)
{
}


Vegetation::~Vegetation(void)
{
}

void Vegetation::drawForLOD()
{
	glColor3f(0.0,1.0, 0.0);
	glDisable(GL_CULL_FACE);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
		glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT, 0, BUFFER_OFFSET(offsets.position));
			glDrawArrays(GL_QUADS, 0, vertices.size());
		glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnable(GL_CULL_FACE);
}
