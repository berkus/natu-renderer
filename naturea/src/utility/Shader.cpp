#include "../utility/Shader.h"

Shader::Shader(string shname)
{
	name		 = shname;
	programID	 = 0;
	vertexShID	 = 0;
	geometryShID = 0;
	fragmentShID = 0;
	isRunning = false;
}

Shader::~Shader(void)
{
	destroy();
	printf("Shader %s destroyed.\n", name.c_str());
}

bool Shader::loadShader(string vs_filename, string fs_filename)
{
	const char* pVertexShaderFileName    = vs_filename.c_str();
	const char* pFragmentShaderFileName	 = fs_filename.c_str();
	printf("# COMPILE %s SHADERS\n", name.c_str());

	destroy();

	GLint bCompiled			 = false;
	bool  bAddVertexShader	 = (pVertexShaderFileName != NULL);
	bool  bAddFragmentShader = (pFragmentShaderFileName != NULL);

	if (bAddVertexShader)
	{
 		// Create the vertex shader_________________________________________________
		vertexShID = glCreateShader(GL_VERTEX_SHADER);
			
		const char* pVSsource = readFile(pVertexShaderFileName);
		glShaderSource (vertexShID, 1, (const char**) &pVSsource, NULL );
		glCompileShader(vertexShID);
		delete [] pVSsource;

		glGetShaderiv(vertexShID, GL_COMPILE_STATUS, &bCompiled);
		if (bCompiled  == false)
		{
			printShaderInfoLog(vertexShID, false);
			printf(" - VS, ");
			pauseAndExit();
			return false;
		}
		printf(" + VS, ");
	}

	// Create the fragment shader_______________________________________________
	if (bAddFragmentShader)
	{
	    fragmentShID = glCreateShader(GL_FRAGMENT_SHADER);
			
		const char* pFSsource = readFile(pFragmentShaderFileName);
		glShaderSource (fragmentShID, 1, (const char**) &pFSsource, NULL );
		glCompileShader(fragmentShID);
		delete [] pFSsource;

		glGetShaderiv(fragmentShID, GL_COMPILE_STATUS, &bCompiled);
		
		if (bCompiled  == false)
		{
			printShaderInfoLog(fragmentShID, false);
			printf(" - FS, ");
			pauseAndExit();
			return false;
		}
		printf(" + FS, ");
	}

    // Create a program object and attach the two compiled shaders______________
	GLint bLinked	= false;

	programID = glCreateProgram();
	if (bAddVertexShader) 
	{
		glAttachShader(programID, vertexShID  );
	}
	if (bAddFragmentShader) 
	{
		glAttachShader(programID, fragmentShID);
	}

	// Link the program object and print out the info log_______________________
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &bLinked);
	
	if (bLinked == false)
	{
		printShaderInfoLog(programID, true);
		printf("- Shader program linking failed...\n\n");
		pauseAndExit();
		return false;
	}
	timeLocation = glGetUniformLocation(programID, "time");
	//printf(" (time location: %i)\n", timeLocation);
	printf("+ SUCCESS [%s] (shader ID= %i)\n",this->name.c_str(), this->programID);

	return true;
}

bool Shader::loadShader(
		string vs_filename,
		string fs_filename,
		string gs_filename,
		GLint  geometry_vertices_out,
		GLenum geometry_primitive_in,
		GLenum geometry_primitive_out
		)
{
	const char* pVertexShaderFileName    = vs_filename.c_str();
	const char* pGeometryShaderFileName  = gs_filename.c_str();
	const char* pFragmentShaderFileName	 = fs_filename.c_str();
						
	printf("# COMPILE %s SHADERS\n", name.c_str());

	destroy();

	GLint bCompiled			 = false;
	bool  bAddVertexShader	 = (pVertexShaderFileName != NULL);
	bool  bAddGeometryShader = (pGeometryShaderFileName != NULL);
	bool  bAddFragmentShader = (pFragmentShaderFileName != NULL);

	if (bAddVertexShader)
	{
 		// Create the vertex shader_________________________________________________
		vertexShID = glCreateShader(GL_VERTEX_SHADER);
			
		const char* pVSsource = readFile(pVertexShaderFileName);
		glShaderSource (vertexShID, 1, (const char**) &pVSsource, NULL );
		glCompileShader(vertexShID);
		delete [] pVSsource;

		glGetShaderiv(vertexShID, GL_COMPILE_STATUS, &bCompiled);
		if (bCompiled  == false)
		{
			printShaderInfoLog(vertexShID, false);		
			printf(" - VS, ");
			pauseAndExit();
			return false;
		}
		printf(" + VS, ");
	}

	if (bAddGeometryShader)
	{
 		// Create the vertex shader_________________________________________________
		geometryShID = glCreateShader(GL_GEOMETRY_SHADER_EXT);
			
		const char* pGSsource = readFile(pGeometryShaderFileName);
		glShaderSource (geometryShID, 1, (const char**) &pGSsource, NULL );
		glCompileShader(geometryShID);
		delete [] pGSsource;

		glGetShaderiv(geometryShID, GL_COMPILE_STATUS, &bCompiled);
		if (bCompiled  == false)
		{
			printShaderInfoLog(geometryShID, false);
			printf(" - GS, ");
			pauseAndExit();
			return false;
		}
		printf(" + GS, ");
	}

	// Create the fragment shader_______________________________________________
	if (bAddFragmentShader)
	{
	    fragmentShID = glCreateShader(GL_FRAGMENT_SHADER);
			
		const char* pFSsource = readFile(pFragmentShaderFileName);
		glShaderSource (fragmentShID, 1, (const char**) &pFSsource, NULL );
		glCompileShader(fragmentShID);
		delete [] pFSsource;

		glGetShaderiv(fragmentShID, GL_COMPILE_STATUS, &bCompiled);
		if (bCompiled  == false)
		{
			printShaderInfoLog(fragmentShID, false);
			printf(" - FS, ");
			pauseAndExit();
			return false;
		}
		printf(" + FS, ");
	}

    // Create a program object and attach the two compiled shaders______________
	GLint bLinked	= false;

	programID = glCreateProgram();
	
	if (bAddVertexShader) 
	{
		glAttachShader(programID, vertexShID  );
	}
	if (bAddGeometryShader) 
	{
		glAttachShader(programID, geometryShID  );
	}
	if (bAddFragmentShader) 
	{
		glAttachShader(programID, fragmentShID);
	}

	// Set shader program parameters for geometry shader
	if (bAddGeometryShader) 
	{
		glProgramParameteriEXT(programID, GL_GEOMETRY_VERTICES_OUT_EXT, geometry_vertices_out);
		glProgramParameteriEXT(programID,   GL_GEOMETRY_INPUT_TYPE_EXT, geometry_primitive_in);
		glProgramParameteriEXT(programID,  GL_GEOMETRY_OUTPUT_TYPE_EXT, geometry_primitive_out);
	}

	// Link the program object and print out the info log_______________________
	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &bLinked);
	if (bLinked == false)
	{
		printShaderInfoLog(programID, true);
		printf(" - Shader program linking failed...\n\n");
		pauseAndExit();
		return false;
	}
	timeLocation = glGetUniformLocation(programID, "time");
	//printf(" (time location: %i)\n", timeLocation);
	printf("+ SUCCESS (shader ID= %i)\n", this->programID);
	return true;


}

void Shader::use(bool isTurnedOn)
{
	if (isTurnedOn && !isRunning){
		isRunning = true;
		glUseProgram(programID);
		// attach uniforms
		int i, size = uniforms.size();
		for (i=0; i<size; i++){
			if (uniforms[i]->location>=0){
				setUniform(uniforms[i]);
			} // else invalid location
		}
	} 
	if (!isTurnedOn && isRunning){
		isRunning = false;
		glUseProgram(0);
	}
}

void Shader::setTime(float in_time)
{
	//printf("set time: %f, loc: %i\n", in_time, timeLocation);
	glUniform1f(timeLocation, in_time);
}

bool Shader::linkTexture(Texture *texture)
{
	GLint loc = registerUniform(texture->inShaderName, UniformType::I1, &(texture->textureUnitNumber));
	if (loc < 0){
		return false;
	}
	return true;
}

void Shader::printShaderInfoLog(GLuint objectId, bool bShaderProgram)
{
	int		infoLogLength = 0;
	int		writtenChars  = 0;
	GLchar* pBuffer		  = NULL;

	if (bShaderProgram)
	{
		glGetProgramiv(objectId, GL_INFO_LOG_LENGTH, &infoLogLength);
	}
	else
	{
		glGetShaderiv(objectId, GL_INFO_LOG_LENGTH, &infoLogLength);
	}

//	assert(glGetError() == GL_NO_ERROR);

	if (infoLogLength > 0)
	{
		pBuffer = new GLchar[infoLogLength];

		if (bShaderProgram)
		{
			glGetProgramInfoLog(objectId, infoLogLength, &writtenChars, pBuffer);
		}
		else
		{
			glGetShaderInfoLog(objectId, infoLogLength, &writtenChars, pBuffer);
		}

		printf("%s\n-----------------------------------------\n", pBuffer);

		delete [] pBuffer;
	}
}

void Shader::destroy()
{
	if (programID != 0)			
	{
		if (vertexShID != 0)
		{
			glDeleteShader(vertexShID);
			vertexShID	= 0;
		}
		if (geometryShID != 0)
		{
			glDeleteShader(geometryShID);
			geometryShID	= 0;
		}
		if (fragmentShID != 0)
		{
			glDeleteShader(fragmentShID);
			fragmentShID	= 0;
		}

		glDeleteProgram(programID);
		programID = 0;
	}
}

int		Shader::getGLLocation(string var_name)
{
	int out = glGetUniformLocation(programID, var_name.c_str());
	/*
	if (out<0){
		printf("shader [%s]: uniform '%s' not found\n", this->name.c_str(), var_name.c_str() );
	}
	*/
	return out;
}

int		Shader::getLocation(string var_name)
{
	int loc = glGetUniformLocation(programID, var_name.c_str());
	//if (loc>=0){
		// valid uniform
		int out = locations.size();
		locations.push_back(loc);
		return out;
	//}
	//return -1;
}
int		Shader::getAttributeLocation(string var_name)
{
	return glGetAttribLocation(programID, var_name.c_str());
}

void	Shader::setUniform1i(int locIndex, int value)
{
	glUniform1i(locations[locIndex], value);
}
void    Shader::setUniform2i(int locIndex, int value1, int value2)
{
	glUniform2i(locations[locIndex], value1, value2);
}
void	Shader::setUniform3i(int locIndex, int value1, int value2, int value3)
{
	glUniform3i(locations[locIndex], value1, value2, value3);
}
void    Shader::setUniform4i(int locIndex, int value1, int value2, int value3, int value4)
{
	glUniform4i(locations[locIndex], value1, value2, value3, value4);
}
void	Shader::setUniform1f(int locIndex, float value)
{
	glUniform1f(locations[locIndex], value);
}
void    Shader::setUniform2f(int locIndex, float value1, float value2)
{
	glUniform2f(locations[locIndex], value1, value2);
}
void	Shader::setUniform3f(int locIndex, float value1, float value2, float value3)
{
	glUniform3f(locations[locIndex], value1, value2, value3);
}
void    Shader::setUniform4f(int locIndex, float value1, float value2, float value3, float value4)
{
	glUniform4f(locations[locIndex], value1, value2, value3, value4);
}
void	Shader::setUniformfp(int locIndex, float* values, int count)
{
	glUniform4fv(locations[locIndex], count, values);
}
void    Shader::setUniform2v(int locIndex, v2 value)
{
	glUniform2f(locations[locIndex],value.x, value.y);
}
void	Shader::setUniform3v(int locIndex, v3 value)
{
	glUniform3f(locations[locIndex], value.x, value.y, value.z);
}
void    Shader::setUniform4v(int locIndex, v4 value)
{
	glUniform4f(locations[locIndex], value.x, value.y, value.z, value.w);
}
void	Shader::setUniformMatrix(int locIndex, m4 & matrix)
{
	glUniformMatrix4fv(locations[locIndex],1,false, matrix.m);
}
void	Shader::setBoolean(int locIndex, bool value)
{
	setUniform1i(locIndex, value?1:0);
}
void	Shader::setTexture(int location, int texUnitNumber)
{
	if (location>-1){
		glUniform1i(location, texUnitNumber);
	}
}

void	Shader::setUniform(Uniform * uni){
	switch (uni->type){
		case UniformType::F1:
			//if (uni->name.compare( "time_offset" ) ==0){
			//	printf("SHADER [%s], time offset: %f\n", this->name.c_str(),*((float*)uni->data));
			//}

			glUniform1f(uni->location, (*(GLfloat*)(uni->data)));
		break;
		case UniformType::F2:
			glUniform2f(uni->location,
				((GLfloat*)(uni->data))[0],
				((GLfloat*)(uni->data))[1]);
		break;
		case UniformType::F3:
			glUniform3f(uni->location,
				((GLfloat*)(uni->data))[0],
				((GLfloat*)(uni->data))[1],
				((GLfloat*)(uni->data))[2]);
		break;
		case UniformType::F4:
			glUniform4f(uni->location,
				((GLfloat*)(uni->data))[0],
				((GLfloat*)(uni->data))[1],
				((GLfloat*)(uni->data))[2],
				((GLfloat*)(uni->data))[3]);
		break;
		case UniformType::I1:
			glUniform1iv(uni->location, 1, (GLint*)(uni->data));
		break;
		case UniformType::I2:
			glUniform2iv(uni->location, 2, (GLint*)(uni->data));
		break;
		case UniformType::I3:
			glUniform3iv(uni->location, 3, (GLint*)(uni->data));
		break;
		case UniformType::I4:
			glUniform4iv(uni->location, 4, (GLint*)(uni->data));
		break;
		case UniformType::M3:
			glUniformMatrix3fv(uni->location, 1, false,  (GLfloat*)(uni->data));
			break;
		case UniformType::M4:
			glUniformMatrix4fv(uni->location, 1, false,  (GLfloat*)(uni->data));
			break;
		case UniformType::B1:
			bool value = *(bool*)(uni->data);
			glUniform1i(uni->location, value?1:0);
			break;
	
	}
}

int		Shader::registerUniform(string inShaderName, UniformType type, void * data){
	// check if it already exists in uniforms
	int index = -1;
	for (int i=0; i<uniforms.size(); i++){
		if (uniforms[i]->name.compare(inShaderName)==0){
			index = i;
			break;
		}
	}
	if (index>=0){
		uniforms[index]->data = data;
		uniforms[index]->type = type;
		return index;
	}
	GLint loc = getGLLocation(inShaderName);
	if (loc<0){ return loc;}
	Uniform * u = new Uniform();
	u->data = data;
	u->location = loc;
	u->type = type;
	u->name = inShaderName;
	int o = uniforms.size();
	uniforms.push_back(u);
	return o;
}

Uniform * Shader::getUniform(int index){
	return uniforms[index];
}

