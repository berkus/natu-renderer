//-----------------------------------------------------------------------------
//  [GSY] Utilities - Simple 4x4 matrix class
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//-----------------------------------------------------------------------------

#ifndef __MATRIX3X3_H__
#define __MATRIX3X3_H__


#include "Vector3.h"
#include <assert.h>


class Matrix3x3
{
	public:
		Matrix3x3()
		{
			setIdentity();
		}

		Matrix3x3(const float* pMatrix)
		{
			memcpy(m, pMatrix, sizeof(float)*9);
		}
		Matrix3x3(const Vector3 &a, const Vector3 &b, const Vector3 &c){
			m[0] = a.x; m[1] = b.x; m[2] = c.x;
			m[3] = a.y; m[4] = b.y; m[5] = c.y;
			m[6] = a.z; m[7] = b.z; m[8] = c.z;
		}
		Matrix3x3(float  m0, float  m1, float  m2,
				  float  m3, float  m4, float  m5,
				  float  m6, float  m7, float  m8)
		{
			m[ 0] =  m0;
			m[ 1] =  m1;
			m[ 2] =  m2;
			m[ 3] =  m3;
			m[ 4] =  m4;
			m[ 5] =  m5;
			m[ 6] =  m6;
			m[ 7] =  m7;
			m[ 8] =  m8;
		}
		Matrix3x3(const Matrix4x4& copy)
		{
			m[ 0] =  copy.m[0];
			m[ 1] =  copy.m[1];
			m[ 2] =  copy.m[2];
			m[ 3] =  copy.m[4];
			m[ 4] =  copy.m[5];
			m[ 5] =  copy.m[6];
			m[ 6] =  copy.m[8];
			m[ 7] =  copy.m[9];
			m[ 8] =  copy.m[10];
		}
		explicit Matrix3x3(const Matrix3x3& copy)
		{
			for (int i = 0; i < 9; i++)
			{
				m[i] = copy.m[i];
			}
		}

		inline operator const float*() const	{ return m;	}
		inline operator float*()				{ return m;	}

		Vector3 operator * (const Vector3& vertex) const
		{
			return Vector3(
				m[ 0]*vertex.x + m[ 3]*vertex.y + m[ 6]*vertex.z,
				m[ 1]*vertex.x + m[ 4]*vertex.y + m[ 7]*vertex.z,
				m[ 2]*vertex.x + m[ 5]*vertex.y + m[ 8]*vertex.z);
		}

		bool operator == (const Matrix3x3& mat) const
		{
			for(int i = 0; i < 9; i++)
			{
				if(m[i] != mat.m[i])
				{
					return false;
				}
			}
			
			return true;
		}


		template<class TT> bool	operator != (const Matrix3x3& mat) const
		{
			return !operator==(mat);
		}

		
		inline void setIdentity()
		{
			memset(m, 0, sizeof(float)*9);
			m[0] = m[4] = m[8] = 1.0f;
		}
	


		inline Matrix3x3 getTranspose(void) const
		{
			return Matrix3x3(m[ 0], m[ 3], m[ 6],
							 m[ 1], m[ 4], m[ 7],
							 m[ 2], m[ 5], m[ 8]);
		}



		

//		protected:
			//! matrix
			float m[9];

	};
typedef Matrix3x3 Matrix3;
typedef Matrix3x3 m3;
#endif //__MATRIX4X4_H__



