#include "GodRays.h"


GodRays::GodRays(ShaderManager *shManager, Light *_light)
{
	light = _light;
	lightDirDOTviewDirValue = 0.0f;
	shaderManager = shManager;
	int shId = shaderManager->loadShader("GodRays", GODRAYS_VS_FILENAME, GODRAYS_FS_FILENAME);
	shader = shaderManager->getShader(shId);
	shader->registerUniform("tintColor",	UniformType::F3,		g_tintColor.data	);
	shader->registerUniform("tintFactor",	UniformType::F1,		&g_tintFactor		);
	shader->registerUniform("expo",			UniformType::F1,		&g_god_expo			);
	shader->registerUniform("decay",		UniformType::F1,		&g_god_decay		);
	shader->registerUniform("density",		UniformType::F1,		&g_god_density		);
	shader->registerUniform("weight",		UniformType::F1,		&g_god_weight		);
	shader->registerUniform("illuminationDecay", UniformType::F1,	&g_illuminationDecay);
	shader->registerUniform("bloomDivide",	UniformType::F1,		&g_bloomDivide		);
	shader->registerUniform("sampleCount",  UniformType::I1,		&g_samples			);
	
	forRaysColorLocation = shader->getLocation("rtex");
	originalColorLocation = shader->getLocation("otex");
	//forBloomColorLocation = shader->getLocation("btex");
	lightPosLocation	 = shader->getLocation("lightPositionOnScreen");
	lightDOTviewLocation = shader->getLocation("lightDirDOTviewDir");
	sizeLoc				 = shader->getLocation("texSize");

	glGenTextures(1, &originalColor);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, originalColor);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, g_samples, GL_RGBA, g_WinWidth, g_WinHeight, false);
   
	glGenTextures(1, &forRaysColor);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, forRaysColor);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, g_samples, GL_RGBA, g_WinWidth, g_WinHeight, false);
   
	//glGenTextures(1, &forBloomColor);
	//	glBindTexture(GL_TEXTURE_2D, forRaysColor);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_WinWidth, g_WinHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
   
	   
	glGenTextures(1, &originalDepth );
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, originalDepth );
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, g_samples, GL_DEPTH_COMPONENT32, g_WinWidth, g_WinHeight, false);
   	 glGenFramebuffersEXT(1, &fboId);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D_MULTISAMPLE, originalColor, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D_MULTISAMPLE, forRaysColor, 0);
		//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, forBloomColor, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D_MULTISAMPLE, originalDepth, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	/*
	glGenTextures(1, &originalColor);
		glBindTexture(GL_TEXTURE_2D, originalColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_WinWidth, g_WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   
	glGenTextures(1, &forRaysColor);
		glBindTexture(GL_TEXTURE_2D, forRaysColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_WinWidth, g_WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   
	//glGenTextures(1, &forBloomColor);
	//	glBindTexture(GL_TEXTURE_2D, forRaysColor);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_WinWidth, g_WinHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
   
	   
	glGenTextures(1, &originalDepth );
		glBindTexture(GL_TEXTURE_2D, originalDepth );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_WinWidth, g_WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	 glGenFramebuffersEXT(1, &fboId);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, originalColor, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, forRaysColor, 0);
		//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_2D, forBloomColor, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, originalDepth, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	*/
	 assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT);
	 assert( glGetError() == GL_NO_ERROR );
	 printf("GODRAYS framebuffers initialized successfully\n");

	 // init shader...
	 
}


GodRays::~GodRays(void)
{
	glDeleteFramebuffersEXT(1, &fboId);
	glDeleteTextures(1, &originalColor);
	glDeleteTextures(1, &originalDepth);
	glDeleteTextures(1, &forRaysColor);
	glDeleteTextures(1, &forBloomColor);
}

void GodRays::begin()
{
	// start frame buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
    // prenastavit viewport
		glViewport(0,0,g_WinWidth, g_WinHeight);
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLenum buffers[2] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
		glDrawBuffersARB(2, buffers);
		//GLenum buffers[3] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT};
		//glDrawBuffersARB(3, buffers);
		// now render scene...
		glEnable(GL_MULTISAMPLE); 
}

void GodRays::end()
{
	// kreslit zas do standardniho zadniho bufferu
	glDisable(GL_MULTISAMPLE); 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer(GL_BACK);
	// viewport zpátky
	glViewport(0,0,g_WinWidth, g_WinHeight);
	/*
	v4 lposScreen = glCoordToScreenSpace(v4(	light->positionFixedToSkybox.x, 
												light->positionFixedToSkybox.y,
												light->positionFixedToSkybox.z,
												1.0)
										);
	*/
	v4 lposScreen = glCoordToScreenSpace(
		*(light->direction) * -FLT_MAX);
	
	//printf("x=%f, y=%f \n", lposScreen.x, lposScreen.y);
	
	// blend textures and draw them
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, g_WinWidth, 0, g_WinHeight, -1,1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(0.0, -.1, 0.0), 
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, originalColor);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, forRaysColor);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, forBloomColor);
	glColor4f(1.f,1.f,1.f,1.f);
	shader->use(true);
	shader->setUniform1i(originalColorLocation, 0);
 	shader->setUniform1i(forRaysColorLocation, 1);
	shader->setUniform2f(sizeLoc, g_WinWidth, g_WinHeight);
	//shader->setUniform1i(forBloomColorLocation, 1);
	
	shader->setUniform2f(lightPosLocation, lposScreen.x,lposScreen.y );
	shader->setUniform1f(lightDOTviewLocation, lightDirDOTviewDirValue);
	glBegin(GL_QUADS);
		glTexCoord2f(0.f, 0.f); glVertex2i(0,0);
		glTexCoord2f(1.f, 0.f); glVertex2i(0+g_WinWidth,0);
		glTexCoord2f(1.f, 1.f); glVertex2i(0+g_WinWidth,0+g_WinHeight);
		glTexCoord2f(0.f, 1.f); glVertex2i(0,0+g_WinHeight);
	glEnd();

	shader->use(false);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glActiveTexture(GL_TEXTURE0);


#if TEST
	// show godrays textures
	
	show_texture(originalColor, 400, 0, 200, 200);
	show_texture(originalDepth, 200, 0, 200, 200);
    show_texture(forRaysColor, 0, 0, 200, 200);
#endif	

	
	
}

void GodRays::windowSizeChanged(int width, int height)
{
	// reinit framebuffers...
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, originalColor);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, g_samples, GL_RGBA, width, height, false);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, originalDepth);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, g_samples, GL_DEPTH_COMPONENT, width, height, false);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, forRaysColor);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, g_samples, GL_RGBA, width, height, false);
	//glBindTexture(GL_TEXTURE_2D, forBloomColor);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0 );
}


