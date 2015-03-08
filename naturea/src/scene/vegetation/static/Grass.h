#ifndef _GRASS_H
#define _GRASS_H

#include "scene\Vegetation\Vegetation.h"
#include "../../../utility/VBO.h"

class Grass :
	public Vegetation
{
public:
	Grass(TextureManager *texManager, ShaderManager *shManager);
	Grass(Grass* copy);
	~Grass(void);

	Vegetation* getCopy();

	void draw();

	void init();

	void update(double time);
	
	void bakeToVBO();

	v3	 transformTexCoords(v3 &origTexCoords);
	
	void fixTexType();

	int			texPart_x;
	int			texPart_y;
	int			textureId;
	int			shaderId;
	Shader*		shader;
	int			textureWaveId;
	
	VBO	*		vbo;
};

#endif
