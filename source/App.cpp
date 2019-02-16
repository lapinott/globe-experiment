/** \file App.cpp */
#include "App.h"

using namespace NYNE;

// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    initGLG3D(G3DSpecification());

    GApp::Settings settings(argc, argv);

    // Change the window and other startup parameters by modifying the
    // settings class.  For example:
    settings.window.caption             = argv[0];

    // Set enable to catch more OpenGL errors
#ifdef G3D_DEBUG
    settings.window.debugContext = true;
#else
	settings.window.debugContext = false;
#endif

    settings.window.width               = 1408;
	settings.window.height				= 792;

    settings.window.fullScreen          = false;
	settings.window.resizable			= true;
	settings.window.framed				= true;
    settings.window.defaultIconFilename = "icon.png";

    // Set to true for a significant performance boost if your app can't render at 60fps, or if
    // you *want* to render faster than the display.
    settings.window.asynchronous        = true;

    settings.hdrFramebuffer.depthGuardBandThickness = Vector2int16(0, 0);
    settings.hdrFramebuffer.colorGuardBandThickness = Vector2int16(0, 0);
	settings.dataDir = String(FileSystem::currentDirectory().append("/data-files"));
    settings.screenCapture.outputDirectory = FileSystem::currentDirectory();
    settings.screenCapture.includeAppRevision = false;
    settings.screenCapture.includeG3DRevision = false;
    settings.screenCapture.filenamePrefix = "_";

    settings.renderer.deferredShading = true;
    settings.renderer.orderIndependentTransparency = true;

    return App(settings).run();
}


App::App(const GApp::Settings& settings) : GApp(settings) {
}


// Called before the application loop begins.  Load data here and
// not in the constructor so that common exceptions will be
// automatically caught.
void App::onInit() {
    GApp::onInit();

    setFrameDuration(1.0f / 30.0f);
    
    showRenderingStats      = false;

    makeGUI();
    developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    developerWindow->videoRecordDialog->setCaptureGui(false);
	developerWindow->cameraControlWindow->setVisible(false);
	developerWindow->setVisible(false);

	scene()->registerEntitySubclass("CameraExtFreelook", CameraExtFreelook::create, true);
	scene()->registerEntitySubclass("Globe", Globe::create, true);
	scene()->registerEntitySubclass("Particle", Particle::create, true);

    loadScene("Globe Experiment");

	m_camera = scene()->typedEntity<CameraExtFreelook>("cameraExt");
	setActiveCamera(m_camera);
	scene()->removeEntity("cameraExt");
	UserControlledEntity::add(m_camera);

	m_particle = scene()->typedEntity<Particle>("particle");
	scene()->removeEntity("particle");

	m_font = GFont::fromFile("cooper.fnt");

	m_plane_tangeant = scene()->typedEntity<VisibleEntity>("plane_tangeant");
	m_plane_magick = scene()->typedEntity<VisibleEntity>("plane_magick");
	m_plane_tangeant->setVisible(false);
	m_plane_magick->setVisible(false);

	m_plane_geodesic = scene()->typedEntity<VisibleEntity>("plane_geodesic");
	m_plane_parallel = scene()->typedEntity<VisibleEntity>("plane_parallel");
	m_plane_geodesic->setVisible(false);
	m_plane_parallel->setVisible(false);
}


void App::makeGUI() {
    debugWindow->setVisible(false);
    developerWindow->videoRecordDialog->setEnabled(true);

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}

void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    GApp::onSimulation(rdt, sdt, idt);

    // Example GUI dynamic layout code.  Resize the debugWindow to fill
    // the screen horizontally.
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
	m_camera->onSimulation(0, rdt);

	// update plane positions
	if (m_mrig) {
		m_plane_tangeant->setFrame(getTangeantPlaneFrame());
		m_plane_magick->setFrame(getMagickPlaneFrame());
	}
}

void App::onUserInput(UserInput* ui) {
    GApp::onUserInput(ui);
    (void)ui;
    // Add key handling here based on the keys currently held or
    // ones that changed in the last frame.
	UserControlledEntity::onUserInput(ui);

	if (userInput->keyPressed(GKey::LEFT_MOUSE)) {
		if (!userInput->keyDown(GKey::LCTRL))
			spawnParticle(ui);
		else {
			makePlaneGeodesic();
			makePlaneParallel();
		}

	}

	if (userInput->keyPressed(GKey::GKey('k'))) {
		killParticles();
	}

	if (userInput->keyPressed(GKey::GKey('p'))) {
		massSpawnParticlesRandom();
	}

	if (userInput->keyPressed(GKey::GKey('m'))) { // did you change your mind? :]
		Particle::GLOBE_MAGICK_BELIEVER = !Particle::GLOBE_MAGICK_BELIEVER;
		Particle::GLOBE_MAGICK_BELIEVER_CHANGE_STATE(scene());
	}

	if (userInput->keyPressed(GKey::GKey('u'))) {
		m_write_ui = !m_write_ui;
	}

	if (userInput->keyPressed(GKey::GKey('l'))) {
		Array<shared_ptr<Entity>> ee;
		scene()->getEntityArray(ee);
		for (const shared_ptr<Entity>& e : ee) {
			if (e->name().find(String("plane_")) == 0) {
				scene()->remove(e);
			}
		}
	}

	// mouse ray x globe
	m_mrig = mouseRayIntersectsGlobe(ui, m_mrig_ray, m_mrig_time);

	// small planes display
	if (m_mrig && !userInput->keyReleased(GKey::SPACE)) {
		if (userInput->keyDown(GKey::SPACE)) {
			m_plane_tangeant->setVisible(true);
			m_plane_magick->setVisible(true);
		}
	}
	else {
		m_plane_tangeant->setVisible(false);
		m_plane_magick->setVisible(false);
	}
}

void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) {
	if (!scene()) {
		if ((submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) && (!rd->swapBuffersAutomatically())) {
			swapBuffers();
		}
		rd->clear();
		rd->pushState(); {
			rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
			drawDebugShapes();
		} rd->popState();
		return;
	}


	GBuffer::Specification gbufferSpec = m_gbufferSpecification;
	extendGBufferSpecification(gbufferSpec);
	m_gbuffer->setSpecification(gbufferSpec);
	m_gbuffer->resize(m_framebuffer->width(), m_framebuffer->height());
	m_gbuffer->prepare(rd, activeCamera(), 0, -(float)previousSimTimeStep(), m_settings.hdrFramebuffer.depthGuardBandThickness, m_settings.hdrFramebuffer.colorGuardBandThickness);

	m_renderer->render(rd,
		activeCamera(),
		m_framebuffer,
		scene()->lightingEnvironment().ambientOcclusionSettings.enabled ? m_depthPeelFramebuffer : nullptr,
		scene()->lightingEnvironment(), m_gbuffer,
		allSurfaces);

	rd->push2D(); {
		if (m_font != nullptr && m_write_ui)
			writeUI();
	} rd->pop2D();

	rd->pushState(m_framebuffer); {
		// Call to make the App show the output of debugDraw(...)
		rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
		drawDebugShapes();
		const shared_ptr<Entity>& selectedEntity = (notNull(developerWindow) && notNull(developerWindow->sceneEditorWindow)) ? developerWindow->sceneEditorWindow->selectedEntity() : nullptr;
		scene()->visualize(rd, selectedEntity, allSurfaces, sceneVisualizationSettings(), activeCamera());

		onPostProcessHDR3DEffects(rd);
	} rd->popState();

	if (submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) {
		swapBuffers();
	}

	rd->clear();

	m_film->exposeAndRender(rd, activeCamera()->filmSettings(), m_framebuffer->texture(0), settings().hdrFramebuffer.colorGuardBandThickness.x + settings().hdrFramebuffer.depthGuardBandThickness.x, settings().hdrFramebuffer.depthGuardBandThickness.x,
		Texture::opaqueBlackIfNull(notNull(m_gbuffer) ? m_gbuffer->texture(GBuffer::Field::SS_POSITION_CHANGE) : nullptr),
		activeCamera()->jitterMotion());
}

bool App::mouseRayIntersectsGlobe(UserInput* ui, Ray& ray, float& time) {
	// find mouse ray x globe intersection
	ray = m_camera->worldRay(ui->mouseXY().x, ui->mouseXY().y, renderDevice->viewport());
	time = ray.intersectionTime(Sphere(Point3(0, 0, 0), 6.356));

	return !(time == inf());
}

CFrame App::getTangeantPlaneFrame() const {
	Point3 intersection = m_mrig_ray.origin() + m_mrig_ray.direction() * m_mrig_time;
	Matrix3 r;
	if ((intersection - Vector3(0, 6.356, 0)).length() < 0.0001)
		r = Matrix3::identity();
	else {
		Vector3 y = intersection.unit();
		Vector3 x = (Vector3(0, 1, 0).cross(y)).unit();
		Vector3 z = x.cross(y);
		r.setColumn(0, x);
		r.setColumn(1, y);
		r.setColumn(2, z);
	}

	return CFrame(r, intersection);
}

CFrame App::getMagickPlaneFrame() const {
	Point3 intersection = m_mrig_ray.origin() + m_mrig_ray.direction() * m_mrig_time;
	Matrix3 r;
	if ((intersection - Vector3(0, 6.356, 0)).length() < 0.0001)
		r = Matrix3::identity();
	else {
		Vector3 y = Vector3(intersection.x, 0, intersection.z).unit();
		Vector3 x = Vector3(0, -1, 0);
		Vector3 z = x.cross(y);
		r.setColumn(0, x);
		r.setColumn(1, y);
		r.setColumn(2, z);
	}

	return CFrame(r, intersection);
}

void App::spawnParticle(UserInput* ui) {

	if (m_mrig) {

		Point3 intersection = m_mrig_ray.origin() + (m_mrig_ray.direction() * m_mrig_time);

		// create particle
		shared_ptr<Particle> new_particle =
			Particle::create(
				String("p") + String(m_particle_count),
				scene().get(),
				m_particle->model(),
				CFrame(intersection));

		scene()->insert(new_particle);
		++m_particle_count;
	}
}

void App::killParticles() {
	Array<shared_ptr<Particle>> particles;
	scene()->getTypedEntityArray<Particle>(particles);
	for (shared_ptr<Particle>& p : particles) {
		scene()->remove(p);
		--m_particle_count;
	}
}

void App::massSpawnParticlesRandom() {

	for (int i = 0; i < 50; i++) {

		// find random intersection
		Vector3 rvec = Vector3::random();
		Ray rray = Ray(Point3(0, 0, 0), rvec);
		float time = rray.intersectionTime(Sphere(Point3(0, 0, 0), 6.356));

		if (!(time == inf())) {

			Point3 intersection = rray.origin() + (rray.direction() * time);

			// create particle
			shared_ptr<Particle> new_particle =
				Particle::create(
					String("p") + String(m_particle_count),
					scene().get(),
					m_particle->model(),
					CFrame(intersection));

			scene()->insert(new_particle);
			++m_particle_count;
		}
	}
}

void App::makePlaneGeodesic() {

	if (m_mrig) {
		Point3 intersection = m_mrig_ray.origin() + (m_mrig_ray.direction() * m_mrig_time);
		Matrix3 r;
		Vector3 x = intersection.unit();
		Vector3 z = (Vector3(0, 1, 0).cross(x)).unit();
		Vector3 y = x.cross(z);
		r.setColumn(0, x);
		r.setColumn(1, y);
		r.setColumn(2, z);
		Point3 t = Point3::zero();

		ArticulatedModel::Specification spec;
		spec.filename = "plane/plane-geodesic.obj";
		spec.scale = 1.0f;
		shared_ptr<ArticulatedModel> mdl = ArticulatedModel::create(spec);

		shared_ptr<VisibleEntity> plane = VisibleEntity::create(
			String("plane_geodesic_" + String(std::to_string(m_pgc++))),
			scene().get(),
			mdl,
			CFrame(r, t),
			shared_ptr<Entity::Track>(),
			false,
			false,
			true,
			Surface::ExpressiveLightScatteringProperties{ false, false }
		);
		scene()->insert(plane);
	}
}

void App::makePlaneParallel() {
	if (m_mrig) {
		Point3 intersection = m_mrig_ray.origin() + (m_mrig_ray.direction() * m_mrig_time);

		ArticulatedModel::Specification spec;
		spec.filename = "plane/plane-parallel.obj";
		spec.scale = (Vector3(intersection.x, 0, intersection.z).length() + 0.644) / 7;
		shared_ptr<ArticulatedModel> mdl = ArticulatedModel::create(spec);

		shared_ptr<VisibleEntity> plane = VisibleEntity::create(
			String("plane_parallel_" + String(std::to_string(m_pgc++))),
			scene().get(),
			mdl,
			CFrame(Matrix3::identity(), Point3(0, intersection.y, 0)),
			shared_ptr<Entity::Track>(),
			false,
			false,
			true,
			Surface::ExpressiveLightScatteringProperties{ false, false }
		);
		scene()->insert(plane);
	}
}

void App::writeUI() {
	m_font->draw2D(renderDevice, "UI: u",
		Point2(renderDevice->width() - 480.0f, 25.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Camera: right mouse + w/a/s/d/q/e",
		Point2(renderDevice->width() - 480.0f, 55.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Spawn particle: click",
		Point2(renderDevice->width() - 480.0f, 85.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Mass spawn particles: p",
		Point2(renderDevice->width() - 480.0f, 115.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Kill particles: k",
		Point2(renderDevice->width() - 480.0f, 145.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Display tangeant/magick planes: space",
		Point2(renderDevice->width() - 480.0f, 175.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Create geodesic/parallel planes: lctrl + click",
		Point2(renderDevice->width() - 480.0f, 205.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Remove all geodesic/parallel planes: l",
		Point2(renderDevice->width() - 480.0f, 235.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, "Spinning ball magick magnet: m",
		Point2(renderDevice->width() - 480.0f, 265.0f), 18.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2DWordWrap(renderDevice, 400, "(the infamous magick physical force that makes the atmosphere stray from it's natural path)",
		Point2(renderDevice->width() - 480.0f, 290.0f), 14.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
	m_font->draw2D(renderDevice, String("MAGICK MAGNET: ") + String(Particle::GLOBE_MAGICK_BELIEVER ? "ON" : "OFF"),
		Point2(renderDevice->width() - 480.0f, renderDevice->height() - 100), 24.0f, Color4(Color3::red(), 0.7), Color4(Color3::black(), 0.7));
}