#include "Globe.h"

shared_ptr<Entity> Globe::create
(const String&      name,
	Scene*             scene,
	AnyTableReader&    reader,
	const ModelTable&  modelTable,
	const Scene::LoadOptions& options) {

	(void)modelTable;

	const shared_ptr<Globe>& g = createShared<Globe>();

	g->Entity::init(name, scene, reader);
	g->VisibleEntity::init(reader, modelTable);
	reader.verifyDone();

	return g;
}

void Globe::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
	VisibleEntity::onSimulation(absoluteTime, deltaTime);

	float radAngle = m_angular_speed * deltaTime;
	Matrix4 rMatrix = Matrix4::yawDegrees(radAngle / pi() * 180);

	CFrame currFrame = frame();
	CFrame nextFrame = CFrame(rMatrix.upper3x3() * currFrame.rotation, currFrame.translation);

	setFrame(nextFrame);
}