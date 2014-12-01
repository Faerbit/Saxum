#include "application.hh"

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

Application::Application() {
    graphics = Graphics(glm::uvec2(1024, 786), 0.1f, 150.0f);
}

void Application::init()
{
    // Don't change this!
    ignoredMouseUpdates = 0;
    cameraLock = true;
    // set Skybox size
    level.setSkydomeSize((graphics.getFarPlane()/2.0f)-10.0f);

    // define where shaders and textures can be found:
    ACGL::Base::Settings::the()->setResourcePath("../");
    ACGL::Base::Settings::the()->setShaderPath("Shader/");
    ACGL::Base::Settings::the()->setTexturePath("Levels/Textures/");
    ACGL::Base::Settings::the()->setGeometryPath("Levels/Geometry/");

    // construct VAO to give shader correct Attribute locations
    ACGL::OpenGL::SharedArrayBuffer ab = std::make_shared<ACGL::OpenGL::ArrayBuffer>();
    ab->defineAttribute("aPosition", GL_FLOAT, 3);
    ab->defineAttribute("aTexCoord", GL_FLOAT, 2);
    ab->defineAttribute("aNormal", GL_FLOAT, 3);
    ACGL::OpenGL::SharedVertexArrayObject vao = std::make_shared<ACGL::OpenGL::VertexArrayObject>();
    vao->attachAllAttributes(ab);

    // look up all shader files starting with 'phong' and build a ShaderProgram from it:
    shader = ACGL::OpenGL::ShaderProgramCreator("phong").attributeLocations(
            vao->getAttributeLocations()).create();
    shader->use();

    // load Level
    level.load();

    // just in case: check for errors
    openGLCriticalError();
}

Graphics* Application::getGraphics() {
    return &graphics;
}

Level* Application::getLevel() {
    return &level;
}

ACGL::OpenGL::SharedShaderProgram Application::getShader() {
    return shader;
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
