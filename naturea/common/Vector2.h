#ifndef _VECTOR2_H
#define _VECTOR2_H

#include "../common/akMath.h"
#include <math.h>
#include <stdlib.h>
#include <cstdio>
#include "settings.h"

class Vector2
{
public:
	Vector2(){
		x	= 0.f;
		y	= 0.f;
	}
	Vector2(float val){
		x	= val;
		y	= val;
	}

	Vector2(float _x, float _y):x(_x),y(_y){}
	~Vector2(){}

	Vector2(const Vector2& copy)
	{
		x = copy.x;
		y = copy.y;
	}
	Vector2(const GLfloat *arr)
	{
		x = arr[0];
		y = arr[1];
	}

	inline float length(){
		return sqrt(x*x+y*y);
	}

	inline operator const float*() const	{ return &x;	}
	inline operator float*()				{ return &x;	}

	//		inline float x() const					{ return x;	}
	//		inline float y() const					{ return y;	}
	//		inline float z() const					{ return z;	}

	inline Vector2& operator = (const Vector2& copy)
	{
		x = copy.x;
		y = copy.y;
		return *this;
	}

	inline bool operator == (const Vector2& vec)
	{
		return (x == vec.x) && (y == vec.y);
	}

	inline bool operator != (const Vector2& vec)
	{
		return !(operator==(vec));
	}

	inline Vector2& operator += (const Vector2& vec)
	{
		x += vec.x;
		y += vec.y;
		return *this;
	}

	inline Vector2& operator -= (const Vector2& vec) 
	{
		x -= vec.x;
		y -= vec.y;
		
		return *this;
	}

	inline Vector2 operator + (const float value) const
	{
		return Vector2(x + value, y + value);
	}

	inline Vector2 operator - (const float value) const
	{
		return operator+(-value);
	}

	inline Vector2 operator + (const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}

	inline Vector2 operator - (const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}

	inline Vector2 operator * (float scalar) const
	{
		return Vector2(x * scalar, y * scalar);
	}

	inline Vector2 operator / (float scalar) const
	{
		return Vector2(x / scalar, y / scalar);
	}

	inline Vector2 operator - () const
	{
		return Vector2(-x, -y);
	}

public:
	union
	{
		struct{float x, y;};
		struct{float r, s;};
		struct{float r, g;};
		float data[2];
	};
};

typedef Vector2 v2;
#endif