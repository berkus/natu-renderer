#ifndef __SIMPLE_GUI_H__
#define __SIMPLE_GUI_H__

#include <vector>
#include "../_utils/GLEE/glee.h"
#include "../_utils/GLUT/glut.h"
#include "data.h"

extern float fps;

class GUIElement;

// Vector of gui elements
using std::vector;
class GUIManager
{
	public:
		static void			addElement(GUIElement* pElement);
		static void			display();
		static void			checkMoveEvent(int x, int y);
		static void			checkClickEvent(int x, int y);
		static GUIElement*	getElement(const char* pName);
		static bool			setElementActive(const char* pName, bool bActive);
		static void			setColor(float r, float g, float b, float a);
		static void			visible(bool bVisible);
		static bool			isVisible();

	protected:
		static vector<GUIElement* > m_GUIElements;
		static bool					m_bVisible;
};


class GUIElement
{
	public:
		GUIElement(int x, int y, const char* pName = NULL);
		GUIElement(int x, int y, int width, int height, const char* pName = NULL);
		virtual ~GUIElement();

		virtual bool mouseClickEvent(int x, int y) = 0;
		virtual bool mouseMoveEvent (int x, int y);
		virtual void render(bool bSetColor = true) = 0;

		inline void setColor(float r, float g, float b, float a);

		void			   setName(const char* pName);
		inline const char* getName()	const	{ return m_pName;		}

		inline int getX()				const	{ return m_X;			}
		inline int getY()				const	{ return m_Y;			}
		inline int getWidth()			const	{ return m_Width;		}
		inline int getHeight()			const	{ return m_Height;		}

		bool isActive()					const	{ return m_bActive;		}
		void setActive(bool active)				{ m_bActive = active;	}
		void deactive()							{ m_bActive = false;	}

		virtual bool isInside(int x, int y) const
		{
			return (x >= m_X && x <= (m_X + m_Width)) && (y >= m_Y && y <= (m_Y + m_Height));
		}

	protected:
		int		m_X, m_Y;
		int		m_Width, m_Height;
		float	m_Color[4];
		bool	m_bMouseOver;
		bool	m_bActive;

		char*	m_pName;

}; // GUIElement




class GUILabel : public GUIElement
{
	public:
		GUILabel(const char* pTitle = 0, int x = 0, int y = 0, const char* pName = NULL);
	    virtual ~GUILabel();

		void render(bool bSetColor = true);
		virtual bool mouseClickEvent(int x, int y);

		inline const char*	 getText() const 
		{ 
			return m_pTitle; 
		}

		void setText(char * label);
		void setCallBackFunc(void (*pCallBackFunc)());

	protected:
		char*	m_pTitle;
		int		m_Font;

		void	(*m_pCallBackFunc)();
};  // GUILabel



class GUICheckBox : public GUIElement
{
	public:
		GUICheckBox(int x, int y, int width, int height, const char* pText, const char* pName = NULL);
	    virtual ~GUICheckBox();

		virtual void render(bool bSetColor = true);

		void setCallBackFunc(void (*pCallBackFunc)(bool));
		virtual bool mouseClickEvent(int x, int y);

	protected:
		GUILabel* m_plblTitle;

		void	(*m_pCallBackFunc)(bool);

}; // GUICheckBox





class GUIPushButton : public GUIElement
{
	public:
		GUIPushButton(int x, int y, int width, int height, const char* pText, const char* pName = NULL);
	    virtual ~GUIPushButton();

		virtual void render(bool bSetColor = true);
		void setCallBackFunc(void (*pCallBackFunc)());
		virtual bool mouseClickEvent(int x, int y);

	protected:
		GUILabel* m_plblTitle;

		void	(*m_pCallBackFunc)();

}; // GUIPushButton


class GUISlider : public GUIElement
{
	public:
		GUISlider(int min, int max, int x, int y, int width, int height, const char* text, const char* pName = NULL);
		virtual ~GUISlider();

		virtual void render(bool bSetColor = true);

		inline int  getValue() const { return m_CurrentValue; }

		void setValue	(int value);
		void setMinimum(int minimum);
		void setMaximum(int maximum);
		void setCallBackFunc(void (*pCallBackFunc)(float));

		virtual bool mouseClickEvent(int x, int y);

		virtual bool isInside(int x, int y) const
		{
			return (x >= m_ProgressBarX && x <= (m_ProgressBarX + m_ProgressBarWidth)) && 
					 (y >= m_ProgressBarY && y <= (m_ProgressBarY + m_ProgressBarHeight));
		}

	private:
		int m_MinimumValue;
		int m_MaximumValue;
		int m_Range;
		float m_CurrentValue;
		int m_CurrentPosition;

		int m_ProgressBarX	 , m_ProgressBarY;
		int m_ProgressBarWidth, m_ProgressBarHeight;

		GUILabel* m_plblText;

		void (*m_pCallBackFunc)(float);

}; // GUISlider


#endif // __SIMPLE_GUI_H__