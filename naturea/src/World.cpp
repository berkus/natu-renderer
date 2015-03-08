#include "World.h"


World::World(void)
{
	p_activeCamera		= NULL;
	p_activeLight		= NULL;
	p_skybox			= NULL;
	p_terrain			= NULL;
	p_grass_prototype	= NULL;
	p_grass_growth		= NULL;
	p_tree1_prototype	= NULL;
	p_tree1_growth		= NULL;
	p_tree2_prototype	= NULL;
	p_tree2_growth		= NULL;
	p_dtree				= NULL;
	p_godRays			= NULL;

	p_backgroundSound	= NULL;

	p_test_model		= NULL;
	p_test_model2		= NULL;

	tree_positions		= NULL;
	p_fog				= NULL;
}


World::~World(void)
{
	SAFE_DELETE_PTR(p_activeCamera);
	SAFE_DELETE_PTR(p_activeLight);
	SAFE_DELETE_PTR(p_skybox);
	SAFE_DELETE_PTR(p_terrain);

	SAFE_DELETE_PTR(p_grass_prototype);
	SAFE_DELETE_PTR(p_grass_growth);
	SAFE_DELETE_PTR(p_tree1_prototype);
	SAFE_DELETE_PTR(p_tree1_growth);
	SAFE_DELETE_PTR(p_tree2_prototype);
	SAFE_DELETE_PTR(p_tree2_growth);
	SAFE_DELETE_PTR(p_dtree);

	SAFE_DELETE_PTR( p_test_model	);
	SAFE_DELETE_PTR( p_test_model2	);
	SAFE_DELETE_PTR( p_backgroundSound);
	SAFE_DELETE_PTR ( p_fog );
	/*
	for (int i=0; i<100; i++){
		SAFE_DELETE_PTR ( tree_positions[i] );
	}
	SAFE_DELETE_ARRAY_PTR( tree_positions );
	*/
	SAFE_DELETE_PTR( p_godRays	);
	textureManager.~TextureManager();
	shaderManager.~ShaderManager();


	printf("WORLD deleted.......\n"); 
}

void World::startLODBuffer()
{

	glPushAttrib(GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	g_fastMode = true;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_LOD_ID);
    // prenastavit viewport
		glViewport(0,0, g_WinWidth * LOD_REDUCTION , g_WinHeight * LOD_REDUCTION);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void World::endLODBuffer()
{
	
	g_fastMode = false;
	// restore rendering settings
	glPopAttrib();

	// redirect rendering back to back screen buffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	// reset viewport
	glViewport(0, 0, g_WinWidth, g_WinHeight);

}



void World::draw()
{
	
	// preprocess - one pass for LOD & occlusion queries
	if (g_orbit){
		p_activeCamera->orbitY(g_center, g_orbit_radius, g_timeDiff*g_orbit_speed);
	}
	p_activeCamera->shoot();
	/*
	startLODBuffer();
		drawForLOD();
	endLODBuffer();
	*/

	// 1st pass (light shadows - create shadow map...)
	if (g_ShadowMappingEnabled){
		p_activeLight->beginShadowMap();
			// render whole shadow casting&recieving scene
			drawForShadowmapping();
		p_activeLight->endShadowMap();
	}
	// 2nd pass (water)
	//p_activeCamera->shoot();
	p_activeLight->turnOn();
	
	

	/*
	m4 camViewMatrixInverse = p_activeCamera->getViewMatrix().getInverse();

	g_LightMVPCameraVInverseMatrix	= p_activeLight->MVPmatrix * camViewMatrixInverse;
	g_LightMVCameraVInverseMatrix	= p_activeLight->MVmatrix * camViewMatrixInverse;
	g_LightPMatrix					= p_activeLight->Pmatrix;

	
	p_water->activeCamera = p_activeCamera;
	// WATER REFLECTION RENDER
	p_water->beginReflection();
		drawReflection();
	p_water->endReflection();
	// WATER REFRACTION RENDER
	p_water->beginRefraction();
		drawUnderWater();
	p_water->endRefraction();
	//p_water
	*/
	


	// 3rd pass (assembly)
	//p_fog->turnOn();


	if (g_godraysEnabled){p_godRays->begin();}
	
	if (g_draw_light_direction){
		p_activeLight->draw();
	}
	p_activeLight->drawSun(& p_activeCamera->position);
	p_terrain->draw();
	p_skybox->draw();
	p_fog->turnOn();
	
	
	
	/*
	if (p_activeCamera->getPosition().y>=WATER_HEIGHT-1.0){
		//p_fog->turnOn();
		p_terrain->drawOverWater();
		//p_fog->turnOff();
	}
	if (p_activeCamera->getPosition().y<=WATER_HEIGHT+1.0){
		drawUnderWater();
	}
	*/
	
	//p_water->draw(); // draw water surface
	
	
	//drawModels();
	
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	drawWithAlpha();
	
	glDisable(GL_BLEND);

	p_fog->turnOff();

	if (g_light_showDir) {
		p_activeLight->draw();
	}

	if (g_godraysEnabled){
		v3 lightDir = (p_activeLight->direction->xyz()).getNormalized();
		p_godRays->lightDirDOTviewDirValue = -lightDir.dot(p_activeCamera->getDirection());
		p_godRays->end();
	}
	
	if(g_showTextures){
		//p_water->showTextures();
		p_activeLight->showTextures();
		//show_textures();
	}
	//p_terrain->drawNormals();
	

	

	//cubeEBO->draw(cubeShader);
}

void World::drawWithAlpha(){
	/*
	if (g_draw_dtree){
		//p_dtree->draw();
		
		v3 camPos = p_activeCamera->getPosition();
		oneBubbleSortWalkthrough(tree_positions, g_tree_gridSize*g_tree_gridSize, camPos);

		v4* f;
		
		for (int c=0; c<g_tree_gridSize*g_tree_gridSize; c++){
			f = tree_positions[c];
			glPushMatrix();
			glTranslatef(f->x, f->y, f->z);
				glRotatef(f->w, 0.0, 1.0, 0.0);
				p_dtree->position = f->xyz();
				p_dtree->rotationY = f->w;
				p_dtree->draw();
			glPopMatrix();
		}
		// 
		// 
		// glPushMatrix();
		// 	glTranslatef(0.0, 5.0, 0.0);
		// 	glScalef(10.0, 10.0, 10.0);
		// 	
		// 	p_dtree->draw();
		// glPopMatrix();
		// 
	}
	/***/
	//p_test_model->draw();
	
	if (g_draw_low_vegetation){
		p_grass_growth->draw();
		p_tree1_growth->draw();
		p_tree2_growth->draw();
	}
	p_dtree->draw();
	
}

void World::drawUnderWater(){
	//p_underWaterFog->turnOn();
	p_terrain->drawUnderWater();
	//p_underWaterFog->turnOff();
}
void World::drawReflection(){
	/*
	g_drawingReflection = true;
	p_skybox->draw();
	p_terrain->cut = true;
	p_terrain->flip= true;
	//double plane[4] = {0.0, 1.0, 0.0, -WATER_HEIGHT+0.5};
	//glEnable(GL_CLIP_PLANE0);
	//glClipPlane(GL_CLIP_PLANE0, plane);
	p_terrain->drawOverWater();
	
	// draw models in low LOD
	int count = v_models.size();
	for (int i=0; i < count; i++){
		((LODmodel*)(v_models[i]))->drawForReflection();
	}


	//glDisable(GL_CLIP_PLANE0);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	p_grass_growth->draw();
	p_tree1_growth->draw();
	p_tree2_growth->draw();
	glDisable(GL_BLEND);
	g_drawingReflection = false;
	*/
}

void World::windowSizeChanged(int width, int height)
{
	//p_water->windowSizeChanged(width, height);
	p_activeCamera->ratio = float(max(width, height))/float(min(width, height));
	p_activeCamera->frustum_treshold = p_activeCamera->getFrustumTreshold();
	p_godRays->windowSizeChanged(width, height);
}

void World::drawForShadowmapping()
{
	g_Draw2Shadowmap = true;
	p_terrain->cut = false;
	p_terrain->drawForLOD();
	glEnable(GL_BLEND);
	// alpha test...
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// draw models with alpha

	p_dtree->draw();





	glDisable(GL_BLEND);

	// draw other models...
	/*
	int count = v_models.size();
	for (int i=0; i < count; i++){
		((LODmodel*)(v_models[i]))->drawForShadowMapping();
	}
	*/
	g_Draw2Shadowmap = false;
}

void World::drawForLOD(){
	p_terrain->drawForLOD();
	//p_water->drawForLOD(); // draw water surface
	p_grass_growth->drawForLOD();
	p_tree1_growth->drawForLOD();
	p_tree2_growth->drawForLOD();
	/*
	int count = v_models.size();
	for (int i=0; i < count; i++){
		v_models[i]->drawForLOD();
	}
	*/
}
void World::show_textures(){
	show_texture(cb_LOD_ID, 0,400,200,200);
}

void World::init()
{
	printf("INITIALIZING WORLD:\n");
	shaderManager.init();

	/*
	// create LOD buffers
	glGenTextures(1, &db_LOD_ID );
		glBindTexture(GL_TEXTURE_2D, db_LOD_ID );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, g_WinWidth * LOD_REDUCTION, g_WinHeight * LOD_REDUCTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glGenTextures(1, &cb_LOD_ID );
		glBindTexture(GL_TEXTURE_2D, cb_LOD_ID );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_WinWidth * LOD_REDUCTION, g_WinHeight * LOD_REDUCTION, 0, GL_RGB, GL_FLOAT, 0);
	 glGenFramebuffersEXT(1, &fb_LOD_ID);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb_LOD_ID);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,  GL_TEXTURE_2D, db_LOD_ID, 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, cb_LOD_ID, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	*/
	p_terrain = new Terrain(&textureManager,&shaderManager);
	
	p_activeCamera = new Camera();
	p_activeCamera->setup(HUMAN_POSITION, HUMAN_DIRECTION, v3(0.0,1.f,0.f), &g_WinWidth, &g_WinHeight, 60.0, 0.1f, 1000.f);
	//p_activeCamera->setup(v3(-100.0,60.f,60.f), v3(1.0,-0.7f,-0.4f), v3(0.0,1.f,0.f), &g_WinWidth, &g_WinHeight, 60.0, 1.f, 1000.f);
	//p_activeCamera->setup(LIGHT_POSITION, -LIGHT_POSITION, v3(0.0,1.f,0.f), &g_WinWidth, &g_WinHeight, 60.0, 1.f, 1000.f);
	p_activeCamera->setTerrain(p_terrain);
	p_activeCamera->setMode(g_cameraMode);
	g_viewer_position = & (p_activeCamera->position);
	g_viewer_direction= & (p_activeCamera->direction);

	// sun
	p_activeLight = new Light(&textureManager);
	p_activeLight->init(); // setup framebuffers for shadow mapping
	p_activeLight->setup(GL_LIGHT0, &g_light_position, &g_light_direction, sunAmb, sunDif, sunSpe, 180, 0.0);
	p_activeLight->turnOn();
	p_activeLight->initShadowMapping(p_activeCamera, SHADOWMAP_RESOLUTION_X);
	
	
	// sky
	p_skybox = new SkyBox(&textureManager, &shaderManager, SKYBOX_TEX_FILENAMES);
	p_skybox->init();
	p_skybox->p_activeCamera = p_activeCamera;
	p_skybox->p_light = p_activeLight;

	p_terrain->init();

	p_godRays = new GodRays(&shaderManager, p_activeLight);
	p_fog	  = new Fog(g_fog_density, g_fog_start, g_fog_end, v4(0.55, 0.55, 0.6, 1.0)*0.6);
	int count = 0;
	// plant grass	
	printf("Planting grass...\n");
	p_grass_prototype = new Grass(&textureManager,&shaderManager);
	p_grass_prototype->init();
	p_grass_growth = p_grass_prototype->getCopy();
	grass_planter.init(	p_terrain,
						p_grass_prototype,
						p_grass_growth,
						GRASS_MIN_HEIGHT,
						GRASS_MAX_HEIGHT,
						GRASS_MIN_DIST,
						500,
						500 );
	count = grass_planter.plantVegetationCount(g_GrassCount);
	printf("\tcount: %i\n", count);

	// plant trees
	printf("Planting trees1...\n");
	p_tree1_prototype = new Tree1(&textureManager,&shaderManager);
	p_tree1_prototype->init();
	p_tree1_growth = p_tree1_prototype->getCopy();
	tree1_planter.init(	p_terrain,
						p_tree1_prototype,
						p_tree1_growth,
						TREE1_MIN_HEIGHT,
						TREE1_MAX_HEIGHT,
						TREE1_MIN_DIST,
						100,
						100 );
 	count = tree1_planter.plantVegetationCount(g_Tree1Count);
	printf("\tcount: %i\n", count);

	// plant trees
	printf("Planting trees2...\n");
	p_tree2_prototype = new Tree2(&textureManager,&shaderManager);
	p_tree2_prototype->init();
	p_tree2_growth = p_tree2_prototype->getCopy();
	tree2_planter.init(	p_terrain,
						p_tree2_prototype,
						p_tree2_growth,
						TREE2_MIN_HEIGHT,
						TREE2_MAX_HEIGHT,
						TREE2_MIN_DIST,
						100,
						100 );
 	count = tree2_planter.plantVegetationCount(g_Tree2Count);
	printf("\tcount: %i\n", count);

//===============================================================================
// TREE INIT
//===============================================================================
	printf("---- DYNAMIC TREE INIT BEGIN ----\n");
	p_dtree = new DTree(&textureManager, &shaderManager);
		p_dtree->loadOBJT( DYN_TREE::OBJT_FILENAME );
		//p_dtree->init();
		p_dtree->viewer_direction = &p_activeCamera->direction;
		p_dtree->viewer_position = &p_activeCamera->position;

	// positions
		dtree_planter.init(p_terrain, p_dtree);
		dtree_planter.size_factor = g_tree_areaFactor;
		dtree_planter.createCandidates(g_dtreemin, g_dtreemax, g_tree_dither, g_tree_mean_distance);
		dtree_planter.setInstanceCount(g_TreeDCount);
		
	p_dtree->init(); 
	printf("----- END DYNAMIC TREE INIT ----\n");
//===============================================================================
// END TREE INIT 
//===============================================================================

	// SOUNDs
	printf("Starting sound... \n");
	p_backgroundSound = new Sound();
	p_backgroundSound->play();
	printf("Sound is playing... \n");

	printf("WORLD CREATED:\n");

}

void World::initModels()
{
/*
	PGR2Model * l0;
	PGR2Model * l1;
	PGR2Model * l2;
	// house 1
	l0 = PGR2Model::loadFromFile(HOUSE1_LOD0_FILENAME, &textureManager, &shaderManager, 0);
	l1 = PGR2Model::loadFromFile(HOUSE1_LOD1_FILENAME, &textureManager, &shaderManager, 1);
	l2 = PGR2Model::loadFromFile(HOUSE1_LOD2_FILENAME, &textureManager, &shaderManager, 2);
	l0->init();
	
	l1->init();
	
	l2->init();
	house1 = new LODmodel();
	house1->translate			(HOUSE1_POSITION);
	house1->rotate				(HOUSE1_ROTATION_AXIS,
								 HOUSE1_ROTATION_ANGLE);
	house1->scale				(HOUSE1_SCALE);
	house1->tresholds =			 HOUSE1_LOD_TRESHOLDS;	
	house1->setLODModel(0, l0);
	house1->setLODModel(1, l1);
	house1->setLODModel(2, l2);
	house1->setLODModel(3, NULL);
	house1->actualLOD	= & g_Statistics.house1_lod;
	house1->LODvalue	= & g_Statistics.house1_samples;

	v_models.push_back(house1);
	
	// house 2
	l0 = PGR2Model::loadFromFile(HOUSE2_LOD0_FILENAME, &textureManager, &shaderManager, 0);
	l1 = PGR2Model::loadFromFile(HOUSE2_LOD1_FILENAME, &textureManager, &shaderManager, 1);
	l2 = PGR2Model::loadFromFile(HOUSE2_LOD2_FILENAME, &textureManager, &shaderManager, 2);
	l0->init();
	l1->init();
	l2->init();
	house2 = new LODmodel();
	house2->translate			(HOUSE2_POSITION);
	house2->rotate				(HOUSE2_ROTATION_AXIS,
								 HOUSE2_ROTATION_ANGLE);
	house2->scale				(HOUSE2_SCALE);
	house2->tresholds =			 HOUSE2_LOD_TRESHOLDS;	
	house2->setLODModel(0, l0);
	house2->setLODModel(1, l1);
	house2->setLODModel(2, l2);
	house2->setLODModel(3, NULL);
	house2->actualLOD	= & g_Statistics.house2_lod;
	house2->LODvalue	= & g_Statistics.house2_samples;
	v_models.push_back(house2);

	// bridge
	l0 = PGR2Model::loadFromFile(BRIDGE_LOD0_FILENAME, &textureManager, &shaderManager, 0);
	l1 = PGR2Model::loadFromFile(BRIDGE_LOD1_FILENAME, &textureManager, &shaderManager, 1);
	//l2 = PGR2Model::loadFromFile(BRIDGE_LOD2_FILENAME, &textureManager);			  , 2
	l0->init();
	l1->init();
	//l2->init();
	bridge = new LODmodel();
	bridge->translate			(BRIDGE_POSITION);
	bridge->rotate				(BRIDGE_ROTATION_AXIS,
								 BRIDGE_ROTATION_ANGLE);
	bridge->scale				(BRIDGE_SCALE);
	bridge->tresholds =			 BRIDGE_LOD_TRESHOLDS;	
	bridge->setLODModel(0, l0);
	bridge->setLODModel(1, l1);
	bridge->setLODModel(2, l1);
	bridge->setLODModel(3, NULL);
	bridge->actualLOD	= & g_Statistics.bridge_lod;
	bridge->LODvalue	= & g_Statistics.bridge_samples;
	v_models.push_back(bridge);

	// tower1
	l0 = PGR2Model::loadFromFile(TOWER1_LOD0_FILENAME, &textureManager, &shaderManager, 0);
	l1 = PGR2Model::loadFromFile(TOWER1_LOD1_FILENAME, &textureManager, &shaderManager, 1);
	//l2 = PGR2Model::loadFromFile(BRIDGE_LOD2_FILENAME, &textureManager);			  , 2
	l0->init();
	l1->init();
	//l2->init();
	tower1 = new LODmodel();
	tower1->translate			(TOWER1_POSITION);
	tower1->rotate				(TOWER1_ROTATION_AXIS,
								 TOWER1_ROTATION_ANGLE);
	tower1->scale				(TOWER1_SCALE);
	tower1->tresholds =			 TOWER1_LOD_TRESHOLDS;	
	tower1->setLODModel(0, l0);
	tower1->setLODModel(1, l1);
	tower1->setLODModel(2, l1);
	tower1->setLODModel(3, NULL);
	tower1->actualLOD	= & g_Statistics.tower1_lod;
	tower1->LODvalue	= & g_Statistics.tower1_samples;
	v_models.push_back(tower1);

	// tower2
	l0 = PGR2Model::loadFromFile(TOWER2_LOD0_FILENAME, &textureManager, &shaderManager, 0);
	//l1 = PGR2Model::loadFromFile(TOWER2_LOD1_FILENAME, &textureManager);			  , 1
	//l2 = PGR2Model::loadFromFile(BRIDGE_LOD2_FILENAME, &textureManager);			  , 2
	l0->init();
	//l1->init();
	//l2->init();
	tower2 = new LODmodel();
	tower2->translate			(TOWER2_POSITION);
	tower2->rotate				(TOWER2_ROTATION_AXIS,
								 TOWER2_ROTATION_ANGLE);
	tower2->scale				(TOWER2_SCALE);
	tower2->tresholds =			 TOWER2_LOD_TRESHOLDS;	
	tower2->setLODModel(0, l0);
	tower2->setLODModel(1, l0);
	tower2->setLODModel(2, l0);
	tower2->setLODModel(3, NULL);
	tower2->actualLOD	= & g_Statistics.tower2_lod;
	tower2->LODvalue	= & g_Statistics.tower2_samples;
	v_models.push_back(tower2);

	// well
	l0 = PGR2Model::loadFromFile(WELL_LOD0_FILENAME, &textureManager, &shaderManager, 0);
	l1 = PGR2Model::loadFromFile(WELL_LOD1_FILENAME, &textureManager, &shaderManager, 1);
	//l2 = PGR2Model::loadFromFile(BRIDGE_LOD2_FILENAME, &textureManager);			, 2
	l0->init();
	l1->init();
	//l2->init();
	well = new LODmodel();
	well->translate			(WELL_POSITION);
	well->rotate				(WELL_ROTATION_AXIS,
								 WELL_ROTATION_ANGLE);
	well->scale				(WELL_SCALE);
	well->tresholds =			 WELL_LOD_TRESHOLDS;	
	well->setLODModel(0, l0);
	well->setLODModel(1, l1);
	well->setLODModel(2, l1);
	well->setLODModel(3, NULL);
	well->actualLOD	= & g_Statistics.well_lod;
	well->LODvalue	= & g_Statistics.well_samples;
	v_models.push_back(well);

	//p_model1 = PGR2Model::loadFromFile(HOUSE1_LOD0_FILENAME, &textureManager);
	//p_model1->init();
	*/
}
void World::drawModels()
{
	/*
	glPushMatrix();
	glTranslatef(50.f, 10.f, 0.f);
		p_model1->render(false);
	glPopMatrix();
	*/
	// draw other models...
	/*
	int count = v_models.size();
	for (int i=0; i < count; i++){
		v_models[i]->draw();
	}
	*/
}
void World::deinitModels()
{
	/*
	for (int i=0; i<v_models.size(); i++){
		SceneModel * m = v_models[v_models.size()-1];
		v_models.pop_back();
		delete m;
		m = NULL;
	}
	*/
}


void World::update(double i_time)
{
	p_activeCamera->setMode(g_cameraMode);
	p_activeCamera->update(i_time);
	p_backgroundSound->update();
	p_dtree->update(i_time);
	p_activeLight->update(i_time);
	//g_light_direction = g_light_position; 

	//g_time = i_time;
	/*int modelCnt = v_models.size();
	for (int i=0; i<modelCnt; i++){
		v_models[i]->update(time);
	}
	p_skybox->update(time);
	p_terrain->update(time);
	p_activeLight->update(time);
	p_grass->update(time);*/
}

void World::translate(v3 &movVector)
{

}


void World::rotate(v3 &axis, float angleRad)
{

}


void World::scale(v3 &scaleVector)
{

}


