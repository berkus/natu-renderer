//-----------------------------------------------------------------------------
//  [GSY] SimpleShader - Simple GLSL framework
//  15/03/2009
//-----------------------------------------------------------------------------
//  Controls: 
//		[c]		... compile shader program
//		[ ]		... toggle fixed/programmable pipeline
//		[h]		... show/hide GUI controls
//		[Ecs]	... quit
//-----------------------------------------------------------------------------

#include "main.h"
#include "../_utils/shaders.h"
#include "../_utils/GLUT/glut.h"
#include "../_utils/Vector3.h"

#include "png.h"
#include "TestObject.h"
#include "branch.h"
#include "tree.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "TreeBranch.h"

// GLOBAL DEFINITIONS__________________________________________________________

// GLOBAL CONSTANTS____________________________________________________________

// GLOBAL VARIABLES____________________________________________________________
GLuint		g_Program				= 0;	// Shader program GL id
GLuint		g_VertexShader			= 0;	// Vertex shader GL id
GLuint		g_FragmentShader		= 0;	// Fragment shader GL id
GLuint		g_GeometryShader		= 0;	// Geometry shader GL id
GLint hTimer = 0;
GLuint texId = 0;

bool tqAvailable;
GLuint tqid;
float fps;

float varA = 0.1;
float varB = 0.8;
float varC = 0.7;
float valA = 0;
float valB = 0;
float ttime = 0;

Branch *br, *br2;
Branch *xbr1, *xbr2, *xbr3;
CoordSystem* cs, *cs2;
TestOBJ *tobj;
//Tree	*tree;

Tree	*tree2;
Tree	*tree3;

TextureManager texManager;
ShaderManager  shaManager;

// FORWARD DECLARATIONS________________________________________________________

// USER DEFINED CALLBACK FUNCTIONS_____________________________________________
void callback_VariableAchanged(float value)
{
	varA = value;
	printf("variable A changed, value=%f\n", varA);
}

void callback_VariableBchanged(float value)
{
	varB = value;
	printf("variable B changed, value=%f\n", varB);
}

void callback_VariableCchanged(float value)
{
	varC = value;
	printf("variable C changed\n");
}
static int isExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *pos, *terminator;
	/* Kontrola parametru – jméno musí být bez mezer. */
	pos = (GLubyte *) strchr(extension, ' ');// ptr na ‘ ‘
	if (pos || *extension == '\0') // nebo prázdné jméno
		return 0;
	extensions = glGetString(GL_EXTENSIONS);
	/* zjištìní pøítomnosti extenze */
	start = extensions;
	for (;;) {
		pos = (GLubyte *) strstr((const char *) start, extension);
		if (!pos)
			break;
		/* není to jen podøetìzec delšího jména extenze? */
		terminator = pos + strlen(extension);
		if (pos == start || *(pos - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return 1;
		start = terminator;
	}
	return 0;
}
/*
void callback_CompileShaders()
{
	bool bResult = createShaderProgram(	g_Program,
										&g_VertexShader,
										&g_GeometryShader,
										&g_FragmentShader,
										(g_bAttachVertexShader   == true) ? "Shaders/simple.vs" : NULL,
										(g_bAttachGeometryShader == true) ? "Shaders/simple.gs" : NULL,
										(g_bAttachFragmentShader == true) ? "Shaders/simple.fs" : NULL,
										6,
										GL_TRIANGLES,
										GL_LINE_STRIP);
										// 6 = kolik muze gs vygenerovat primitiv
										// GL_TRIANGLES ... jaka primitiva prijma
										// GL_TRIANGLE_STRIP ... jaka primitiva produkuje
	// Locate some parameters by name so we can set them later__________________
	//hTimer = glGetUniformLocation(g_Program, "timer");
	//tobj->shaderProgramID = g_Program;
	
	g_bUseShaders = true;
	callback_UseShaders(bResult);

	// prepare tree...
	//tree->init();
}
*/


// RENDER FUNCTIONS____________________________________________________________

//-----------------------------------------------------------------------------
// Name: display()
// Desc: 
//-----------------------------------------------------------------------------
void display()
{
	static float rotY = 0.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, -1.0f, g_CameraZ );
    glRotatef( g_RotObject[0], 1.0f, 0.0f, 0.0f );
    glRotatef( g_RotObject[1] + rotY, 0.0f, 1.0f, 0.0f );
	
	glColor3f(1.f, 1.f, 1.f);
	// draw tree/branch

	char * fpsChars = new char[100];

	if (tqAvailable){
	    // measure on GPU			
			glBeginQuery(GL_TIME_ELAPSED, tqid);
		
			
			
		tree2->draw();	
		
		
		//if (result_available)
			glEndQuery(GL_TIME_ELAPSED);
		
		GLuint64EXT time = 0;
		glGetQueryObjectui64vEXT(tqid, GL_QUERY_RESULT, &time); // blocking CPU
		fps = 1000000000.0/ double(time);
		int cnt = printf("fps: %f", fps);

		sprintf(fpsChars, "fps: %f", fps);
		BACK(cnt);
	} else {
		tree2->draw();
	}

	// Render GUI controls
	fpsLabel->setText(fpsChars);
	GUIManager::display();			// Render gui components

	glutSwapBuffers();

	if (g_bAutoRotationEnabled)
	{
		rotY += 0.75f;
	}
}


void initApp(void){


	if (isExtensionSupported(TIME_QUERY_EXTENSION)){
		tqAvailable = true;
		glGenQueries(1, &tqid);
	}

	// init shaders
	// branch shader
	Shader * pBranchShader = new Shader();
	if (!pBranchShader->createShaderProgram("Shaders/branch_vs2.glsl", "Shaders/branch_fs.glsl")){
		// error... stop
		system("PAUSE");
		exit(1);
	}
	shaManager.addShader(pBranchShader);
	// leaf shader
	Shader * pLeafShader = new Shader();
	if (!pLeafShader->createShaderProgram("Shaders/leaf_vs.glsl", "Shaders/leaf_fs.glsl")){
		// error... stop
		system("PAUSE");
		exit(1);
	}
	shaManager.addShader(pLeafShader);
	
	// init coord system of branch
	/*
	Vector3 org(0.f,0.f, 0.f);
	Vector3 r(1.f,0.f, 0.f);
	Vector3 s(0.f,0.f, 1.f);
	Vector3 t(0.f,1.f, 0.f);
	cs = new CoordSystem(org, r,s,t);
	//cs->rotate(v3(1.f,0.f,1.f), 0.01);
	CoordSystem	cs2 = cs->getRotated(v3(0.f, 0.f, 1.f), M_PI/3.0);
	CoordSystem	cs3 = cs->getRotated(v3(0.f, 0.f, 1.f), -M_PI/2.0);
	CoordSystem	cs4 = cs->getRotated(v3(0.f, 0.f, 1.f), 0.0);

	v3 motionVector(1.f, 1.f, 1.f);
	float angle = randomf(0.f, M_PI );
	float scale = 1.0;
	motionVector = v3(scale*cos(angle), scale*sin(angle), 0.f);
	TreeBranch * br1 = new TreeBranch(NULL, *cs, 0.0f, &texManager, 1.0f, 0.2,0.1,2,4,0.3326, 0.398924, motionVector);
	br1->setBending(0.6,0);

	angle = randomf(0.f, M_PI );
	motionVector = v3(scale*cos(angle), scale*sin(angle), 0.f);
	TreeBranch * br2 = new TreeBranch(br1, cs2, 0.5f, &texManager, 0.5f, 0.1,0.05,1,4,0.3326, 0.398924, motionVector);
	br2->setBending(0.1,0);

	angle = randomf(0.f, M_PI );
	motionVector = v3(scale*cos(angle), scale*sin(angle), 0.f);
	TreeBranch * br3 = new TreeBranch(br1, cs3, 0.6f, &texManager, 0.3f, 0.1,0.05,1,4,0.3326, 0.398924, motionVector);
	br3->setBending(0.1,0);
	
	angle = randomf(0.f, M_PI );
	motionVector = v3(scale*cos(angle), scale*sin(angle), 0.f);
	TreeBranch * br4 = new TreeBranch(br1, cs4, 1.0f, &texManager, 0.3f, 0.1,0.05,1,4,0.3326, 0.398924, motionVector);
	br4->setBending(0.1,0);

	tree2 = new Tree();
	tree2->branchShaderID = pBranchShader->programID;
	//tree2->leafShaderID	  = pLeafShader->programID;
	tree2->trunk = br1;

	*/
	tree2 = new Tree();
	tree2->branchShaderID	= pBranchShader->programID;
	tree2->leafShaderID		= pLeafShader->programID;
	tree2->load("data/JavorListy.objt", &texManager);

	tree2->init();
	
	//glEnable(GL_TEXTURE_RECTANGLE_ARB);
}

void deinitApp(){
	if (tqAvailable){
		glDeleteQueries(1, &tqid);
	}
	delete br;
	delete br2;
	delete cs;
	delete cs2;
}

void onTimer(int value)
{
	ttime += PI/100;

	tree2->setTime(ttime);
	
	glutTimerFunc(10, onTimer, value);
}


//-----------------------------------------------------------------------------
// Name: initGL()
// Desc: 
//-----------------------------------------------------------------------------
void initGL()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

	float lightPos[4]		= {0.0f, 3.0f, 3.0f, 1.0f};
	float lightAmbient[4]	= {0.4f, 0.4f, 0.4f, 1.0f};
	float lightDiffuse[4]	= {0.0f, 0.0f, 0.0f, 1.0f};
	float lightSpecular[4]	= {0.5f, 0.5f, 0.5f, 1.0f};
	float lightDirection[4]	= {0.0f,-1.5f,-3.0f, 1.0f};
	
	float matGlobal[4]	= {0.1f, 0.1f, 0.1f, 1.0f};
	float matSpecular[4]= {0.8f, 0.8f, 0.8f, 1.0f};

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matGlobal);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matGlobal);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 256);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_POSITION      , lightPos);
	glLightfv(GL_LIGHT1, GL_AMBIENT       , lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE       , lightDiffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR	  , lightSpecular);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection);
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF	  , 24);
	glLightf (GL_LIGHT1, GL_SPOT_EXPONENT , 512);
	
	g_bShadersSupported = (	GLEE_ARB_shader_objects && GLEE_ARB_shading_language_100 &&
							GLEE_ARB_vertex_shader  && GLEE_ARB_fragment_shader);
	//texId = createSimpleTexture(256,256);

}


//-----------------------------------------------------------------------------
// Name: reshape()
// Desc: 
//-----------------------------------------------------------------------------
void reshape (int w, int h)
{
	if ((w > 1) && (h > 1))
	{
		g_WindowWidth  = w;
		g_WindowHeight = h;

		glViewport(0, 0, g_WindowWidth, g_WindowHeight);

		glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(60, GLfloat(g_WindowWidth)/ g_WindowHeight, 0.1f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
	}
}


// GLUT FUNCTIONS______________________________________________________________

//-----------------------------------------------------------------------------
// Name: keyboard()
// Desc: 
//-----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y)
{
	printf("KEY\n");
	switch(key)
	{
		case 27 : 
			// Destroy shaders
			//destroyShaderProgram(g_Program, &g_VertexShader, &g_GeometryShader, &g_FragmentShader);
			exit(0);
		break;
		case ' ': 
			//callback_UseShaders(!g_bUseShaders);
		break;
		case 'c': 
			//callback_CompileShaders();
		break;
		case 'h':
			GUIManager::visible(!GUIManager::isVisible());
		break;

		case 's':
			if (g_WindStrength>0.0){
				g_WindStrength -= 0.1;
			}
			printf("Wind strength = %f\n", g_WindStrength);
			break;
		case 'S':
			if (g_WindStrength<2.0){
				g_WindStrength += 0.1;
			}
			printf("Wind strength = %f\n", g_WindStrength);
			break;

		case 'd':
			g_WindDirection.rotateY(-0.1);
			printf("Wind direction = %f, %f, %f\n", g_WindDirection.x, g_WindDirection.y,g_WindDirection.z);
			break;
		case 'D':
			g_WindDirection.rotateY(0.1);
			printf("Wind direction = %f, %f, %f\n", g_WindDirection.x, g_WindDirection.y,g_WindDirection.z);
			break;

	}

	glutPostRedisplay();
}


//-----------------------------------------------------------------------------
// Name: mouse()
// Desc: 
//-----------------------------------------------------------------------------
void mouse(int button, int state, int x, int y)
{
	if (!guiActive){
		if (state == GLUT_DOWN)
		{
			g_LastMouseX = x;
			g_LastMouseY = y;

			g_LastMouseButton = button;
			
			GUIManager::checkClickEvent(x, y);
		}
	}
}


//-----------------------------------------------------------------------------
// Name: mouseMotion()
// Desc: 
//-----------------------------------------------------------------------------
void mouseMotion(int x, int y)
{
	if (!guiActive){
		switch(g_LastMouseButton)
		{
			case GLUT_LEFT_BUTTON:
				g_RotObject[0] += ROTATION_SPEED*(y - g_LastMouseY);
				g_RotObject[1] += ROTATION_SPEED*(x - g_LastMouseX);
			break;

			case GLUT_RIGHT_BUTTON:
				g_CameraZ += TRANSLATION_SPEED*(x - g_LastMouseX);
			break;
		}
	}
	g_LastMouseX = x;
	g_LastMouseY = y;
}

	
//-----------------------------------------------------------------------------
// Name: mousePassiveMotion()
// Desc: 
//-----------------------------------------------------------------------------
void mousePassiveMotion(int x, int y)
{
	GUIManager::checkMoveEvent(x, y);
}


//-----------------------------------------------------------------------------
// Name: main()
// Desc: 
//-----------------------------------------------------------------------------
int main (int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	
	glutInitWindowPosition(800, 100);
	glutInitWindowSize(g_WindowWidth, g_WindowHeight);
	glutCreateWindow("NATUrenderer");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(display);

    glutTimerFunc(1, onTimer, 1234);


	glutMotionFunc(mouseMotion);
	glutPassiveMotionFunc(mousePassiveMotion);
	glutMouseFunc(mouse);

	initGL();
	initGUI();
	
	initApp();
	
	glutMainLoop();
	
	deinitApp();
	return 0;
}