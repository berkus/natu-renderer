#include "Fog.h"


Fog::Fog(float	_density,
		float	_start,
		float	_end,
		v4		_color):
		density	(_density ),
		start	(_start	  ),
		end		(_end	  ),
		color	(_color	  )
{
}


Fog::~Fog(void)
{

}
void Fog::init()
{
	glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
	glFogi(GL_FOG_MODE, GL_EXP2);			// Fog Mode
	glFogfv(GL_FOG_COLOR, color.data);		// Set Fog Color
	glFogf(GL_FOG_DENSITY, density);		// How Dense Will The Fog Be
	//glHint(GL_FOG_HINT, GL_DONT_CARE);		// Fog Hint Value
	glFogf(GL_FOG_START, start);			// Fog Start Depth
	glFogf(GL_FOG_END, end);				// Fog End Depth
}

void Fog::turnOn(){
	glEnable(GL_FOG);
	init();
}

void Fog::turnOff(){
	glDisable(GL_FOG);
}
