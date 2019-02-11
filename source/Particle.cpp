#include "Particle.h"
#include <string>

shared_ptr<Entity> Particle::create
(const String&      name,
	Scene*             scene,
	AnyTableReader&    reader,
	const ModelTable&  modelTable,
	const Scene::LoadOptions& options) {

	(void)modelTable;

	const shared_ptr<Particle>& p = createShared<Particle>();

	p->Entity::init(name, scene, reader);
	p->VisibleEntity::init(reader, modelTable);
	reader.verifyDone();

	return p;
}

shared_ptr<Particle> Particle::create
(const String&                           name,
	Scene*                                  scene,
	const shared_ptr<Model>&                model,
	const CFrame&                           frame,
	const shared_ptr<Entity::Track>&        track,
	bool                                    canChange,
	bool                                    shouldBeSaved,
	bool                                    visible,
	const Surface::ExpressiveLightScatteringProperties& expressiveLightScatteringProperties,
	const ArticulatedModel::PoseSpline&     artPoseSpline,
	const shared_ptr<Model::Pose>&          pose) {

	const shared_ptr<Particle>& particle = createShared<Particle>();

	particle->Entity::init(name, scene, frame, track, canChange, shouldBeSaved);
	particle->VisibleEntity::init(model, visible, expressiveLightScatteringProperties, artPoseSpline, MD3Model::PoseSequence(), pose);

	// we find our particle initial velocity based on where it is spawned.
	// following the 'globe model', this speed (including direction) is simply the speed
	// of the 'parallel' upon which the particle is spawned.
	// we are said to believe everything revolves around the earth's imaginary axis, along
	// a parallel line defined as a 3D circle perpendicular to earth's axis, so
	// lets implement exactly this here...
	double globe_angular_speed_rad = 7.292e-5;	/* [rad/s] */
	Vector3 spawn_pos = particle->frame().translation;

	// vectorial velocity
	Vector3 parallel = spawn_pos.cross(Vector3(0, -1, 0));
	Vector3 v = parallel * globe_angular_speed_rad;

	// set velocity
	particle->setVelocity(v);
	particle->setVelocityScalar(v.length());

	return particle;
}

bool Particle::GLOBE_MAGICK_BELIEVER = false;

void Particle::GLOBE_MAGICK_BELIEVER_CHANGE_STATE(const shared_ptr<Scene>& s) {
	Array<shared_ptr<Particle>> particles;
	s->getTypedEntityArray<Particle>(particles);
	for (shared_ptr<Particle>& p : particles) {
		// re-init
		double globe_angular_speed_rad = 7.292e-5;
		Vector3 pos = p->frame().translation;
		Vector3 parallel = pos.cross(Vector3(0, -1, 0));
		Vector3 v = parallel * globe_angular_speed_rad;
		p->setVelocity(v);
		p->setVelocityScalar(v.length());
	}
}

void Particle::onSimulation(SimTime absoluteTime, SimTime deltaTime) {
	VisibleEntity::onSimulation(absoluteTime, deltaTime);

	// delta position
	Vector3 delta = m_velocity * deltaTime;
	if (delta.length() == 0 || delta.isNaN() || !delta.isFinite())
		return;
	debugAssertM(G3D::abs(m_velocity.length() / m_s_velocity - 1) < 0.001,
		String("diff: ") + String(std::to_string(G3D::abs(m_velocity.length() / m_s_velocity - 1))) +
		String("\nm_velocity.length() = ") + String(std::to_string(m_velocity.length())) +
		String("\nm_s_velocity = ") + String(std::to_string(m_s_velocity))
	);

	// we 'pull' the particle down to stay at the same altitude (otherwise they would fly to orbit),
	// like air particles stay at the same altitude under stable conditions
	// due to atmospheric pressure (Archimedes force etc... same as a body of water
	// is 'quiet' under stable conditions)
	// this is when something interesting happens. Particles don't
	// follow their parralel lines as they 'should' (according to spinning-ball believers)
	// but converge to the "equator", according to the laws of physics instead.
	Vector3 new_pos = findNewPos(frame().translation, delta);

	// the magickal globe-believer force here
	if (GLOBE_MAGICK_BELIEVER)
		new_pos = magickKeepPathParallel(frame().translation, new_pos);

	// actual physics
	else
		// nothing to add... :]

	setNewVelocity(frame().translation, new_pos);

	setFrame(CFrame(frame().rotation, new_pos));
}

void Particle::setVelocity(const Vector3& v) {
	m_velocity = v;
}

void Particle::setVelocityScalar(float v) {
	m_s_velocity = v;
}

void Particle::setNewVelocity(const Vector3& oldPos, const Vector3& newPos) {
	Vector3 newVelocity = newPos - oldPos;
	setVelocity(newVelocity.unit() * m_s_velocity);
}

// we want to keep the same 'delta' length, but that
// the resulting pos (after applying the delta) still lies on
// the globe's surface
Vector3 Particle::findNewPos(const Vector3& pos, const Vector3& delta) {

	float omega = 2 * asin((delta.length() / 2) / 6.356); // hardcode booooo

	// we need to rotate pos by omega in the plane defined by pos & delta
	// that is: keeping a particle's original momentum
	Matrix3 r = Matrix3::fromAxisAngle(pos.cross(delta), omega);

	return r * pos;
}

// an unknown magick physical force that spinning-ball believers have to believe in also
Vector3 Particle::magickKeepPathParallel(const Vector3& oldPos, const Vector3& newPos) {

	// we have to align the delta on a parallel line/plane here...
	Vector3 delta = newPos - oldPos;
	Vector3 deltaH = Vector3(delta.x, 0, delta.z).unit() * delta.length();
	//                                ^ magick here!!

	// now accounting for the small altitude offset from previous operation (yes details........)
	// also the 'radius' to use here is the one the particle is magickly rotating about
	float smallRadius = 6.356 * sin(::acos(oldPos.dot(Vector3(0, 1, 0)) / oldPos.length()));
	float omega = 2 * asin((deltaH.length() / 2) / smallRadius);
	Matrix3 r = Matrix3::fromAxisAngle(Vector3(0, 1, 0), omega);

	return r * oldPos;
}