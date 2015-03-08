#include "VBO.h"

VBODataSet::VBODataSet(const void *_data, int _dataUnitSize, int _type, string _inShaderName, bool _positionFlag)
{
	data = _data;
	dataUnitSize = _dataUnitSize;
	type = _type;
	inShaderName = _inShaderName;
	positionFlag = _positionFlag;
	componentCount = dataUnitSize/sizeof(GLfloat);
}
VBODataSet::~VBODataSet(){
	if (data!=NULL)
		delete [] data;
}


VBO::VBO(void)
{
	id = 0;
}


VBO::~VBO(void)
{	
	if (id != 0){
		glDeleteBuffers(1, &id);
		id = 0;
	}
}

bool VBO::load()
{
	return false;
}
	
bool VBO::save()
{
	return false;
}

int		VBO::addVertexAttribute(VBODataSet * dataset)
{
	int o = datasets.size();
	datasets.push_back(dataset);
	return o;
}

void	VBO::cleanUp()
{
	// delete datasets data
	for (int i=0; i<datasets.size(); i++){
		delete [] (datasets[i]->data);
		datasets[i]->data = NULL;
	}		
}

int		VBO::getReqiuredSpace()
{
	/*
	* Sum the space needed for each vertex attribute...
	*/
	int totalSize = 0;
	//datasets[i]->dataUnitSize * vertexCount
	for (int i=0; i<datasets.size(); i++){
		totalSize += datasets[i]->dataUnitSize * vertexCount;
	}
	return totalSize;
}

void	VBO::compileWithShader(Shader * shader)
{
	/*
	* Setup the shader so that it'll work with this VBO
	*/
	GLint location;
	for (int i=0; i<datasets.size(); i++){
		location = shader->getAttributeLocation(datasets[i]->inShaderName);
		shader->attributeLocations.push_back(location);
		shader->attributeNames.push_back(datasets[i]->inShaderName);
	}
}
void	VBO::compileData(GLenum usage)
{
	/*
	* Load data to GPU
	*/
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);
		// allocate enough space
		int	vboSize = getReqiuredSpace();
		glBufferData(GL_ARRAY_BUFFER, vboSize, NULL, usage);
		
		// fill set by set...
		int offset = 0;
		int size = 0;
		for (int i=0; i<datasets.size(); i++){
			size = datasets[i]->dataUnitSize * vertexCount;
			datasets[i]->offset		= offset;
			datasets[i]->size		= size;
			glBufferSubData(GL_ARRAY_BUFFER,
							offset,
							size,
							datasets[i]->data
							);
			offset += size;
			
		}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

void	VBO::setVertexCount(int count)
{
	vertexCount = count;
}
int		VBO::getVertexCount()
{
	return vertexCount;
}

void	VBO::bind(Shader * shader){
	glBindBuffer(GL_ARRAY_BUFFER, id);
		glEnableClientState(GL_VERTEX_ARRAY);
		VBODataSet * ds;
		for (int i=0; i<datasets.size(); i++){
			ds = datasets[i];
			if ( ds->positionFlag){
				glVertexPointer(3, ds->type, 0,  BUFFER_OFFSET(ds->offset));
			} else if (shader!=NULL){
				//  if attribute not found in shader
				if (shader->attributeLocations[i]<0){
					continue;
				}
				glEnableVertexAttribArray(GLuint(shader->attributeLocations[i]));
				glVertexAttribPointer(GLuint(shader->attributeLocations[i]), ds->componentCount, ds->type, GL_FALSE, 0, BUFFER_OFFSET(ds->offset));
			}
		}
}

void	VBO::unbind(Shader * shader)
{
		glDisableClientState(GL_VERTEX_ARRAY);
		VBODataSet * ds;
		for (int i=0; i<datasets.size(); i++){
			ds = datasets[i];
			if (shader!=NULL)
				if (shader->attributeLocations[i]<=0){
					
				} else {
				
					if (! ds->positionFlag){
						glDisableVertexAttribArray(shader->attributeLocations[i]);
					}
				}
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	VBO::draw(Shader *shader, GLuint primitiveType, int first){
	if (shader!=NULL)
		shader->use(true);
	bind(shader);	
	glDrawArrays(primitiveType, first, vertexCount);
	unbind(shader);
	if (shader!=NULL)
		shader->use(false);
}