#include "gui.h"

const float DEFAULT_ALPHA = 1.0f;

vector<GUIElement* >	GUIManager::m_GUIElements;
bool					GUIManager::m_bVisible = true;
		
void GUIManager::visible(bool bVisible)
{
	m_bVisible = bVisible;
}


bool GUIManager::isVisible()
{
	return m_bVisible;
}


void GUIManager::addElement(GUIElement* pElement)
{
	if (pElement != NULL)
	{
		m_GUIElements.push_back(pElement);
	}
}


GUIElement*	GUIManager::getElement(const char* pName)
{
	if (pName == NULL)
	{
		return NULL;
	}
	else
	{
		for (vector<GUIElement* >::iterator it = m_GUIElements.begin(); it < m_GUIElements.end(); it++)
		{
			if (strcmp((*it)->getName(), pName) == 0)
			{
				return (*it);
			}
		}

		return NULL;
	}
}


bool GUIManager::setElementActive(const char* pName, bool bActive)
{
	if (pName == NULL)
	{
		return false;
	}
	else
	{
		for (vector<GUIElement* >::iterator it = m_GUIElements.begin(); it < m_GUIElements.end(); it++)
		{
			if (strcmp((*it)->getName(), pName) == 0)
			{
				(*it)->setActive(bActive);
			}
		}

		return false;
	}
}


void GUIManager::checkMoveEvent(int x, int y)
{
	guiActive = false;
	for (vector<GUIElement* >::iterator it = m_GUIElements.begin(); it < m_GUIElements.end(); it++)
	{
		if ((*it)->mouseMoveEvent(x, y)){
			guiActive = true;
		}
	}
}


void GUIManager::checkClickEvent(int x, int y)
{
	if (m_bVisible)
	{
		guiActive = false;
		for (std::vector<GUIElement* >::iterator it = m_GUIElements.begin(); it < m_GUIElements.end(); it++)
		{
			if ((*it)->mouseClickEvent(x, y)){
				guiActive = true;
			}
		}
	}
}
void GUIManager::setColor(float r, float g, float b, float a){
	if (m_bVisible)
	{
		for (std::vector<GUIElement* >::iterator it = m_GUIElements.begin(); it < m_GUIElements.end(); it++)
		{
			(*it)->setColor(r,g,b,a);
				
		}
	}
}

void GUIManager::display()
{
	if (m_bVisible)
	{
		// Render gui
		GLboolean bLightOn			= glIsEnabled(GL_LIGHTING);
		GLboolean bTextureOn		= glIsEnabled(GL_TEXTURE_2D);
		GLint	  polygon_mode[3]	= {GL_FILL, GL_FILL, GL_FILL};
		
		glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);

		GLint viewport[4] = {0};
		glGetIntegerv(GL_VIEWPORT, viewport);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		for (vector<GUIElement* >::iterator it = m_GUIElements.begin(); it < m_GUIElements.end(); it++)
		{
			(*it)->render();
		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	//	glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		if (bLightOn == GL_TRUE)
		{
			glEnable(GL_LIGHTING);
		}
		if (bTextureOn == GL_TRUE)
		{
			glEnable(GL_TEXTURE_2D);
		}
		glPolygonMode(GL_FRONT_AND_BACK, polygon_mode[0]);
	}
}


// ELEMENT_________________________________________________________________________

GUIElement::GUIElement(int x, int y, const char* pName) 
	: m_X(x), m_Y(y), m_Width(0), m_Height(0), m_bMouseOver(false), m_pName(NULL)
{
	setName((pName == NULL) ? "GUIElement" : pName);
	m_Color[0] = m_Color[1] = m_Color[2] = 1.0f;
	m_Color[3] = DEFAULT_ALPHA;
}


GUIElement::GUIElement(int x, int y, int width, int height, const char* pName)
	: m_X(x), m_Y(y), m_Width(width), m_Height(height), m_bMouseOver(false), m_pName(NULL)
{
	setName((pName == NULL) ? "GUIElement" : pName);

	m_Color[0] = m_Color[1] = m_Color[2] = 1.0f;
	m_Color[3] = DEFAULT_ALPHA;
}


GUIElement::~GUIElement()
{
	if (m_pName) delete [] m_pName;
}


void GUIElement::setName(const char* pName)
{
	if (pName)
	{
		int nameLen = strlen(pName);

		if (m_pName) delete [] m_pName;
		m_pName = new char[nameLen + 1];
		strcpy(m_pName, pName);
	}
}
			

bool GUIElement::mouseMoveEvent (int x, int y)
{
	m_bMouseOver = isInside(x, y);
	return m_bMouseOver;
	
}


void GUIElement::setColor(float r, float g, float b, float a) 
{
	m_Color[0] = r;
	m_Color[1] = g;
	m_Color[2] = b;
	m_Color[3] = a;
}


// LABEL_________________________________________________________________________

GUILabel::GUILabel(const char* pTitle, int x, int y, const char* pName) 
	: GUIElement(x, y, (pName == NULL) ? "GUILabel" : pName), 
	  m_pTitle(NULL), 
	  m_Font((int)GLUT_BITMAP_HELVETICA_10)
{
	if (pTitle)
	{
		int titleLength = strlen(pTitle);
		m_pTitle = new char[titleLength + 1];
		strcpy(m_pTitle, pTitle); 
		m_Width  = titleLength*5+5;
//		m_Height = 10;
	}
	m_pCallBackFunc = NULL;
}


GUILabel::~GUILabel() 
{
	if (m_pTitle) delete [] m_pTitle;
}
void GUILabel::setText(char * label)
{
	int titleLength = strlen(label);
	m_pTitle = new char[titleLength + 1];
	strcpy(m_pTitle, label); 
	m_Width  = titleLength*5+5;
}

void GUILabel::render(bool bSetColor)
{
	if (bSetColor)
	{
		if (m_bMouseOver)
		{
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			glColor4f(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		}
	}

	glRasterPos2i(m_X, m_Y);

	char* tempPtr = m_pTitle;
	while (*tempPtr)        
	{
		glutBitmapCharacter((void *)m_Font, *tempPtr++);
	}

	if (bSetColor)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}


bool GUILabel::mouseClickEvent(int x, int y)
{
	if (isInside(x, y) && (m_pCallBackFunc != NULL))
	{
		m_pCallBackFunc();
		return true;
	}
	return false;
}


void GUILabel::setCallBackFunc(void (*pCallBackFunc)()) 
{ 
	m_pCallBackFunc = pCallBackFunc; 
}



// CHECK BOX_________________________________________________________________________

GUICheckBox::GUICheckBox(int x, int y, int width, int height, const char* pText, const char* pName)
	: GUIElement(x, y, width, height, (pName == NULL) ? "GUICheckBox" : pName), m_plblTitle(NULL)
{
	m_bActive		= false;
	m_pCallBackFunc = 0;

	m_plblTitle = new GUILabel(pText, m_X + m_Height + 5, m_Y + m_Height - 1);
	m_Width = (m_plblTitle->getWidth() != 0) ? width + 10 + m_plblTitle->getWidth() : width;
}


GUICheckBox::~GUICheckBox()
{
	if (m_plblTitle) delete m_plblTitle;
}


void GUICheckBox::render(bool bSetColor)
{
	if (bSetColor)
	{
		if (m_bMouseOver)
		{
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			glColor4f(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		}
	}

	glBegin(GL_LINE_LOOP);
		glVertex2i(m_X, m_Y);
		glVertex2i(m_X, m_Y+m_Height);
		glVertex2i(m_X+m_Height, m_Y+m_Height);
		glVertex2i(m_X+m_Height, m_Y);
	glEnd();

	if (m_bActive)
	{
		glBegin(GL_LINES);
			glVertex2i(m_X, m_Y+1);
			glVertex2i(m_X+m_Height-1, m_Y+m_Height);
			glVertex2i(m_X, m_Y+m_Height);
			glVertex2i(m_X+m_Height-1, m_Y+1);
		glEnd();
	}

	if (m_plblTitle) m_plblTitle->render(false);

	if (bSetColor)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}


void GUICheckBox::setCallBackFunc(void (*pCallBackFunc)(bool)) 
{ 
	m_pCallBackFunc = pCallBackFunc; 
}


bool GUICheckBox::mouseClickEvent(int x, int y)
{
	if (isInside(x, y))
	{
		m_bActive = !m_bActive;
				
		if (m_pCallBackFunc)
		{
			m_pCallBackFunc(m_bActive);
		}
		return true;
	}
	return false;
}



// PUSH BUTTON_________________________________________________________________________
GUIPushButton::GUIPushButton(int x, int y, int width, int height, const char* pText, const char* pName)
	: GUIElement(x, y, width, height, (pName == NULL) ? "GUIPushButton" : pName), m_plblTitle(NULL)
{
	m_pCallBackFunc = 0;
	m_plblTitle = new GUILabel(	pText, 
								m_X + m_Width/2 - 5*((pText != NULL) ? strlen(pText)/2 : 0), 
								m_Y + m_Height/2 + 4);
}


GUIPushButton::~GUIPushButton()
{
	if (m_plblTitle) delete m_plblTitle;
}



void GUIPushButton::render(bool bSetColor)
{
	if (bSetColor)
	{
		if (m_bMouseOver)
		{
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			glColor4f(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		}
	}

	glBegin(GL_LINE_LOOP);
		glVertex2i(			 m_X,			 m_Y);
		glVertex2i(			 m_X, m_Y + m_Height);
		glVertex2i(m_X + m_Width, m_Y + m_Height);
		glVertex2i(m_X + m_Width,			 m_Y);
	glEnd();

	if (m_plblTitle) m_plblTitle->render(false);

	if (bSetColor)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}


void GUIPushButton::setCallBackFunc(void (*pCallBackFunc)()) 
{ 
	m_pCallBackFunc = pCallBackFunc; 
}


bool GUIPushButton::mouseClickEvent(int x, int y)
{
	if (isInside(x, y) && m_pCallBackFunc)
	{
		m_pCallBackFunc();
		return true;
	}
	return false;
}




// SLIDER_________________________________________________________________________


GUISlider::GUISlider(int min, int max, int x, int y, int width, int height, const char* pText, const char* pName)
	: GUIElement(x, y, width, height, (pName == NULL) ? "GUISlider" : pName), 
	  m_MinimumValue(min), m_MaximumValue(max), m_plblText(NULL)
{
	m_pCallBackFunc = NULL;

	m_plblText = new GUILabel(pText, x, y + height);

	m_Range				= m_MaximumValue - m_MinimumValue;
	m_CurrentPosition	= int(m_X + 0.5*(m_Width));
	m_CurrentValue		= (m_MinimumValue + m_MaximumValue) / 2;

	m_ProgressBarX		= m_X + m_plblText->getWidth();
	m_ProgressBarY		= m_Y;
	m_ProgressBarWidth  = m_Width - (m_plblText->getWidth() + 5);
	m_ProgressBarHeight = m_Height;
}


GUISlider::~GUISlider()
{
	if (m_plblText) delete m_plblText;
}


void GUISlider::setMinimum(int minimum)
{
	float ratio = m_CurrentValue / float(m_MaximumValue - m_MinimumValue);

	m_MinimumValue		= minimum;
	m_Range				= m_MaximumValue - m_MinimumValue;
	m_CurrentValue		= int(m_MinimumValue + ratio*m_Range);
	m_CurrentPosition = m_ProgressBarX + ratio*m_ProgressBarWidth;
}


void GUISlider::setMaximum(int maximum)
{
	float ratio = m_CurrentValue / float(m_MaximumValue - m_MinimumValue);

	m_MaximumValue		= maximum;
	m_Range				= m_MaximumValue - m_MinimumValue;
	m_CurrentValue		= int(m_MinimumValue + ratio*m_Range);
	m_CurrentPosition = m_ProgressBarX + ratio*m_ProgressBarWidth;
}


void GUISlider::setValue(int value)
{
	m_CurrentValue		= value;

	const float ratio = float(m_CurrentValue - m_MinimumValue) / m_Range;
	m_CurrentPosition = m_ProgressBarX + ratio*m_ProgressBarWidth;
}


void GUISlider::render(bool bSetColor)
{
	if (bSetColor)
	{
		if (m_bMouseOver)
		{
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			glColor4f(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		}
	}

	if (m_plblText) m_plblText->render(false);

	glBegin(GL_QUADS);
		glVertex2i(m_ProgressBarX	 , m_ProgressBarY);
		glVertex2i(m_ProgressBarX	 , m_ProgressBarY + m_ProgressBarHeight);
		glVertex2i(m_CurrentPosition, m_ProgressBarY + m_ProgressBarHeight);
		glVertex2i(m_CurrentPosition, m_ProgressBarY);
	glEnd();

	glBegin(GL_LINE_LOOP);
		glVertex2i(m_ProgressBarX					  , m_ProgressBarY);
		glVertex2i(m_ProgressBarX					  , m_ProgressBarY + m_ProgressBarHeight);
		glVertex2i(m_ProgressBarX + m_ProgressBarWidth, m_ProgressBarY + m_ProgressBarHeight);
		glVertex2i(m_ProgressBarX + m_ProgressBarWidth, m_ProgressBarY);
	glEnd();

	if (bSetColor)
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
}


bool GUISlider::mouseClickEvent(int x, int y)
{
	if (isInside(x, y))
	{
		const float ratio = float(x - m_ProgressBarX) / float (m_ProgressBarWidth);
		m_CurrentValue    = float(m_MinimumValue + ratio*m_Range);
		m_CurrentPosition = m_ProgressBarX + ratio*m_ProgressBarWidth;
		
		if (m_pCallBackFunc)
		{
			//m_pCallBackFunc(m_CurrentValue / float(abs(m_MaximumValue - m_MinimumValue)));
			m_pCallBackFunc(m_CurrentValue);
		}
		return true;
	}
	return false;
}


void GUISlider::setCallBackFunc(void (*pCallBackFunc)(float)) 
{ 
	m_pCallBackFunc = pCallBackFunc;
}
