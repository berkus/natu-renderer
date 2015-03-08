#ifndef _TREE2_H
#define _TREE2_H

#include "scene\Vegetation\Vegetation.h"

class Tree2 :
	public Vegetation
{
public:
	Tree2(TextureManager *texManager, ShaderManager *shManager);
	Tree2(Tree2* copy);
	~Tree2(void);

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
	
	
	
};

#endif
