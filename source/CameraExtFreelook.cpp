#include "CameraExtFreelook.h"

using namespace NYNE;

shared_ptr<CameraExtFreelook> CameraExtFreelook::create(const String& name) {
	Any a(Any::TABLE);
	AnyTableReader reader(a);
	return dynamic_pointer_cast<CameraExtFreelook>(create(name, NULL, reader, ModelTable()));
}


shared_ptr<Entity> CameraExtFreelook::create
(const String&      name,
	Scene*             scene,
	AnyTableReader&    reader,
	const ModelTable&  modelTable,
	const Scene::LoadOptions& options) {

	(void)modelTable;

	const shared_ptr<CameraExtFreelook>& c = createShared<CameraExtFreelook>();

	c->Entity::init(name, scene, reader);
	c->Camera::init(reader);
	c->init(reader);
	reader.verifyDone();

	return c;
}

void CameraExtFreelook::init(AnyTableReader& propertyTable) {
	propertyTable.getIfPresent("velocity", m_velocity);
}

void CameraExtFreelook::onUserInput() {
	if (userInput()->keyPressed(UserControlls::m_camera_freelook_enable_mouselook)) {
		m_mouselook_enabled = !m_mouselook_enabled;
		if (m_mouselook_enabled) {
			m_mouse_screen_pos = userInput()->mouseXY();
		}
		else {
			userInput()->setMouseXY(m_mouse_screen_pos);
		}
	}
}

void CameraExtFreelook::onMouseScrollInput() {
	m_velocity *= 1.0f + (float)(mouseScrollInput().dy) / 10.0f;
	m_velocity *= 1.0f + (float)(mouseScrollInput().dy) / 10.0f;
}

void CameraExtFreelook::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
	Camera::onSimulation(absoluteTime, deltaTime);
	UserControlledEntity::onSimulation(absoluteTime, deltaTime);

	if (isUserControlled() && userInput() != nullptr) {

		if (deltaTime > 0) {

			UserInput* ui = userInput();

			if (userInput()->keyDown(UserControlls::m_camera_freelook_forward)) {
				Vector3 translate = frame().lookVector() * (float)deltaTime * m_velocity;
				setFrame(CFrame(frame().rotation, frame().translation + translate));
			}
			if (userInput()->keyDown(UserControlls::m_camera_freelook_backwards)) {
				Vector3 translate = -1.0 * frame().lookVector() * (float)deltaTime * m_velocity;
				setFrame(CFrame(frame().rotation, frame().translation + translate));
			}
			if (userInput()->keyDown(UserControlls::m_camera_freelook_strafe_right)) {
				Vector3 translate = frame().rightVector() * (float)deltaTime * m_velocity;
				setFrame(CFrame(frame().rotation, frame().translation + translate));
			}
			if (userInput()->keyDown(UserControlls::m_camera_freelook_strafe_left)) {
				Vector3 translate = -1.0 * frame().rightVector() * (float)deltaTime * m_velocity;
				setFrame(CFrame(frame().rotation, frame().translation + translate));
			}
			if (userInput()->keyDown(UserControlls::m_camera_freelook_roll_left)) {
				Matrix4 roll = Matrix4::rollDegrees(200 * (float)deltaTime);
				setFrame(CFrame(frame().rotation * roll.upper3x3(), frame().translation));
			}
			if (userInput()->keyDown(UserControlls::m_camera_freelook_roll_right)) {
				Matrix4 roll = Matrix4::rollDegrees(-200 * (float)deltaTime);
				setFrame(CFrame(frame().rotation * roll.upper3x3(), frame().translation));
			}
			if (m_mouselook_enabled) {
				Matrix4 mouse_yaw = Matrix4::yawDegrees(-1.0f * userInput()->mouseDX() * m_sensitivity);
				Matrix4 mouse_pitch = Matrix4::pitchDegrees(-1.0f * userInput()->mouseDY() * m_sensitivity);
				setFrame(CFrame(frame().rotation * mouse_yaw.upper3x3() * mouse_pitch.upper3x3(), frame().translation));
				userInput()->setMouseXY(m_mouse_screen_pos);
			}
		}
	}
}

void CameraExtFreelook::scale(float factor) {
	CameraExt::scale(factor);
	m_velocity *= factor;
}

Any CameraExtFreelook::toAny(const bool forceAll) const {
	return CameraExt::toAny(forceAll);
}
void CameraExtFreelook::onPose(Array< shared_ptr<class Surface> >& surfaceArray) {
	CameraExt::onPose(surfaceArray);
}
void CameraExtFreelook::makeGUI(GuiPane* p, GApp* app) {
	CameraExt::makeGUI(p, app);
}