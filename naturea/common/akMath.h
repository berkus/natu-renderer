#ifndef _MATH_H
#define _MATH_H

#include <time.h>
#include <cmath>
#include <cfloat>
#include <random>

const double PI			= 3.1415926536;
const double HALF_PI	= 0.5*PI;
const double TWO_PI		= 2.0*PI;
const float DEG_TO_RAD = float(PI/180.0f);
static int lastRandom = 0;


inline float randomf(float minf, float maxf){ 
	//srand(unsigned int(clock()));
	lastRandom = rand();
	return float((double(lastRandom)/RAND_MAX)*(maxf-minf)) + minf; 
}
inline int randomi(int mini, int maxi){
	return int((double(rand())/RAND_MAX)*(maxi-mini)) + mini; 
}
inline unsigned int randomui(unsigned int mini, unsigned int maxi){
	srand ( time(NULL) );
	return unsigned int((double(rand())/RAND_MAX)*(maxi-mini)) + mini; 
}

inline float min2f(float a, float b){
	return std::min(a,b);
}
inline float max2f(float a, float b){
	return std::max(a,b);
}
inline float smoothTransitionCos(float x){
	if (x<0.0){
		return 1;
	} else {
		if (x>PI){
			return -1;
		}
		return cos(x);
	}
}


#endif