#include "application.hh"
#include "loader.hh"
using namespace tinyxml2;

Application::Application() {
    //load the config.xml
    loadConfig();
    graphics = Graphics(glm::uvec2(windowWidth, windowHeight), 0.1f, farPlane);
}

void Application::init()
{
    // init random generator
    std::srand(std::time(NULL));
    // choose Level             TODO: Choose this in a menu
    std::string heightmapFilePath = heightmapPath + "heightmapLvl1.png";
    this->level = Level(heightmapFilePath);
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
    std::string levelXmlFilePath = levelXmlPath + "Level1.xml";
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

void Application::loadConfig() {
    XMLDocument* config = new XMLDocument();
    const char* xmlFile = "../data/config.xml";
    config->LoadFile(xmlFile);
    if (config->ErrorID()!=0){
        printf("Could not open config.xml!\n");
        exit(-1);
    }
    XMLElement* resolution = config->FirstChildElement("resolution");
    errorCheck(resolution->FirstChildElement("width")->QueryIntText(&windowWidth));
    errorCheck(resolution->FirstChildElement("height")->QueryIntText(&windowHeight));
    errorCheck(config->FirstChildElement("farPlane")->QueryFloatText(&farPlane));
    const char* charCompositionsPath = config->FirstChildElement("compositionsPath")->GetText();
    if(charCompositionsPath == NULL){
        printf("XMLError: No compositionsPath found.\n");
        exit(-1);
    }
    compositionsPath = charCompositionsPath;

    const char* charShaderPath = config->FirstChildElement("shaderPath")->GetText();
    if(charShaderPath == NULL){
        printf("XMLError: No shaderPath found.\n");
        exit(-1);
    }
    shaderPath = charShaderPath;

    const char* charGeometryPath = config->FirstChildElement("geometryPath")->GetText();
    if(charGeometryPath == NULL){
        printf("XMLError: No geometryPath found.\n");
        exit(-1);
    }
    geometryPath = charGeometryPath;

    const char* charTexturePath = config->FirstChildElement("texturePath")->GetText();
    if(charTexturePath == NULL){
        printf("XMLError: No texturePath found.\n");
        exit(-1);
    }
    texturePath = charTexturePath;

    const char* charScriptPath = config->FirstChildElement("scriptPath")->GetText();
    if(charScriptPath == NULL){
        printf("XMLError: No scriptPath found.\n");
        exit(-1);
    }
    scriptPath = charScriptPath;

    const char* charHeightmapPath = config->FirstChildElement("heightmapPath")->GetText();
    if(charHeightmapPath == NULL){
        printf("XMLError: No heightmapPath found.\n");
        exit(-1);
    }
    heightmapPath = charHeightmapPath;

    const char* charLevelXmlPath = config->FirstChildElement("levelXmlPath")->GetText();
    if(charLevelXmlPath == NULL){
        printf("XMLError: No levelXmlPath found.\n");
        exit(-1);
    }
    levelXmlPath = charLevelXmlPath;
}

void Application::errorCheck(XMLError error){
    if (error) {
        printf("XMLError: ");
        if (error == XML_WRONG_ATTRIBUTE_TYPE) {
            printf("Wrong attribute type.\n");
        }
        else if (error == XML_NO_ATTRIBUTE) {
            printf("No attribute.\n");
        }
        else if (error == XML_CAN_NOT_CONVERT_TEXT) {
            printf("Can not convert text.\n");
        }
        else if (error == XML_NO_TEXT_NODE) {
            printf("No text.\n");
        }
        else {
            printf("Unknown error.\n");
        }
        exit(-1);
    }
}
