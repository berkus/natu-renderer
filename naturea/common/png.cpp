#include "png.h"

// Constructor
PNG::PNG(){
	width	= 0;
	height	= 0;
}
PNG::PNG(int _width, int _height){
	create(_width, _height);
}

// Destructor
PNG::~PNG(){}

void PNG::create(int _width, int _height, unsigned char *_im){
	width  = _width;
	height = _height;
	int items = width*height*4;
	for (int x = 0; x<items; x++){
		image.push_back(0);	
	}
	im = _im;
}

// Save current image in file [true = success, false otherwise]
bool PNG::save(string pathname) const{
	//create encoder and set settings and info (optional)
	LodePNG::Encoder encoder;
	encoder.getSettings().zlibsettings.windowSize = 2048;

	//encode and save
	std::vector<unsigned char> buffer;
	encoder.encode(buffer, im==NULL ? 0 : im, width, height);
	LodePNG::saveFile(buffer, pathname);
	
	if(encoder.hasError())
	{
		return false;
	}
	return true;
}

// Load image from file [true = success, false otherwise]
bool PNG::load(string pathname){
	std::vector<unsigned char> buffer;
	LodePNG::loadFile(buffer, pathname); //load the image file with given filename
	LodePNG::Decoder decoder;
	decoder.inspect(buffer);
	if (decoder.isGreyscaleType()){
		decoder.getInfoRaw().color.colorType = 0;
		decoder.getInfoRaw().color.bitDepth = 16;
		// is grayscale
	}
	decoder.decode(image, buffer);
	width  = decoder.getWidth(); 
	height = decoder.getHeight();
	if(decoder.hasError())
	  {
		return false;
	  }
	return true;
}

// access pixel data at x,y
flint PNG::getPixel(const int x, const int y) const{
	flint p;
	//get RGBA components
	p.ch[0] = image[4 * y * width + 4 * x + 0]; // R
	p.ch[1] = image[4 * y * width + 4 * x + 1]; // G
	p.ch[2] = image[4 * y * width + 4 * x + 2]; // B
	p.ch[3] = image[4 * y * width + 4 * x + 3]; // A
	return p;
}
int PNG::getPixelRaw(const int x, const int y) const {
	return 256*image[2*y*width + 2*x]+image[2*y*width + 2*x + 1];
}

// set pixel data at x,y
void PNG::setPixel(const int x, const int y, float value){
	flint z;
	z.f = value;
	setPixel(x,y, z);
}
// set pixel data at x,y
void PNG::setPixel(const int x, const int y, flint value){
	image[4 * y * width + 4 * x + 0] = value.ch[0]; // R
	image[4 * y * width + 4 * x + 1] = value.ch[1]; // G
	image[4 * y * width + 4 * x + 2] = value.ch[2]; // B
	image[4 * y * width + 4 * x + 3] = value.ch[3]; // A	
}

unsigned char * PNG::getData(){
	int size = image.size();
	unsigned char * dataOut = new unsigned char[size];
	for (int i=0; i<size; i++){
		dataOut[i] = image[i];
	}
	return dataOut;
}

float * PNG::getFloatData(){
	int size = image.size();
	float * dataOut = new float[size];
	for (int i=0; i<size; i++){
		dataOut[i] = float(image[i])/255.f;
	}
	return dataOut;
}
void PNG::clear(){
	image.clear();
}