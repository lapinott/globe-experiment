#pragma once

#include <G3D/G3D.h>

class Particle : public VisibleEntity {

public:

	static bool GLOBE_MAGICK_BELIEVER;

	static void GLOBE_MAGICK_BELIEVER_CHANGE_STATE(const shared_ptr<Scene>& s);

private:

	Vector3 m_velocity;
	double m_s_velocity; // constant speed (scalar)

	Vector3 findNewPos(const Vector3& pos, const Vector3& delta);

	void setNewVelocity(const Vector3& oldPos, const Vector3& newPos);

	Vector3 magickKeepPathParallel(const Vector3& oldPos, const Vector3& newPos);

public:

	static shared_ptr<Entity> create(const String& name, Scene* scene, AnyTableReader& propertyTable, const ModelTable& modelTable = ModelTable(),
		const Scene::LoadOptions& options = Scene::LoadOptions());

	static shared_ptr<Particle> create
	(const String&                           name,
		Scene*                                  scene,
		const shared_ptr<Model>&                model,
		const CFrame&                           frame = CFrame(),
		const shared_ptr<Track>&                track = shared_ptr<Entity::Track>(),
		bool                                    canChange = true,
		bool                                    shouldBeSaved = true,
		bool                                    visible = true,
		const Surface::ExpressiveLightScatteringProperties& expressiveLightScatteringProperties = Surface::ExpressiveLightScatteringProperties(),
		const ArticulatedModel::PoseSpline&     artPoseSpline = ArticulatedModel::PoseSpline(),
		const shared_ptr<Model::Pose>&          pose = nullptr);

	virtual void onSimulation(SimTime absoluteTime, SimTime deltaTime) override;

	void setVelocity(const Vector3& v);

	void setVelocityScalar(float v);
};