#ifndef _FOG_H
#define _FOG_H

#include "settings.h"

class Fog
{
public:
	Fog(float	_density,
		float	_start,
		float	_end,
		v4		_color);

	~Fog(void);

	void init();
	void turnOn();
	void turnOff();

	float	density;
	float	start;
	float	end;
	v4		color;
};

#endif

