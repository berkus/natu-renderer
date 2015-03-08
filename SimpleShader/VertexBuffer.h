#ifndef _VERTEXBUFFER_H
#define _VERTEXBUFFER_H

#include <vector>
#include "../_utils/Vector3.h"

using namespace std;
// max count of texture coords
static const int MAX_TEXCOORDS = 8;

struct VertexFormat
		{
			int		mElementsForPosition;				///< Coordinates for position data for each vertex. Usually 3.
			int		mElementsForNormal;					///< Coordinates for normal data for each vertex. Usually 0 or 3.
			int		mElementsForColor;					///< Coordinates for color data for each vertex.
			int		mElementsForTexCoord[MAX_TEXCOORDS];	///< Texture coordinates for each texture coordinates set for each vertex. 
			int		mElementsPerVertex;					///< Total number of coordinates per vertex.

			bool	mHasIndices;						///< If true, then this is an indexed vertex buffer.
		};


class VertexBuffer
{
public:
	VertexBuffer(void);
	~VertexBuffer(void);
	void * pData;
	void * pIndices;
	// positions

	// normals

	// tangents

	// binormals

	// textureCoords
};

#endif
