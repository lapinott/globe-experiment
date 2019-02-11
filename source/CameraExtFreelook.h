#pragma once
#include "CameraExt.h"
#include "Controlls.h"
#include <G3D/G3D.h>

namespace NYNE {

	class CameraExtFreelook : public CameraExt, public UserControlledEntity {

	private:

		bool	m_mouselook_enabled		= false; // tmp
		float	m_velocity				= 10;
		float	m_sensitivity			= 0.2;
		Vector2	m_mouse_screen_pos		= Vector2(0, 0);

	public:

		static shared_ptr<Entity> create(const String& name, Scene* scene, AnyTableReader& propertyTable, const ModelTable& modelTable = ModelTable(),
			const Scene::LoadOptions& options = Scene::LoadOptions());

		static shared_ptr<CameraExtFreelook> create(const String& name = "CameraExtFreelook");

		void init(AnyTableReader& propertyTable);

		virtual void onSimulation(SimTime absoluteTime, SimTime deltaTime) override;

		virtual void onUserInput() override;

		virtual void onMouseScrollInput() override;

		virtual void scale(float factor) override;

		// get rid of compiler warnings
		virtual Any toAny(const bool forceAll = false) const override;
		virtual void onPose(Array< shared_ptr<class Surface> >& surfaceArray) override;
		virtual void makeGUI(GuiPane* p, GApp* app) override;

	};
}