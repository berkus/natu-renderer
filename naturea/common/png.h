#ifndef _PNG_H
#define _PNG_H
#include "LODEpng/lodepng.h"

#include "Vector4.h"

#include <vector>
#include <string>
using namespace std;

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

	void create(int width, int height, unsigned char *_im = NULL);
	
	// Save current image in file [true = success, false otherwise]
	bool save(string pathname) const;

	// Load image from file [true = success, false otherwise]
	bool load(string pathname);
	
	// access pixel data at x,y
	flint getPixel(const int x, const int y) const;
	int   getPixelRaw(const int x, const int y) const;

	// set pixel data at x,y
	void setPixel(const int x, const int y, float value);

	// set pixel data at x,y
	void setPixel(const int x, const int y, flint value);

	void clear();
	int width, height;
	unsigned char * getData();

	float * getFloatData();
private:
	vector<unsigned char> image;
	unsigned char * im;
};


#endif