#include "glTrackballCamera.h"

void TrackballCamera::reset()
{
	rotation = QQuaternion();
	translation = QVector3D(0.0f, 0.0f, -5.0f);
	distance = 5.0f; // 初始距离
}

QMatrix4x4 TrackballCamera::getViewMatrix() const
{
	QMatrix4x4 view;
	view.translate(translation);
	view.rotate(rotation);
	return view;
}

void TrackballCamera::rotate(float angleX, float angleY)
{
	QQuaternion newRot = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, angleY) *
		QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, angleX);
	rotation = newRot * rotation;
}

void TrackballCamera::pan(float dx, float dy)
{
	translation += QVector3D(dx, dy, 0.0f);
}

void TrackballCamera::zoom(float delta)
{
	// 计算新的距离
	distance += delta;

	// 应用距离限制 (1.5 - 50)
	distance = qBound(1.5f, distance, 50.0f);

	// 更新相机位置
	QVector3D direction = QVector3D(0, 0, -1); // 初始看向-Z方向
	translation.setZ(-distance); // 更新Z轴位置
}
