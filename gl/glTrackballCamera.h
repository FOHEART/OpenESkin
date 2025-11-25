#pragma once
#include "glInclude.h"

class TrackballCamera
{
public:
	TrackballCamera()
	{
		reset();
	}

	void reset();

	QMatrix4x4 getViewMatrix() const;

	void rotate(float angleX, float angleY);

	void pan(float dx, float dy);
	void zoom(float delta);

	float getDistance() const { return distance; }

private:
	QQuaternion rotation;
	QVector3D translation;
	float distance; // 相机到原点的距离
};

