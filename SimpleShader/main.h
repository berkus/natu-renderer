#ifndef __MAIN_H__
#define __MAIN_H__

#include <vector>
#include "../_utils/GLEE/glee.h" 
#include "settings.h"
#include "gui.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <iomanip>

#define BACKSPACE(cnt) for (int i=0; i<cnt; i++){printf("\b \b");}
#define BACK(cnt) for (int i=0; i<cnt; i++){printf("\b");}
// GLOBAL DEFINITIONS__________________________________________________________
#define TIME_QUERY_EXTENSION	"GL_EXT_timer_query"
#define GL_TIME_ELAPSED			0x88BF

// GLOBAL CONSTANTS____________________________________________________________
const float ROTATION_SPEED		= 0.5f;
const float TRANSLATION_SPEED	= 0.05f;

// GLOBAL VARIABLES____________________________________________________________
int			g_WindowWidth		= 512;
int			g_WindowHeight		= 512;

v3			g_WindDirection(1.0,0.0,0.0);
float		g_WindStrength = 1.0f;
v4			g_WoodAmplitude(1.0, 1.0, 1.0, 1.0);
v4			g_WoodFrequency(1.0, 1.0, 1.0, 1.0);
float		g_LeafAmplitude = 1.0;
float		g_LeafFrequency = 1.0;

GLfloat		g_RotObject[3]		= { 0 };		// Object rotation
GLfloat		g_CameraZ			= -5.0f;		// Camera Z position

// Previous mouse location
GLint		g_LastMouseX		= -1;
GLint		g_LastMouseY		= -1;
GLint		g_LastMouseButton	= 0;

bool		g_bAutoRotationEnabled = false;		// Rotate tea-pot automatically
bool		g_bShadersSupported		= false;	// GPU supports GLSL language
bool		g_bUseShaders			= false;	// Use fixed/programmable pipeline
bool		g_bAttachVertexShader	= false;	// Attach vertex shader to shader program
bool		g_bAttachFragmentShader	= false;	// Attach fragment shader to shader program
bool		g_bAttachGeometryShader	= false;	// Attach geometry shader to shader program

extern GLuint g_Program;						// Shader program GL id, declared in main.cpp
extern GLuint g_VertexShader;					// Vertex shader GL id, declared in main.cpp
extern GLuint g_FragmentShader;					// Fragment shader GL id, declared in main.cpp
extern GLuint g_GeometryShader;					// Geometry shader GL id, declared in main.cpp

GUILabel*		fpsLabel;

// FORWARD DECLARATIONS________________________________________________________
// Callback functions
//void callback_CompileShaders();					// Compile GLSL shaders
void callback_VariableAchanged(float value);	// Callback function for slider A
void callback_VariableBchanged(float value); 	// Callback function for slider B
void callback_VariableCchanged(float value); 	// Callback function for slider C



void clbk_WA0changed(float value){
	g_WoodAmplitude.x = value;
	printf("WA: %f, %f, %f, %f\n",
		g_WoodAmplitude.x,
		g_WoodAmplitude.y,
		g_WoodAmplitude.z,
		g_WoodAmplitude.w);
} 
void clbk_WA1changed(float value){
	g_WoodAmplitude.y = value;
	printf("WA: %f, %f, %f, %f\n",
		g_WoodAmplitude.x,
		g_WoodAmplitude.y,
		g_WoodAmplitude.z,
		g_WoodAmplitude.w);
} 
void clbk_WA2changed(float value){
	g_WoodAmplitude.z = value;
	printf("WA: %f, %f, %f, %f\n",
		g_WoodAmplitude.x,
		g_WoodAmplitude.y,
		g_WoodAmplitude.z,
		g_WoodAmplitude.w);
} 
void clbk_WA3changed(float value){
	g_WoodAmplitude.w = value;
	printf("WA: %f, %f, %f, %f\n",
		g_WoodAmplitude.x,
		g_WoodAmplitude.y,
		g_WoodAmplitude.z,
		g_WoodAmplitude.w);
} 

void clbk_WF0changed(float value){
	g_WoodFrequency.x = value;
	printf("WF: %f, %f, %f, %f\n",
		g_WoodFrequency.x,
		g_WoodFrequency.y,
		g_WoodFrequency.z,
		g_WoodFrequency.w);
} 
void clbk_WF1changed(float value){
	g_WoodFrequency.y = value;
	printf("WF: %f, %f, %f, %f\n",
		g_WoodFrequency.x,
		g_WoodFrequency.y,
		g_WoodFrequency.z,
		g_WoodFrequency.w);
} 
void clbk_WF2changed(float value){
	g_WoodFrequency.z = value;
	printf("WF: %f, %f, %f, %f\n",
		g_WoodFrequency.x,
		g_WoodFrequency.y,
		g_WoodFrequency.z,
		g_WoodFrequency.w);
} 
void clbk_WF3changed(float value){
	g_WoodFrequency.w = value;
	printf("WF: %f, %f, %f, %f\n",
		g_WoodFrequency.x,
		g_WoodFrequency.y,
		g_WoodFrequency.z,
		g_WoodFrequency.w);
} 

void clbk_WSchanged(float value){
	g_WindStrength = value;
	printf("WS: %f\n",
		g_WindStrength);
} 

void clbk_LAchanged(float value){
	g_LeafAmplitude = value;
	printf("LA: %f\n",
		g_LeafAmplitude);
} 

void clbk_LFchanged(float value){
	g_LeafFrequency = value;
	printf("LA: %f\n",
		g_LeafFrequency);
} 






// CALLBACK FUNCTIONS__________________________________________________________

//-----------------------------------------------------------------------------
// Name: callback_EnableWireMode()
// Desc: Enable/disable wire mode
//-----------------------------------------------------------------------------
void callback_EnableWireMode(bool enable)
{
	glPolygonMode(GL_FRONT_AND_BACK, (enable == true) ? GL_LINE : GL_FILL);
}


//-----------------------------------------------------------------------------
// Name: callback_RotateModel()
// Desc: Enable/disable automatic model rotation
//-----------------------------------------------------------------------------
void callback_RotateModel(bool bRotate)
{
	g_bAutoRotationEnabled = bRotate;
}


//-----------------------------------------------------------------------------
// Name: callback_AttachVertexShader()
// Desc: Attach/detach vertex shader
//-----------------------------------------------------------------------------
//void callback_AttachVertexShader	 (bool bAttach )
//{
//	g_bAttachVertexShader	= bAttach;
//	if (bAttach == false)
//	{
//		GUIManager::setElementActive("Attach VS CheckBox", false);
//	}
//}


//-----------------------------------------------------------------------------
// Name: callback_AttachGeometryShader()
// Desc: Attach/detach geometry shader
//-----------------------------------------------------------------------------
//void callback_AttachGeometryShader(bool bAttach )
//{
//	g_bAttachGeometryShader = bAttach;
//	if (bAttach == false)
//	{
//		GUIManager::setElementActive("Attach GS CheckBox", false);
//	}
//}


//-----------------------------------------------------------------------------
// Name: callback_AttachFragmentShader()
// Desc: Attach/detach fragment shader
//-----------------------------------------------------------------------------
//void callback_AttachFragmentShader(bool bAttach )
//{
//	g_bAttachFragmentShader = bAttach;
//	if (bAttach == false)
//	{
//		GUIManager::setElementActive("Attach FS CheckBox", false);
//	}
//}


//-----------------------------------------------------------------------------
// Name: callback_UseShaders()
// Desc: Use programmable-pipeline on/off
//-----------------------------------------------------------------------------
//void callback_UseShaders(bool bShaders)	
//{
//	g_bUseShaders = (g_Program == 0) ? false : bShaders;
//	GUIManager::setElementActive("UseShaders CheckBox", g_bUseShaders);
//}


//-----------------------------------------------------------------------------
// Name: initGUI()
// Desc: Initializes GUI
//-----------------------------------------------------------------------------
void initGUI()
{
	//GUICheckBox*	pcbxUseShaders		= new GUICheckBox	(10,  15, 40, 10, "use pragrammable pipeline", "UseShaders CheckBox");
	//GUICheckBox*	pcbxVertexShader	= new GUICheckBox	(10,  30, 40, 10, "attach vertex shader"	 , "Attach VS CheckBox"	);
	//GUICheckBox*	pcbxGeometryShader	= new GUICheckBox	(10,  45, 40, 10, "attach geometry shader"	 , "Attach GS CheckBox"	);
	//GUICheckBox*	pcbxFragmentShader	= new GUICheckBox	(10,  60, 40, 10, "attach fragment shader"	 , "Attach FS CheckBox"	);
	//GUIPushButton*	pbtnCompile			= new GUIPushButton	(10,  75,130, 13, "compile program"			 , "Compile CheckBox"	);
	fpsLabel							= new GUILabel		("fps", 10, 10,  "fpsLabel");
	GUICheckBox*	pcbxWireMode		= new GUICheckBox	(10, 100, 40, 10, "wire-mode"				 , "Wire-Mode CheckBox"	);
	GUICheckBox*	pcbxRotate			= new GUICheckBox	(10, 116,40, 10, "rotate"					 , "Rotate CheckBox"	);

	GUISlider*		psldVariable_A		= new GUISlider		(0, 2, 10, 135, 250, 6, "wind strength"		, "wind strength"	);
	GUISlider*		psldVarWA0			= new GUISlider		(0, 5, 10, 150, 250, 6, "wood amplitude 0"	, "wood amplitude 0"	);
	GUISlider*		psldVarWA1			= new GUISlider		(0, 5, 10, 165, 250, 6, "wood amplitude 1"	, "wood amplitude 1"	);
	GUISlider*		psldVarWA2			= new GUISlider		(0, 5, 10, 180, 250, 6, "wood amplitude 2"	, "wood amplitude 2"	);
	GUISlider*		psldVarWA3			= new GUISlider		(0, 5, 10, 195, 250, 6, "wood amplitude 3"	, "wood amplitude 3"	);

	GUISlider*		psldVarWF0			= new GUISlider		(0, 10, 10, 210, 250, 6, "wood frequency 0"	, "wood frequency 0"	);
	GUISlider*		psldVarWF1			= new GUISlider		(0, 10, 10, 225, 250, 6, "wood frequency 1"	, "wood frequency 1"	);
	GUISlider*		psldVarWF2			= new GUISlider		(0, 10, 10, 240, 250, 6, "wood frequency 2"	, "wood frequency 2"	);
	GUISlider*		psldVarWF3			= new GUISlider		(0, 10, 10, 255, 250, 6, "wood frequency 3"	, "wood frequency 3"	);


	GUISlider*		psldVarLA			= new GUISlider		(0, 10, 10, 275, 250, 6, "leaf amplitude"	, "leaf amplitude"	);
	GUISlider*		psldVarLF			= new GUISlider		(0, 10, 10, 290, 250, 6, "leaf frequency"	, "leaf frequency"	);

	//pcbxUseShaders->setCallBackFunc(callback_UseShaders);
//	pcbxUseShaders->setActive(true);
	//pcbxVertexShader->setCallBackFunc(callback_AttachVertexShader);
	//pcbxGeometryShader->setCallBackFunc(callback_AttachGeometryShader);
	//pcbxFragmentShader->setCallBackFunc(callback_AttachFragmentShader);
	//pbtnCompile->setCallBackFunc(callback_CompileShaders);
	pcbxWireMode->setCallBackFunc(callback_EnableWireMode);
	pcbxRotate->setCallBackFunc(callback_RotateModel);
	psldVariable_A->setCallBackFunc(clbk_WSchanged);


	psldVarWA0->setCallBackFunc(clbk_WA0changed);
	psldVarWA1->setCallBackFunc(clbk_WA1changed);
	psldVarWA2->setCallBackFunc(clbk_WA2changed);
	psldVarWA3->setCallBackFunc(clbk_WA3changed);

	psldVarWF0->setCallBackFunc(clbk_WF0changed);
	psldVarWF1->setCallBackFunc(clbk_WF1changed);
	psldVarWF2->setCallBackFunc(clbk_WF2changed);
	psldVarWF3->setCallBackFunc(clbk_WF3changed);

	psldVarLA->setCallBackFunc(clbk_LAchanged);
	psldVarLF->setCallBackFunc(clbk_LFchanged);

	//GUIManager::addElement(pcbxUseShaders);
	//GUIManager::addElement(pcbxVertexShader);
	//GUIManager::addElement(pcbxGeometryShader);
	//GUIManager::addElement(pcbxFragmentShader);
	//GUIManager::addElement(pbtnCompile);
	GUIManager::addElement(pcbxWireMode);
	GUIManager::addElement(pcbxRotate);
	GUIManager::addElement(psldVariable_A);
	GUIManager::addElement(psldVarWA0);
	GUIManager::addElement(psldVarWA1);
	GUIManager::addElement(psldVarWA2);
	GUIManager::addElement(psldVarWA3);
	GUIManager::addElement(psldVarWF0);
	GUIManager::addElement(psldVarWF1);
	GUIManager::addElement(psldVarWF2);
	GUIManager::addElement(psldVarWF3);
	GUIManager::addElement(psldVarLA);
	GUIManager::addElement(psldVarLF);
	GUIManager::addElement(fpsLabel);
	GUIManager::setColor(0.f, 0.f, 0.f, 0.f);
}

#endif