#include "WaterSurface.h"


WaterSurface::WaterSurface(TextureManager *texManager, ShaderManager *shManager):
	SceneModel(texManager, shManager)
{

}

WaterSurface::~WaterSurface(void)
{
	// destroy buffers
	glDeleteFramebuffersEXT(1, &fb_refl_ID);
	glDeleteTextures(1, &cb_refl_ID);
	glDeleteTextures(1, &db_refl_ID);
	glDeleteFramebuffersEXT(1, &fb_refr_ID);
	glDeleteTextures(1, &cb_refr_ID);
	glDeleteTextures(1, &db_refr_ID);
}

void WaterSurface::drawForLOD()
{
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	glScalef(500.f, 1.f, 500.f);
	glDisable(GL_CULL_FACE);
		glColor3f(0.f, 0.f, 1.f);
		drawPlane();
	glEnable(GL_CULL_FACE);
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void WaterSurface::draw()
{
	textureManager->bindTexture(dudv_ID, GL_TEXTURE3);
	shader->use(true);
	shader->setTime(g_time);
	shader->setUniform1i(water_reflection_loc, 0);// texture unit 0
	shader->setUniform1i(water_refraction_loc, 1);// texture unit 1
	shader->setUniform1i(water_depth_loc, 2);// texture unit 2
	//shader->setUniform1i(water_dudv_loc, 3);

	glColor3f(0.f, 0.1f, 1.f);
	// use texture
	glPushMatrix();
	glScalef(500.f, 1.f, 500.f);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);		
		glBindTexture(GL_TEXTURE_2D, cb_refl_ID);
		glActiveTexture(GL_TEXTURE1);		
		glBindTexture(GL_TEXTURE_2D, cb_refr_ID);
		glActiveTexture(GL_TEXTURE2);		
		glBindTexture(GL_TEXTURE_2D, db_refr_ID);
			//glClientActiveTexture(GL_TEXTURE0);
			drawPlane();
		glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glPopMatrix();
	shader->use(false);
	textureManager->unbindTexture(dudv_ID);
	
}


void WaterSurface::init()
{
	// init textures & buffers
	//textureManager->loadTexture(...);
	dudv_ID = textureManager->loadTexture(WATER_DUDV_MAP, "water_dudvmap", 3, false, GL_REPEAT, GL_LINEAR);

	glGenTextures(1, &cb_refl_ID);
		glBindTexture(GL_TEXTURE_2D, cb_refl_ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_WinWidth, g_WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   
	   
	glGenTextures(1, &db_refl_ID );
		glBindTexture(GL_TEXTURE_2D, db_refl_ID );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_WinWidth, g_WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	 glGenFramebuffersEXT(1, &fb_refl_ID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_refl_ID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cb_refl_ID, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, db_refl_ID, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	glGenTextures(1, &cb_refr_ID);
		glBindTexture(GL_TEXTURE_2D, cb_refr_ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_WinWidth, g_WinHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
   
	   
	glGenTextures(1, &db_refr_ID );
		glBindTexture(GL_TEXTURE_2D, db_refr_ID );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_WinWidth, g_WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	 glGenFramebuffersEXT(1, &fb_refr_ID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_refr_ID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cb_refr_ID, 0);
		//glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_2D, color_tex1_id, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, db_refr_ID, 0);
	 
	 assert(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)==GL_FRAMEBUFFER_COMPLETE_EXT);
	 assert( glGetError() == GL_NO_ERROR );
	 // init shaders...
	 int shaderID = shaderManager->loadShader("Water", WATER_VS_FILENAME, WATER_FS_FILENAME);
	 shader = shaderManager->getShader(shaderID);
 	 water_reflection_loc = shader->getLocation("water_reflection");
	 water_refraction_loc = shader->getLocation("water_refraction");
	 water_depth_loc = shader->getLocation("water_depthmap");
	 shader->linkTexture(textureManager->getTexture(dudv_ID));
}

void WaterSurface::update(double time)
{

}

void WaterSurface::windowSizeChanged(int width, int height)
{
	// reinit framebuffers...
		glBindTexture(GL_TEXTURE_2D, cb_refl_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, db_refl_ID );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, cb_refr_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, db_refr_ID );
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, 0 );
}

void WaterSurface::beginReflection()
{
	glDisable(GL_CULL_FACE);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_refl_ID);
    // prenastavit viewport
		glViewport(0,0,g_WinWidth, g_WinHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glDrawBuffersARB(2, buffers);
		// nastavit transformaci...
		glPushMatrix();
		if (activeCamera->getPosition().y>=WATER_HEIGHT){
			glTranslatef(0.0f, WATER_HEIGHT, 0.0f);
			glScalef(1.f, -1.f, 1.f); // flip Y
			//glCullFace(GL_FRONT);
		} else {
			glScalef(1.f, 2.0f, 1.f);
			glTranslatef(0.0f, WATER_HEIGHT-1.f, 0.0f);
		}
		
		//double plane[4] = {0.0, 1.0, 0.0, -0.5};  // slow on my GPU
		//glEnable(GL_CLIP_PLANE0);
		//glClipPlane(GL_CLIP_PLANE0, plane);//GLenum buffers[2] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};//, GL_DEPTH_ATTACHMENT_EXT };
}


void WaterSurface::endReflection()
{
	glEnable(GL_CULL_FACE);
	// vymazat transformaci
	glPopMatrix();
	// viewport zpatky...
		//glDisable(GL_CLIP_PLANE0); // slow on my GPU
	if (activeCamera->getPosition().y>=WATER_HEIGHT){
		glCullFace(GL_BACK);
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glViewport(0,0,g_WinWidth, g_WinHeight);
	
}

void WaterSurface::beginRefraction()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_refr_ID);
    // prenastavit viewport
		glViewport(0,0,g_WinWidth, g_WinHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glDrawBuffersARB(2, buffers);
		// nastavit transformaci...
		glPushMatrix();
		if (activeCamera->getPosition().y>=WATER_HEIGHT){
			glTranslatef(0.0f, WATER_HEIGHT, 0.0f);
			glScalef(1.f, 0.5f, 1.f); // flip Y
		} else {
			glScalef(1.f, 1.0f, 1.f);
			glTranslatef(0.0f, WATER_HEIGHT-1.f, 0.0f);
		}
		// draw underwater world...
}


void WaterSurface::endRefraction()
{
	// vymazat transformaci
	glPopMatrix();
	// viewport zpatky...
	glViewport(0,0,g_WinWidth, g_WinHeight);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	
}

void WaterSurface::showTextures()
{
	show_texture(cb_refl_ID, 0,0,200,200);
	show_texture(db_refl_ID, 200,0,200,200);
	show_texture(cb_refr_ID, 400,0,200,200);
	show_texture(db_refr_ID, 600,0,200,200);
}