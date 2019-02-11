#pragma once

#define _DISABLE_EXTENDED_ALIGNED_STORAGE

#include <map>
#include <G3D/G3D.h>

class UserControlls {

private:

	UserControlls() {}

public:

	UserControlls(const UserControlls&) = delete;
	void operator = (const UserControlls&) = delete;

	static GKey m_camera_freelook_forward;
	static GKey m_camera_freelook_backwards;
	static GKey m_camera_freelook_strafe_left;
	static GKey m_camera_freelook_strafe_right;
	static GKey m_camera_freelook_roll_left;
	static GKey m_camera_freelook_roll_right;
	static GKey m_camera_freelook_velocity_less;
	static GKey m_camera_freelook_velocity_more;
	static GKey m_camera_freelook_enable_mouselook;
};