//-----------------------------------------------------------------------------
//  [GSY] Utilities - Simple 4x4 matrix class
//  27/02/2008
//-----------------------------------------------------------------------------
//  Controls: 
//-----------------------------------------------------------------------------

#ifndef __MATRIX4X4_H__
#define __MATRIX4X4_H__


#include "Vector4.h"
#include <assert.h>
#include <cstdio>
using namespace std;

class Matrix4x4
{
	public:

		Matrix4x4()
		{
			setIdentity();
		}

		Matrix4x4(const float* pMatrix)
		{
			memcpy(m, pMatrix, sizeof(float)*16);
		}

		Matrix4x4(float  m0, float  m1, float  m2, float  m3,
				  float  m4, float  m5, float  m6, float  m7,
				  float  m8, float  m9, float m10, float m11,
				  float m12, float m13, float m14, float m15)
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
			m[ 9] =  m9;
			m[10] = m10;
			m[11] = m11;
			m[12] = m12;
			m[13] = m13;
			m[14] = m14;
			m[15] = m15;
		}
	
		explicit Matrix4x4(const Matrix4x4& copy)
		{
			for (int i = 0; i < 16; i++)
			{
				m[i] = copy.m[i];
			}
		}
		inline const void printOut()
		{
			printf("MATRIX:\n");
				printf("\t %f %f %f %f\n", m[0], m[4], m[8] , m[12]);
				printf("\t %f %f %f %f\n", m[1], m[5], m[9] , m[13]);
				printf("\t %f %f %f %f\n", m[2], m[6], m[10], m[14]);
				printf("\t %f %f %f %f\n", m[3], m[7], m[11], m[15]);
		}

		inline void scale(v3 &scaleVector)
		{
			m[0]  *= scaleVector.x;
			m[5]  *= scaleVector.y;
			m[10] *= scaleVector.z;
		}

		inline void translate(v3 &moveVector)
		{
			m[12] += moveVector.x;
			m[13] += moveVector.y;				
			m[14] += moveVector.z;
		}


		inline void rotate(v3 &axis, float angleRad)
		{
			float sina = sin(angleRad);
			float cosa = cos(angleRad);
			float omcosa= 1.f-cosa;
			float  m0 = cosa+omcosa*axis.x*axis.x;
			float  m1 = omcosa*axis.x*axis.y + axis.z*sina;
			float  m2 = omcosa*axis.x*axis.z - axis.y*sina;;
			float  m3 = 0.f;
			float  m4 = omcosa*axis.x*axis.y - axis.z*sina;
			float  m5 = cosa + omcosa*axis.y*axis.y;
			float  m6 = omcosa*axis.y*axis.z + axis.x*sina;
			float  m7 = 0.f;
			float  m8 = omcosa*axis.x*axis.z + axis.y*sina;
			float  m9 = omcosa*axis.y*axis.z - axis.x*sina;;
			float m10 = cosa + omcosa*axis.z*axis.z;
			float m11 = 0.f;
			float m12 = 0.f;
			float m13 = 0.f;
			float m14 = 0.f;
			float m15 = 1.f;
			Matrix4x4 rot(m0,  m1,  m2,  m3,
				          m4,  m5,  m6,  m7,
				          m8,  m9, m10, m11,
				         m12, m13, m14, m15);
			(*this) = rot * (*this);
		}


		inline operator const float*() const	{ return m;	}
		inline operator float*()				{ return m;	}


		Matrix4x4& operator *= (const Matrix4x4& mat)
		{
			(*this) = (*this) * mat;
			return *this;
		}

		Matrix4x4 operator * (const Matrix4x4& mat) const
		{

			return Matrix4x4(
				// 1st row
				m[ 0] * mat.m[ 0] + m[ 4] * mat.m[ 1] +	m[ 8] * mat.m[ 2] + m[12] * mat.m[ 3],
				m[ 1] * mat.m[ 0] + m[ 5] * mat.m[ 1] + m[ 9] * mat.m[ 2] + m[13] * mat.m[ 3],
				m[ 2] * mat.m[ 0] + m[ 6] * mat.m[ 1] + m[10] * mat.m[ 2] + m[14] * mat.m[ 3],
				m[ 3] * mat.m[ 0] +	m[ 7] * mat.m[ 1] +	m[11] * mat.m[ 2] +	m[15] * mat.m[ 3],

				// 2nd row
				m[ 0] * mat.m[ 4] +	m[ 4] * mat.m[ 5] +	m[ 8] * mat.m[ 6] +	m[12] * mat.m[ 7],
				m[ 1] * mat.m[ 4] +	m[ 5] * mat.m[ 5] +	m[ 9] * mat.m[ 6] +	m[13] * mat.m[ 7],
				m[ 2] * mat.m[ 4] +	m[ 6] * mat.m[ 5] +	m[10] * mat.m[ 6] +	m[14] * mat.m[ 7],
				m[ 3] * mat.m[ 4] +	m[ 7] * mat.m[ 5] +	m[11] * mat.m[ 6] +	m[15] * mat.m[ 7],
					
				// 3rd row
				m[ 0] * mat.m[ 8] +	m[ 4] * mat.m[ 9] +	m[ 8] * mat.m[10] +	m[12] * mat.m[11],
				m[ 1] * mat.m[ 8] +	m[ 5] * mat.m[ 9] +	m[ 9] * mat.m[10] +	m[13] * mat.m[11],
				m[ 2] * mat.m[ 8] +	m[ 6] * mat.m[ 9] +	m[10] * mat.m[10] +	m[14] * mat.m[11],
				m[ 3] * mat.m[ 8] +	m[ 7] * mat.m[ 9] +	m[11] * mat.m[10] +	m[15] * mat.m[11],
					
				// 4th row
				m[ 0] * mat.m[12] +	m[ 4] * mat.m[13] +	m[ 8] * mat.m[14] +	m[12] * mat.m[15],
				m[ 1] * mat.m[12] +	m[ 5] * mat.m[13] +	m[ 9] * mat.m[14] +	m[13] * mat.m[15],
				m[ 2] * mat.m[12] +	m[ 6] * mat.m[13] +	m[10] * mat.m[14] +	m[14] * mat.m[15],
				m[ 3] * mat.m[12] +	m[ 7] * mat.m[13] +	m[11] * mat.m[14] +	m[15] * mat.m[15]);
		}


		Vector4 operator * (const Vector4& vertex) const
		{
			return Vector4(
				m[ 0]*vertex.x + m[ 4]*vertex.y + m[ 8]*vertex.z + m[12]*vertex.w,
				m[ 1]*vertex.x + m[ 5]*vertex.y + m[ 9]*vertex.z + m[13]*vertex.w,
				m[ 2]*vertex.x + m[ 6]*vertex.y + m[10]*vertex.z + m[14]*vertex.w,
				m[ 3]*vertex.x + m[ 7]*vertex.y + m[11]*vertex.z + m[15]*vertex.w);
		}

		Vector4 operator * (const Vector3& vertex) const
		{
			return Vector4(
				m[ 0]*vertex.x + m[ 4]*vertex.y + m[ 8]*vertex.z + m[12],
				m[ 1]*vertex.x + m[ 5]*vertex.y + m[ 9]*vertex.z + m[13],
				m[ 2]*vertex.x + m[ 6]*vertex.y + m[10]*vertex.z + m[14],
				m[ 3]*vertex.x + m[ 7]*vertex.y + m[11]*vertex.z + m[15]);
		}

		bool operator == (const Matrix4x4& mat) const
		{
			for(int i = 0; i < 16; i++)
			{
				if(m[i] != mat.m[i])
				{
					return false;
				}
			}
			
			return true;
		}


		template<class TT> bool	operator != (const Matrix4x4& mat) const
		{
			return !operator==(mat);
		}

		
		inline void setIdentity()
		{
			memset(m, 0, sizeof(float)*16);
			m[0] = m[5] = m[10] = m[15] = 1.0f;
		}
	
		inline void setEntry(int index, float value)
		{
			if ((index < 16) && (index >= 0))
			{
				m[index] = value;
			}
		}
		
		inline float getEntry(int index) const
		{
			assert(index >= 0 && index < 16);

			return m[index];
		}


		inline float getDeterminant() const
		{
			const float d0 = (m[0] == 0) ? 
				0.0f :
				m[0]*(m[ 5]*(m[10]*m[15] - m[11]*m[14]) + m[ 6]*(m[11]*m[13] - m[ 9]*m[15]) + m[ 7]*(m[ 9]*m[14] - m[10]*m[13]));

			const  float d1 = (m[1] == 0) ? 
				0.0f :
				m[1]*(m[ 4]*(m[10]*m[15] - m[11]*m[14]) + m[ 6]*(m[11]*m[12] - m[ 8]*m[15]) + m[ 7]*(m[ 8]*m[14] - m[10]*m[12]));

			const float d2 = (m[2] == 0) ?
				0.0f :
				m[2]*(m[ 4]*(m[ 9]*m[15] - m[11]*m[13]) + m[ 5]*(m[11]*m[12] - m[ 8]*m[15]) + m[ 7]*(m[ 8]*m[13] - m[ 9]*m[12]));

			const float d3 = (m[3] == 0) ?
				0.0f :
				m[3]*(m[ 4]*(m[ 9]*m[14] - m[10]*m[13]) + m[ 5]*(m[10]*m[12] - m[ 8]*m[14]) + m[ 6]*(m[ 8]*m[13] - m[ 9]*m[12]));

			return d0 - d1 + d2 - d3;
		}


		inline Matrix4x4 getTranspose(void) const
		{
			return Matrix4x4(m[ 0], m[ 4], m[ 8], m[12],
							 m[ 1], m[ 5], m[ 9], m[13],
							 m[ 2], m[ 6], m[10], m[14],
							 m[ 3], m[ 7], m[11], m[15]);
		}


		inline Matrix4x4 getInverse(void) const
		{
			Matrix4x4 matResult = *this;
			matResult.invert();
			return matResult;
		}

		/*inline Matrix4x4 inverse(void) const
		{
			Matrix4x4 matResult = *this;
			matResult.invert();
			return matResult;
		}*/

		inline void transpose(void)
		{
			const float m1  = m[ 1];
			const float m2  = m[ 2];
			const float m3  = m[ 3];
			const float m6  = m[ 6];
			const float m7  = m[ 7];
			const float m11 = m[11];

			m[ 1] = m[ 4]; 
			m[ 2] = m[ 8];
			m[ 3] = m[12];
			m[ 4] = m1; 
			m[ 6] = m[ 9];
			m[ 7] = m[13];
			m[ 8] = m2;
			m[ 9] = m6;
			m[11] = m[14];
			m[12] = m3;
			m[13] = m7;
			m[14] = m11;
		}


		void invert(void)
		{
			float old[16];
			float det = getDeterminant();

			assert(det != 0);
			memcpy(old, m, 16*sizeof(float));

			const float m0005 = m[ 0] * m[ 5];
			const float m0006 = m[ 0] * m[ 6];
			const float m0007 = m[ 0] * m[ 7];
			const float m0104 = m[ 1] * m[ 4];
			const float m0106 = m[ 1] * m[ 6];
			const float m0107 = m[ 1] * m[ 7];
			const float m0204 = m[ 2] * m[ 4];
			const float m0205 = m[ 2] * m[ 5];
			const float m0207 = m[ 2] * m[ 7];
			const float m0304 = m[ 3] * m[ 4];
			const float m0305 = m[ 3] * m[ 5];
			const float m0306 = m[ 3] * m[ 6];
			const float m0813 = m[ 8] * m[13];
			const float m0814 = m[ 8] * m[14];
			const float m0815 = m[ 8] * m[15];
			const float m0912 = m[ 9] * m[12];
			const float m0914 = m[ 9] * m[14];
			const float m0915 = m[ 9] * m[15];
			const float m1012 = m[10] * m[12];
			const float m1013 = m[10] * m[13];
			const float m1015 = m[10] * m[15];
			const float m1112 = m[11] * m[12];
			const float m1113 = m[11] * m[13];
			const float m1114 = m[11] * m[14];

			const float invDet = 1.0f/det;

			m[ 0] = (old[ 5]*(m1015 - m1114) + old[ 6]*(m1113 - m0915) + old[ 7]*(m0914 - m1013)) * invDet;
			m[ 1] = (old[ 1]*(m1114 - m1015) + old[ 2]*(m0915 - m1113) + old[ 3]*(m1013 - m0914)) * invDet;
			m[ 2] = (old[13]*(m0207 - m0306) + old[14]*(m0305 - m0107) + old[15]*(m0106 - m0205)) * invDet;
			m[ 3] = (old[10]*(m0107 - m0305) + old[11]*(m0205 - m0106) + old[ 9]*(m0306 - m0207)) * invDet;
			m[ 4] = (old[ 4]*(m1114 - m1015) + old[ 6]*(m0815 - m1112) + old[ 7]*(m1012 - m0814)) * invDet;
			m[ 5] = (old[ 0]*(m1015 - m1114) + old[ 2]*(m1112 - m0815) + old[ 3]*(m0814 - m1012)) * invDet;
			m[ 6] = (old[12]*(m0306 - m0207) + old[14]*(m0007 - m0304) + old[15]*(m0204 - m0006)) * invDet;
			m[ 7] = (old[11]*(m0006 - m0204) + old[ 8]*(m0207 - m0306) + old[10]*(m0304 - m0007)) * invDet;
			m[ 8] = (old[ 4]*(m0915 - m1113) + old[ 5]*(m1112 - m0815) + old[ 7]*(m0813 - m0912)) * invDet;
			m[ 9] = (old[ 0]*(m1113 - m0915) + old[ 1]*(m0815 - m1112) + old[ 3]*(m0912 - m0813)) * invDet;
			m[10] = (old[12]*(m0107 - m0305) + old[13]*(m0304 - m0007) + old[15]*(m0005 - m0104)) * invDet;
			m[11] = (old[ 9]*(m0007 - m0304) + old[11]*(m0104 - m0005) + old[ 8]*(m0305 - m0107)) * invDet;
			m[12] = (old[ 4]*(m1013 - m0914) + old[ 5]*(m0814 - m1012) + old[ 6]*(m0912 - m0813)) * invDet;
			m[13] = (old[ 0]*(m0914 - m1013) + old[ 1]*(m1012 - m0814) + old[ 2]*(m0813 - m0912)) * invDet;
			m[14] = (old[12]*(m0205 - m0106) + old[13]*(m0006 - m0204) + old[14]*(m0104 - m0005)) * invDet;
			m[15] = (old[10]*(m0005 - m0104) + old[ 8]*(m0106 - m0205) + old[ 9]*(m0204 - m0006)) * invDet;
		}


//		protected:
			//! matrix
			float m[16];

	};
typedef Matrix4x4 Matrix4;
typedef Matrix4x4 m4;
#endif //__MATRIX4X4_H__



