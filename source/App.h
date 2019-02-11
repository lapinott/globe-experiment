/**
  \file App.h

  The G3D 10.00 default starter app is configured for OpenGL 4.1 and
  relatively recent GPUs.
 */
#pragma once

#include <G3D/G3D.h>
#include "CameraExtFreelook.h"
#include "Globe.h"
#include "Particle.h"

using namespace NYNE;

/** \brief Application framework. */
class App : public GApp {
protected:

    /** Called from onInit */
    void makeGUI();

	shared_ptr<Globe> m_globe;
	shared_ptr<CameraExtFreelook> m_camera;
	shared_ptr<Particle> m_particle;
	unsigned m_particle_count = 0;

	shared_ptr<VisibleEntity> m_plane_tangeant;
	shared_ptr<VisibleEntity> m_plane_magick;

	void spawnParticle(UserInput* ui);

	void killParticles();

	void massSpawnParticlesRandom();

	void writeUI();

	bool mouseRayIntersectsGlobe(UserInput* ui, Ray& ray, float& time);
	bool m_mrig = false;
	Ray m_mrig_ray;
	float m_mrig_time;

	CFrame getTangeantPlaneFrame() const;
	CFrame getMagickPlaneFrame() const;

	shared_ptr<GFont> m_font;

	bool m_write_ui = true;

public:
    
    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;
    virtual void onUserInput(UserInput* ui) override;

	virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
};
