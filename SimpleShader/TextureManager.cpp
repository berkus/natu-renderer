#include "TextureManager.h"


TextureManager::TextureManager(void)
{
}


TextureManager::~TextureManager(void)
{
}

void TextureManager::bindTexture(GLuint texUnit, int texIndex){
	if (textures.size()>texIndex){
		glActiveTexture(texUnit);	
		GLuint id = textures[texIndex]->glID;
		GLuint texType = textures[texIndex]->type;
		glBindTexture(texType, id);
	} else {
		printf("Texture INDEX out of bounds [%i/%i]!!\n", textures.size(),texIndex);
	}
}
void TextureManager::unbindTexture(GLuint texUnit, int texIndex){
	if (textures.size()>texIndex){
		glActiveTexture(texUnit);	
		GLuint texType = textures[texIndex]->type;
		glBindTexture(texType, 0);
	} else {
		printf("Texture INDEX out of bounds [%i/%i]!!\n", textures.size(),texIndex);
	}
}