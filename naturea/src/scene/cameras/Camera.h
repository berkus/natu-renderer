#ifndef _CAMERA_H
#define _CAMERA_H

#include "settings.h"
#include "scene\terrain\Terrain.h"

class Camera
{
public:
	Camera();
	~Camera();
	void set(Camera * _cam);
	void setup(v3 & pos, v3 & dir, v3 &up, int *w, int *h, float fo, float n, float fa);
	void setPosition(float x, float y, float z);
	void setPosition(v3 _v);
	v3 getPosition();
	void setDirection(float x, float y, float z);
	void setDirection(v3 _v);
	v3 getDirection();

	void setLookAtPoint(v3 &_p);
	void orbitY(v3 &center, float radius, float time);

	m4 getViewMatrix();
	m4 getLightMVPCameraVInverse();
	m4 getProjectionMatrix();

	float getTurn();
	void setTurn(float _turn);
	float getLookup();
	void setLookup(float _lookup);

	Vector4 * getCornersCameraInWs();
	void computeCorners();

	void setTerrain(Terrain * terr);
	void setMode(CameraMode m);

	void draw();
	void shoot();

	void update(double _time);
	void move(v3 & dist);
	void rotate(v3 & axis, float angle);
	void zoom(float fovy);

	void handleMouseMove(int x, int y);
	bool handleKeyDown(int key, int action);
	v3 position;
	v3 direction;
	v3 upVector;
	float ratio;
	float frustum_treshold;
	float getFrustumTreshold();
private:
	Terrain * terrain;
	CameraMode mode;
	v3 human_movement;
	float activityFactor;
	double time;
	float angle;

	float step;
	Vector4 corners[8];
	Vector4 cornersCameraInWs[8];
	float points[24];
	GLuint vbo;
	GLuint index_vbo;
	bool isWalking;
	GLsizei* width, *height;
	GLfloat fov, near, far;


};

#endif
