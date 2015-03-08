#ifndef _PNG_H
#define _PNG_H
#include "../_utils/LODEpng/lodepng.h"

#include "../_utils/Vector4.h"

#include <vector>
#include <string>
using namespace std;

union flint{
	float			f;
	int				i;
	unsigned char	ch[4];	
};

/**
* PNG handles all work with png images used as textures...
* Adam Kucera
**/
class PNG {

public:
	// Constructor
	PNG();
	PNG(int _width, int _height);

	// Destructor
	~PNG();

	void create(int width, int height);

	// Save current image in file [true = success, false otherwise]
	bool save(string pathname) const;

	// Load image from file [true = success, false otherwise]
	bool load(string pathname);
	
	// access pixel data at x,y
	flint getPixel(const int x, const int y) const;

	// set pixel data at x,y
	void setPixel(const int x, const int y, float value);

	// set pixel data at x,y
	void setPixel(const int x, const int y, flint value);

	int width, height;
	char * getData();
private:
	std::vector<unsigned char> image;

	


};


#endif