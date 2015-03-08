#ifndef _LIGHT_H
#define _LIGHT_H

#include "scene\cameras\Camera.h"
#include "scene\scenemodel.h"
class Light :
	public SceneModel
{
public:
	Light(TextureManager * tm);
	~Light(void);
	void draw();

	void drawSun(v3* position);

	virtual void drawForLOD();

	void init();

	void update(double time);

	void translate(v3 &movVector);

	void rotate(v3 &axis, float angleRad);

	void scale(v3 &scaleVector);

	void turnOn();
	void turnOff();
	void setup(GLuint lid, v4 *pos, v4 *dir, v4 &ambi, v4 &diff, v4 &spec, float cutOff, float exp);

	void showTextures();
	void initShadowMapping(Camera *_cam, int resolution);
	void beginShadowMap();
	void endShadowMap();
// attributes
	Texture * shadowMapTexture;
	GLuint fb_shad_ID; // framebuffer
	GLuint db_shad_ID; // depth attachment...
	GLuint cb_shad_ID; // color attachment...

	Shader * shader;
	v4 *position;
	v3 positionFixedToSkybox;
	v4 * direction;
	v3 upVector;
	v3 right;
	GLuint lightId;
	int shadowMapTMID;
	Camera * camera;
	m4 *MVPmatrix;
	m4 MVmatrix;
	m4 Pmatrix;
	int resolution_x;
	int resolution_y;

	float width, height, near, far;

};

#endif

