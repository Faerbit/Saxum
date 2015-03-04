#include "level.hh"
#include "loader.hh"
#include <string>

Level::Level(std::string heightmapFilePath, std::string xmlFilePath){
    this->terrain = Terrain(heightmapFilePath);
    // default value
    skydomeSize = 50.0f;
    physics = Physics();
    this->xmlFilePath = xmlFilePath;
}

Level::Level() {
}

Level::~Level() {
    if (luaState!=nullptr) {
        lua_close(luaState);
    }
    for(unsigned int i = 0; i<objects.size(); i++) {
        delete(objects.at(i));
    }
}

void Level::load() {
    //Intialize lua state
    // Check if there's an existing state and close it
    if(luaState != nullptr){
        lua_close(luaState);
        luaState = nullptr;
    }
    // Create a new lua state
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
    //Expose the class Level and its functions to Lua
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Level>("Level")
        .addFunction("deleteObject", &Level::deleteObject)
        .addFunction("getObjectCount", &Level::getPhysicsObjectsVectorSize)
        .addFunction("moveObject", &Level::moveObject)
        .endClass();
    //Push the level to Lua as a global variable
    luabridge::push(luaState, this);
    lua_setglobal(luaState, "level");
    
    this->camera = Camera(glm::vec2(-0.8f, 0.0f), 3.0f);
}

void Level::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass,
        glm::mat4* viewProjectionMatrix, std::vector<glm::mat4>* shadowVPs) {
    for(unsigned int i = 0; i<objects.size(); i++) {
        // do not project shadow of skydome 
        if(lightingPass || (objects.at(i) != skydome)) {
            objects.at(i)->render(shader, lightingPass, viewProjectionMatrix, shadowVPs);
        }
    }
}

void Level::update(float runTimeSinceLastUpdate, float runTime, glm::vec2 mouseDelta, bool wPressed, bool aPressed, bool sPressed, bool dPressed,bool kPressed, bool lPressed) {
    physics.takeUpdateStep(runTimeSinceLastUpdate);
    
    // Ignore first two mouse updates, because they are incorrect
    // DON'T try to move this functionallity elsewhere
    static int i = 0;
    if (i <2) {
        i++;
    }
    else {
        mouseDelta.x = -mouseDelta.x;
        camera.updateRotation(mouseDelta/100.0f);
        physics.updateCameraPos(mouseDelta, 50, camera.getDistance());
        
        camera.setPosition(physics.getCameraPosition());
        camera.setDirection(physics.getCameraToPlayer());
    }
    if(wPressed){
        physics.rollForward(camera.getVector(),strength);
    }
    if(aPressed) {
        physics.rollLeft(camera.getVector(),strength);
    }
    if(sPressed) {
        physics.rollBack(camera.getVector(),strength);
    }
    if(dPressed){
        physics.rollRight(camera.getVector(),strength);
    }
    
    if(kPressed)
        camera.setIsPhysicsCamera(true);
    if(lPressed)
        camera.setIsPhysicsCamera(false);
    
    cameraCenter->setPosition(physics.getPos(0));
    cameraCenter->setRotation(physics.getRotation(0));
    
    for(unsigned i = 0; i < physicsObjects.size();i++)
    {
        physicsObjects[i]->setPosition(physics.getPos(i));
        physicsObjects[i]->setRotation(physics.getRotation(i));
    }
    
    skydome->setPosition(glm::vec3(cameraCenter->getPosition().x, 
        0.0f, cameraCenter->getPosition().z));

    if (runTime > 2.0f) {
        for(unsigned int i = 0; i<triggers.size(); i++) {
            triggers.at(i).triggerUpdate();
        }
    }
}

glm::vec3 Level::getAmbientLight() {
    return ambientLight;
}

std::vector<Light>* Level::getLights() {
    return &lights;
}

Camera* Level::getCamera() {
    return &camera;
}

Object* Level::getCameraCenter() {
    return cameraCenter;
}

Light* Level::getDirectionalLight() {
    return &directionalLight;
}

glm::vec4 Level::getFogColour() {
    return fogColour;
}

glm::vec3 Level::getCameraPosition() {
   return cameraCenter->getPosition() + camera.getVector();
}

void Level::setSkydomeSize(float size) {
    skydomeSize = size;
}

float Level::getSkydomeSize() {
    return this->skydomeSize;
}

std::vector<Object*>* Level::getObjects() {
    return &objects;
}

std::vector<Object*>* Level::getPhysicsObjects() {
    return &physicsObjects;
}

void Level::moveObject(int objectIndex, float strength, float xPos, float yPos, float zPos){
    glm::vec3 position = glm::vec3(xPos, yPos, zPos);
    physics.removePositionConstraint(objectIndex);
    physics.addPositionConstraint(objectIndex, strength, position);
}

//should not be used since objects does not get synchronized and deletion is not implemented in pyhsics
void Level::deleteObject(int objectIndex){
    physicsObjects.erase(physicsObjects.begin() + objectIndex);
    for(unsigned int i = 0; i<triggers.size(); i++) {
        if(triggers.at(i).deleteNotification(objectIndex)){
            triggers.erase(triggers.begin() + i);
            i--;
        }
    }
}

void Level::resetPlayer(){
    Loader loader = Loader();
    glm::vec3 newPosition = loader.reloadPlayerPosition(xmlFilePath, this);
    //TODO cameraCenter.setPosition(newPosition);
}

void Level::setStrength(float strength) {
    this->strength = strength;
}

void Level::setSkydomeObject(Object* object){
    this->skydome = object;
}

void Level::addObject(Object* object) {
    objects.push_back(object);
}

void Level::addPhysicsObject(Object* object) {
    physicsObjects.push_back(object);
}

void Level::setAmbientLight(glm::vec3 colour) {
    this->ambientLight = colour;
}

void Level::setFogColour(glm::vec4 colour) {
    this->fogColour = colour;
}

void Level::setDirectionalLight(Light light) {
    this->directionalLight = light;
}

Physics* Level::getPhysics() {
    return &physics;
}

unsigned int Level::getObjectsVectorSize() {
    return objects.size();
}

unsigned int Level::getPhysicsObjectsVectorSize() {
    return physicsObjects.size();
}

void Level::setCameraCenter(Object* object) {
    this->cameraCenter = object;
}

void Level::addLight(Light light) {
    this->lights.push_back(light);
}

void Level::addTrigger(Trigger trigger) {
    this->triggers.push_back(trigger);
}

lua_State* Level::getLuaState() {
    return luaState;
}

Terrain* Level::getTerrain() {
    return &terrain;
}
