#include "application.hh"
#include "loader.hh"

Application::Application() {
    Loader loader = Loader();
    //load the config.xml
    loader.loadConfig(this);
    graphics = Graphics(glm::uvec2(windowWidth, windowHeight), 0.1f, farPlane, shadowCubeSize, maxShadowRenderCount);
}

void Application::init()
{
    // init random generator
    std::srand(std::time(NULL));
    // choose Level             TODO: Choose this in a menu
    std::string heightmapFilePath = heightmapPath + "heightmapLvl1.png";
    std::string levelXmlFilePath = levelXmlPath + "Level1.xml";
    this->level = Level(heightmapFilePath, levelXmlFilePath);
    level.getPhysics()->init(geometryPath);
    // Don't change this!
    ignoredMouseUpdates = 0;
    cameraLock = true;
    // set Skybox size
    level.setSkydomeSize((graphics.getFarPlane())-31.0f);
    
    // define where shaders and textures can be found:
    ACGL::Base::Settings::the()->setResourcePath("../");
    ACGL::Base::Settings::the()->setShaderPath(shaderPath);
    ACGL::Base::Settings::the()->setTexturePath(texturePath);
    ACGL::Base::Settings::the()->setGeometryPath(geometryPath);
    
    // load Level
    level.load();
    Loader loader = Loader();
    
    loader.load(levelXmlFilePath, &level, compositionsPath, scriptPath);
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

void Application::setWindowWidth(int windowWidth) {
    this->windowWidth = windowWidth;
}

void Application::setWindowHeight(int windowHeight) {
    this->windowHeight = windowHeight;
}

void Application::setShadowCubeSize(int shadowCubeSize) {
    this->shadowCubeSize = shadowCubeSize;
}

void Application::setFarPlane(float farPlane) {
    this->farPlane = farPlane;
}

void Application::setCompositionsPath(std::string compositionsPath) {
    this->compositionsPath = "../" + compositionsPath;
}

void Application::setShaderPath(std::string shaderPath) {
    this->shaderPath = shaderPath;
}

void Application::setGeometryPath(std::string geometryPath) {
    this->geometryPath = geometryPath;
}

void Application::setTexturePath(std::string texturePath) {
    this->texturePath = texturePath;
}

void Application::setScriptPath(std::string scriptPath) {
    this->scriptPath = "../" + scriptPath;
}

void Application::setHeightmapPath(std::string heightmapPath) {
    this->heightmapPath = "../" + heightmapPath;
}

void Application::setLevelXmlPath(std::string levelXmlPath) {
    this->levelXmlPath = "../" + levelXmlPath;
}

void Application::setMaxShadowRenderCount(int count) {
    this->maxShadowRenderCount = count;
}
