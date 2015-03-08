#include "Shader.h"


Shader::Shader(void)
{
	programID	 = 0;
	vertexShID	 = 0;
	geometryShID = 0;
	fragmentShID = 0;
}


Shader::~Shader(void)
{
	destroyShaderProgram();
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



void Shader::destroyShaderProgram()
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


bool Shader::createShaderProgram(
						 const char* pVertexShaderFileName, 
						 const char* pFragmentShaderFileName)
{
	
	
	printf("\n\n\n#########################################\n");
	printf("#####        COMPILE SHADERS        #####\n#########################################\n");

	destroyShaderProgram();

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
		printShaderInfoLog(vertexShID, false);
		if (bCompiled  == false)
		{
			printf("Vertex shader compilation failed...\n\n");
			return false;
		}
		printf("Vertex shader compiled...\n");
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
		printShaderInfoLog(fragmentShID, false);
		if (bCompiled  == false)
		{
			printf("Fragment shader compilation failed...\n\n");
			return false;
		}
		printf("Fragment shader compiled...\n");
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
	printShaderInfoLog(programID, true);
	if (bLinked == false)
	{
		printf("Shader program linking failed...\n\n");
		return false;
	}
	
	printf("Shader program linked...\n");
	printf("Compilation finished...\n\n");

	return true;
}



bool Shader::createShaderProgram( 
						 const char* pVertexShaderFileName, 
						 const char* pGeometryShaderFileName, 
						 const char* pFragmentShaderFileName,
						 GLint	geometry_vertices_out,
						 GLenum geometry_primitive_in,
						 GLenum geometry_primitive_out)
{	
	printf("\n\n\n#########################################\n");
	printf("#####        COMPILE SHADERS        #####\n#########################################\n");

	destroyShaderProgram();

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
		printShaderInfoLog(vertexShID, false);
		if (bCompiled  == false)
		{
			printf("Vertex shader compilation failed...\n\n");
			return false;
		}
		printf("Vertex shader compiled...\n");
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
		printShaderInfoLog(geometryShID, false);
		if (bCompiled  == false)
		{
			printf("Geometry shader compilation failed...\n\n");
			return false;
		}
		printf("Geometry shader compiled...\n");
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
		printShaderInfoLog(fragmentShID, false);
		if (bCompiled  == false)
		{
			printf("Fragment shader compilation failed...\n\n");
			return false;
		}
		printf("Fragment shader compiled...\n");
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
	printShaderInfoLog(programID, true);
	if (bLinked == false)
	{
		printf("Shader program linking failed...\n\n");
		return false;
	}
	
	printf("Shader program linked...\n");
	printf("Compilation finished...\n\n");
	return true;
}
