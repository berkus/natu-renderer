#ifndef _TREE1_H
#define _TREE1_H

#include "scene\Vegetation\Vegetation.h"





class Tree1 :
	public Vegetation
{
public:
	Tree1(TextureManager *texManager, ShaderManager *shManager);
	Tree1(Tree1* copy);
	~Tree1(void);

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
