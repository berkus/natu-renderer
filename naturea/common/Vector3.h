//-----------------------------------------------------------------------------
//  [GSY] Utilities - Simple 3D vector class
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//-----------------------------------------------------------------------------


#ifndef __VECTOR3_H__
#define __VECTOR3_H__
#include <math.h>
#include "../common/akMath.h"

#include <stdlib.h>
#include <cstdio>

union flint{
	float			f;
	int				i;
	unsigned char	ch[4];	
};

inline double DEGREES_TO_RADIANS(double deg)
{
	return deg * (PI / 180.0);
}

inline double RADIANS_TO_DEGREES(double rad)
{
	return rad * (180.0 / PI);
}

inline float randomFromRange(float _min, float _max)
{
	const float random01 = rand() / float(RAND_MAX);
	return _min + (_max - _min)*random01;
}


class Vector3
{
public:
	inline static const Vector3& zero()  { static Vector3 v(0.0f, 0.0f, 0.0f); return v; }
	inline static const Vector3& unit()  { static Vector3 v(1.0f, 1.0f, 1.0f); return v; }
	inline static const Vector3& unitX() { static Vector3 v(1.0f, 0.0f, 0.0f); return v; }
	inline static const Vector3& unitY() { static Vector3 v(0.0f, 1.0f, 0.0f); return v; }
	inline static const Vector3& unitZ() { static Vector3 v(0.0f, 0.0f, 1.0f); return v; }
	inline static		Vector3  randomFromRange(float _min, float _max)
	{
		return Vector3(	::randomFromRange(_min,_max),
			::randomFromRange(_min,_max),
			::randomFromRange(_min,_max));
	}

public:
	Vector3()
	{
		x = y = z = 0;
	}
	Vector3(float _x)
	{
		x = _x;
		y = _x;
		z = _x;
	}
	Vector3(float _x, float _y, float _z = 0)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3(const Vector3& copy)
	{
		x = copy.x;
		y = copy.y;
		z = copy.z;
	}

	Vector3(const float *arr)
	{
		x = arr[0];
		y = arr[1];
		z = arr[2];
	}


	inline operator const float*() const	{ return &x;	}
	inline operator float*()				{ return &x;	}

	//		inline float x() const					{ return x;	}
	//		inline float y() const					{ return y;	}
	//		inline float z() const					{ return z;	}

	inline Vector3& operator = (const Vector3& copy)
	{
		x = copy.x;
		y = copy.y;
		z = copy.z;
		return *this;
	}

	inline bool operator == (const Vector3& vec)
	{
		return (x == vec.x) && (y == vec.y) && (z == vec.z);
	}

	inline bool operator != (const Vector3& vec)
	{
		return !(operator==(vec));
	}

	inline Vector3& operator += (const Vector3& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;

		return *this;
	}

	inline Vector3& operator -= (const Vector3& vec) 
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;

		return *this;
	}

	inline Vector3 operator + (const float value) const
	{
		return Vector3(x + value, y + value, z + value);
	}

	inline Vector3 operator - (const float value) const
	{
		return operator+(-value);
	}

	inline Vector3 operator + (const Vector3& vec) const
	{
		return Vector3(x + vec.x, y + vec.y, z + vec.z);
	}

	inline Vector3 operator - (const Vector3& vec) const
	{
		return Vector3(x - vec.x, y - vec.y, z - vec.z);
	}

	inline Vector3 operator * (float scalar) const
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}

	inline Vector3 operator / (float scalar) const
	{
		return Vector3(x / scalar, y / scalar, z / scalar);
	}

	inline Vector3 operator - () const
	{
		return Vector3(-x, -y, -z);
	}

	inline void set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	inline float distanceTo(const Vector3& vec)
	{
			Vector3 d = vec - *this;
			return sqrt(d.x*d.x+d.y*d.y+d.z*d.z);
	}
	inline float length() const
	{
		return static_cast<float>(sqrt(x*x + y*y + z*z));
	}

	inline void normalize()
	{
		const float len = length();
		if (len != 0)
		{
			float invLength = 1.0f / len;
			x *= invLength;
			y *= invLength;
			z *= invLength;
		}
	}

	inline Vector3 getNormalized() const
	{	
		const float len = length();
		if (len != 0)
		{
			const float invLength = 1.0f / len;
			return Vector3(x * invLength, y * invLength, z * invLength);
		}
		return Vector3::zero();
	}

	inline Vector3 cross(const Vector3& vec) const
	{
		return Vector3(	y * vec.z - z * vec.y,
			z * vec.x - x * vec.z,
			x * vec.y - y * vec.x);
	}

	inline float dot(const Vector3& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	inline void rotateX(float angle) 
	{
		const float sinAngle = float(sin(angle));
		const float cosAngle = float(cos(angle));
		const float Y = y;

		y =  Y*cosAngle + z*sinAngle;
		z = -Y*sinAngle + z*cosAngle;
	}

	inline void rotateY(float angle) 
	{
		const float sinAngle = float(sin(angle));
		const float cosAngle = float(cos(angle));
		const float X = x;

		x =  X*cosAngle + z*sinAngle;
		z = -X*sinAngle + z*cosAngle;
	}

	inline void rotateZ(float angle) 
	{
		const float sinAngle = float(sin(angle));
		const float cosAngle = float(cos(angle));
		const float X = x;

		x =  X*cosAngle + y*sinAngle;
		y = -X*sinAngle + y*cosAngle;
	}

	inline Vector3 Vector3::getRotated(float angle, const Vector3& axis) const
	{
		Vector3 result;
		const float cosAngle = float(cos(angle));
		const float sinAngle = float(sin(angle));
		const float oneMinusCosAngle = 1 - cosAngle;
		const float xOMCA = oneMinusCosAngle*axis.x;
		const float yOMCA = oneMinusCosAngle*axis.y;
		const float zOMCA = oneMinusCosAngle*axis.z;
		//		axis.normalize();

		result.x = (cosAngle + xOMCA * axis.x) * x;
		result.x += (xOMCA * axis.y - axis.z * sinAngle) * y;
		result.x += (xOMCA * axis.z + axis.y * sinAngle) * z;

		result.y  = (yOMCA * axis.x + axis.z * sinAngle) * x;
		result.y += (cosAngle + yOMCA * axis.y) * y;
		result.y += (yOMCA * axis.z - axis.x * sinAngle) * z;

		result.z  = (zOMCA * axis.x - axis.y * sinAngle) * x;
		result.z += (zOMCA * axis.y + axis.x * sinAngle) * y;
		result.z += (cosAngle + zOMCA * axis.z) * z;

		return result;
	}
	inline Vector3 Vector3::getRotatedY(float angle) const
	{
		
		const float sinAngle = float(sin(angle));
		const float cosAngle = float(cos(angle));
		const float X = x;

		return Vector3(X*cosAngle + z*sinAngle, y,-X*sinAngle + z*cosAngle);
	}

	inline void Vector3::rotate(float angle, const Vector3& axis)
	{
		(*this) = getRotated(angle, axis);
	}

	inline float angleTo(const Vector3& v){
		Vector3 a = v.getNormalized();
		Vector3 b = this->getNormalized();
		return acos(a.dot(b));
	}
	inline void printOut(){
		printf("V3 [%f %f %f]\n", x,y,z);
	}
	inline void min(Vector3 &vector){
		x = min2f(x, vector.x);
		y = min2f(y, vector.y);
		z = min2f(z, vector.z);
	}
	inline void max(Vector3 &vector){
		x = max2f(x, vector.x);
		y = max2f(y, vector.y);
		z = max2f(z, vector.z);
	}

public:
	union
	{
		struct{float x, y, z;};
		struct{float r, s, t;};
		struct{float r, g, b;};
		float data[3];
	};
};
typedef Vector3 v3;

#endif __VECTOR3_H__
