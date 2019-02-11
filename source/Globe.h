#pragma once

#include <G3D/G3D.h>

class Globe : public VisibleEntity {

private:

	double m_angular_speed = 7.292e-5;		// rad/s; source: wikipedia

public:

	static shared_ptr<Entity> create(const String& name, Scene* scene, AnyTableReader& propertyTable, const ModelTable& modelTable = ModelTable(),
		const Scene::LoadOptions& options = Scene::LoadOptions());

	virtual void onSimulation(SimTime absoluteTime, SimTime deltaTime) override;
};