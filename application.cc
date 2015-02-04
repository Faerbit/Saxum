#include "application.hh"
#include "loader.hh"


Application::Application() {
    graphics = Graphics(glm::uvec2(1024, 786), 0.1f, 150.0f);
}

void Application::init()
{
    // init random generator
    std::srand(std::time(NULL));
    // choose Level             TODO: Choose this in a menu
    this->level = Level("1");
    // Don't change this!
    ignoredMouseUpdates = 0;
    cameraLock = true;
    // set Skybox size
    level.setSkydomeSize((graphics.getFarPlane())-31.0f);

    // define where shaders and textures can be found:
    ACGL::Base::Settings::the()->setResourcePath("../");
    ACGL::Base::Settings::the()->setShaderPath("Shader/");
    ACGL::Base::Settings::the()->setTexturePath("Levels/Textures/");
    ACGL::Base::Settings::the()->setGeometryPath("Levels/Geometry/");

    // load Level
    level.load();
    Loader loader = Loader();
    loader.load("Level1.xml", &level);

    graphics.init(&level);

    // just in case: check for errors
    openGLCriticalError();
}

Graphics* Application::getGraphics() {
    return &graphics;
}

Level* Application::getLevel() {
    return &level;
}

void Application::setFocused(bool focused) {
    this->focused = focused;
}

bool Application::isFocused() {
    return focused;
}

void Application::setCameraLock(bool locked) {
    // Prevent camera jumping huge distances
    if (!locked) {
        ignoreNextMouseUpdate();
    }
    cameraLock = locked;
}

void Application::ignoreNextMouseUpdate() {
    ignoredMouseUpdates++;
}

void Application::ignoredOneMouseUpdate() {
    ignoredMouseUpdates--;
}

int Application::getIgnoredMouseUpdates() {
    return ignoredMouseUpdates;
}

bool Application::isLocked() {
    return cameraLock;
}
