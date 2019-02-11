#pragma once
#include "UserControlledEntity.h"
#include <G3D/G3D.h>

namespace NYNE {

	class CameraExt : public Camera {

	protected:

		CameraExt() {}

	public:

		virtual void scale(float factor);

		static Matrix4 m4_look_up(const Vector3& look, const Vector3& up);
	};
}