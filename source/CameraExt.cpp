#include "CameraExt.h"

using namespace NYNE;

void CameraExt::scale(float factor) {
	//setFarPlaneZ(farPlaneZ() * factor);
	setNearPlaneZ(nearPlaneZ() * factor);
}

Matrix4 CameraExt::m4_look_up(const Vector3& look, const Vector3& up) { // tmp for ginza
	Matrix4 m;

	Vector3 l = normalize(look);
	Vector3 u = normalize(up);

	Vector3 xaxis = u.cross(l);
	normalize(xaxis);

	Vector3 yaxis = l.cross(xaxis);
	normalize(yaxis);

	m.setRow(0, Vector4(xaxis.x, yaxis.x, look.x, 0.0f));
	m.setRow(1, Vector4(xaxis.y, yaxis.y, look.y, 0.0f));
	m.setRow(2, Vector4(xaxis.z, yaxis.z, look.z, 0.0f));
	m.setRow(3, Vector4(0.0f, 0.0f, 0.0f, 1.0f));

	return m;
}