#include "TestObject.h"
#include "png.h"

TestOBJ::TestOBJ(int shader){
	shaderProgramID = shader;
	/*PNG png(4,4);
	float k = 0;
	for (int i=0; i<4; i++){
		for (int j=0; j<4; j++){
			png.setPixel(i,j,k);
			k+=0.6f;
		}
	}
	
*/
	data = new float[4*5*3];
	float k = 0.1;
	for (int i=0; i<4*5*3; i=i+3){
		
		data[i + 0]= k*.1f;//(float)i/(float)(16*4);
		data[i + 1]= 0.0f;
		data[i + 2]= 0.0f;
		//data[i + 3]= 0.0f;
		printf("DATA[%i] = %f\n", i,   data[i + 0]);
		k+=1;
		
		//printf("DATA[%i] = %f\n", i+1, i*0.0);
		//printf("DATA[%i] = %f\n", i+2, i*0.0);
		//printf("DATA[%i] = %f\n", i+3, i*0.0);
	}

};

TestOBJ::~TestOBJ(){
	delete [] data;
};

void TestOBJ::draw(){
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
	if (g_bShadersSupported && g_bUseShaders)
	{
		glUseProgram(shaderProgramID);
		glUniform1i(glGetUniformLocation(shaderProgramID, "my_texture"),0);
	}
	// draw
	float tSizeX = 4;
	float tSizeY = 5;
	float tStepX = 1/tSizeX;
	float tStepY = 1/tSizeY;
	float x,y,v;
	glBegin(GL_LINE_STRIP);
		for (int i=0; i<tSizeX*tSizeY; i++){
			x = float(i%(int)tSizeX);///(tSizeX-1);
			y = i/(int)tSizeX;///(tSizeY-1);
			//v = data[int((y*(tSizeY-1))*tSizeX+x*(tSizeX-1))*4];
			v = data[int(y*tSizeX+x)*3];
			
			//printf("[X,Y]=[%f, %f] = %f\n",x,y, v );
			glMultiTexCoord2f(GL_TEXTURE0, x+0.5,y+0.5);
			glVertex3f(0.f, i*0.1, 0.f);
		}
	glEnd();
	//
	if (g_bShadersSupported && g_bUseShaders)
	{
		glUseProgram(NULL);
	}
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
};

void TestOBJ::init(){
	int width = 4, height = 5;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textureID);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP); 
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glEnable(GL_TEXTURE_RECTANGLE_ARB);
};