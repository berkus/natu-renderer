#include "Light.h"


Light::Light(TextureManager * tm)
{
	positionFixedToSkybox = v3(0.0,0.0,0.0);
	textureManager = tm;
	shader = NULL;
	init();
	
}


Light::~Light(void)
{
	SAFE_DELETE_PTR ( shader );
}
void Light::init()
{
	shader = new Shader("sun");
	shader->loadShader("shaders/sun_vs.glsl", "shaders/sun_fs.glsl");
	upVector =v3(0.0, 1.0, 0.0);
}

void Light::update(double time)
{
	v3 dir= direction->xyz();
	right = dir.cross(upVector).getNormalized();
	upVector = right.cross(dir);
}

void Light::drawSun(v3* position){
	glColor3f(1.0, 1.0, 0.98);
	glPushMatrix();
		glTranslatef(
			position->x - direction->x * 250.0,
			position->y - direction->y * 250.0,
			position->z - direction->z * 250.0
			);
		shader->use(true);
		glutSolidSphere(20.0, 10,10);
		shader->use(false);
	glPopMatrix();

}

void Light::translate(v3 &movVector)
{
	*position += v4(movVector, 0.0);
}

void Light::rotate(v3 &axis, float angleRad)
{
	//direction->rotate(angleRad, axis);
}

void Light::scale(v3 &scaleVector)
{

}

void Light::showTextures(){
	show_texture(cb_shad_ID, 0,0,400,400);
	show_texture(db_shad_ID, 0,400,400,400);
}

void Light::initShadowMapping(Camera *_cam, int resolution)
{
	camera = _cam;
	resolution_x = resolution;
	resolution_y = resolution;
	/*
	glGenTextures(1, &db_shad_ID );
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, db_shad_ID );
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 2, GL_DEPTH_COMPONENT32F, resolution_x, resolution_y, GL_TRUE);
	
	glGenTextures(1, &cb_shad_ID );
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, cb_shad_ID );
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 2, GL_RGB, resolution_x, resolution_y, GL_TRUE);
	 
	 glGenFramebuffersEXT(1, &fb_shad_ID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_shad_ID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D_MULTISAMPLE, db_shad_ID, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D_MULTISAMPLE, cb_shad_ID, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	*/
	

	glGenTextures(1, &db_shad_ID );
		glBindTexture(GL_TEXTURE_2D, db_shad_ID );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	
	glGenTextures(1, &cb_shad_ID );
		glBindTexture(GL_TEXTURE_2D, cb_shad_ID );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	 
	 glGenFramebuffersEXT(1, &fb_shad_ID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_shad_ID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, db_shad_ID, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cb_shad_ID, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	

	shadowMapTexture = new Texture();
	//shadowMapTexture->type = GL_TEXTURE_2D_MULTISAMPLE;
	shadowMapTexture->id = db_shad_ID;
	shadowMapTexture->inShaderName = "shadowMap";
	shadowMapTexture->textureUnitNumber = 7; // last texture (0-7)
	shadowMapTMID = textureManager->addTexture(shadowMapTexture);
	textureManager->shadowMapID = shadowMapTMID;
	g_shadowmap1 = shadowMapTexture;
	MVPmatrix = new m4();
	g_LightMVPmatrix = MVPmatrix;
}


void Light::beginShadowMap(){
	// set camera to light position and set its params...
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_shad_ID);
    // setup viewport
		glViewport(0,0,resolution_x, resolution_y);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(-width*0.5, width*0.5, -height*0.5, height*0.5, near ,far);
		
		glGetFloatv(GL_MODELVIEW_MATRIX, &Pmatrix.m[0]);
		glLoadIdentity();
		gluLookAt(	position->x	, position->y , position->z	,
					position->x + direction->x	, position->y + direction->y, position->z + direction->z	,
					upVector.x	,upVector.y , upVector.z	);	
		glGetFloatv(GL_MODELVIEW_MATRIX, &MVmatrix.m[0]);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadMatrixf(Pmatrix.m);
	glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadMatrixf(MVmatrix.m);

	*MVPmatrix = Pmatrix * MVmatrix;
	
	// redirect renderign to framebuffer
	
	// set viewport
	//glViewport(0, 0, SHADOWMAP_RESOLUTION_X, SHADOWMAP_RESOLUTION_Y);
	// ... render scene at fastest settings
	//glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	/*
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	*/
}
void Light::endShadowMap(){
	/*
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	glDisable(GL_MULTISAMPLE);
	*/
	// redirect rendering back to back screen buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// reset viewport
	glViewport(0, 0, g_WinWidth, g_WinHeight);
	// set camera...
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Light::drawForLOD()
{
	draw();
}

void Light::draw()
{
	glDisable(GL_LIGHTING);
	v3 dir = direction->xyz();


	v3 tl =		upVector*0.5*height - right*0.5*width		+ dir * near;
	v3 tr =		upVector*0.5*height + right*0.5*width		+ dir * near;
	v3 bl = -	upVector*0.5*height - right*0.5*width		+ dir * near;
	v3 br = -	upVector*0.5*height + right*0.5*width		+ dir * near;




	glPushMatrix();
	glTranslatef(position->x, position->y, position->z);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
		glVertex3f( 0.0, 0.0, 0.0);
		glVertex3f( upVector.x, upVector.y, upVector.z);
	glEnd();
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
		glVertex3f( 0.0, 0.0, 0.0);
		glVertex3f( right.x, right.y, right.z);
	glEnd();
	glColor3f(0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
		glVertex3f( 0.0, 0.0, 0.0);
		glVertex3f( dir.x, dir.y, dir.z);
	glEnd();



	glColor3f(0.0, 0.0, 0.0);
	glutSolidSphere(0.2, 5, 5);
	
	glBegin(GL_LINE_STRIP);		
		glVertex3f(tl.x, tl.y, tl.z);
		glVertex3f(tr.x, tr.y, tr.z);
		glVertex3f(br.x, br.y, br.z);
		glVertex3f(bl.x, bl.y, bl.z);
		glVertex3f(tl.x, tl.y, tl.z);
	glEnd();

	v3 tlf =  tl + dir * far;
	v3 trf =  tr + dir * far;
	v3 blf =  bl + dir * far;
	v3 brf =  br + dir * far;

	glBegin(GL_LINE_STRIP);		
		glVertex3f(tlf.x, tlf.y, tlf.z);
		glVertex3f(trf.x, trf.y, trf.z);
		glVertex3f(brf.x, brf.y, brf.z);
		glVertex3f(blf.x, blf.y, blf.z);
		glVertex3f(tlf.x, tlf.y, tlf.z);
	glEnd();

	glBegin(GL_LINES);		
		glVertex3f(tl.x, tl.y, tl.z);
		glVertex3f(tlf.x, tlf.y, tlf.z);
		glVertex3f(tr.x, tr.y, tr.z);
		glVertex3f(trf.x, trf.y, trf.z);
		glVertex3f(br.x, br.y, br.z);
		glVertex3f(brf.x, brf.y, brf.z);
		glVertex3f(bl.x, bl.y, bl.z);
		glVertex3f(blf.x, blf.y, blf.z);
	glEnd();

	//glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, db_shad_ID); 
	glColor4f(1.f,1.f,1.f,0.5f);

	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0, 0.f, 0.f); glVertex3f(blf.x, blf.y, blf.z);
		glMultiTexCoord2f(GL_TEXTURE0, 1.f, 0.f); glVertex3f(brf.x, brf.y, brf.z);
		glMultiTexCoord2f(GL_TEXTURE0, 1.f, 1.f); glVertex3f(trf.x, trf.y, trf.z);
		glMultiTexCoord2f(GL_TEXTURE0, 0.f, 1.f); glVertex3f(tlf.x, tlf.y, tlf.z);		
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0); 
	//glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
	/*
	float lightLineSize = 50.f;
	// draw a line from origin to the direction
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	//glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(0.0,8.0,0.0);
		glColor3f(1.0,0.0,0.0);
		
		glBegin(GL_LINES);
			glVertex3f(0.0, 0.0, 0.0);
			glVertex3f(lightLineSize*position.x, lightLineSize*position.y, lightLineSize*position.z);
		glEnd();
	*/



	//glPopMatrix();
	glEnable(GL_LIGHTING);
	//glPopAttrib();



	// draw a circle on the screen where the light is...

	/*
	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslate(positionFixedToSkybox);
		glColor3f(1.0,0.0,0.0);
		glutSolidSphere(10.0, 5, 5);
	glPopMatrix();
	glEnable(GL_LIGHTING);
	glPopAttrib();
	*/
}

void Light::turnOn()
{
	glEnable(GL_LIGHTING);
	glEnable(lightId);
	//position.printOut();
	//position = -g_light_direction;
	glLightfv(lightId, GL_POSITION      , direction->data);
	//glLightfv(lightId, GL_SPOT_DIRECTION, (*direction).data);
	//direction->printOut();
	
}

void Light::turnOff()
{
	glDisable(lightId);
}

void Light::setup(GLuint lid, v4 *pos, v4 *dir, v4 &ambi, v4 &diff, v4 &spec, float cutOff, float exp)
{
	lightId		= lid;
	position	= pos;
	direction	= dir;
	width		= 20.0;
	height		= 20.0;
	near		= g_ShadowNear;
	far			= g_ShadowFar;
	g_ShadowFarMNear = far-near;
	glLightfv(lightId, GL_POSITION      , position->data);
	glLightfv(lightId, GL_SPOT_DIRECTION, direction->data);
	glLightfv(lightId, GL_AMBIENT       , ambi.data);
	glLightfv(lightId, GL_DIFFUSE       , diff.data);
	glLightfv(lightId, GL_SPECULAR	    , spec.data);
	//glLightf (lightId, GL_SPOT_CUTOFF	, cutOff);
	//glLightf (lightId, GL_SPOT_EXPONENT , exp);
}
