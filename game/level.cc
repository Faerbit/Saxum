#include "level.hh"
#include "loader.hh"
#include <string>

Level::Level(std::string xmlFilePath, float farPlane) {
    // default value
    skydomeSize = 50.0f;
    physics = Physics();
    this->xmlFilePath = xmlFilePath;
    this->farPlane = farPlane;
}

Level::Level() {
}

Level::~Level() {
    if (luaState!=nullptr) {
        lua_close(luaState);
    }
    delete(waterPlane);
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
        .addFunction("getObjectCount", &Level::getPhysicsObjectsVectorSize)
        .addFunction("moveObject", &Level::moveObject)
        .addFunction("resetPlayer", &Level::resetPlayer)
        .addFunction("movePlayer", &Level::movePlayer)
        .addFunction("setSunDirection", &Level::setSunDirection)
        .addFunction("forceMove", &Level::forceMove)
        .addFunction("activateEndgame", &Level::activateEndgame)
        .addFunction("preloadLightPosition", &Level::preloadLightPosition)
        .addFunction("addLightByParameters", &Level::addLightByParameters)
        .addFunction("deleteFourLights", &Level::deleteFourLights)
        .endClass();
    //Push the level to Lua as a global variable
    luabridge::push(luaState, this);
    lua_setglobal(luaState, "level");
    
    this->camera = Camera(glm::vec2(-0.8f, 0.0f), 3.0f);
}

int Level::checkMaxSurroundingLights() {
    int maxSurroundingLights = 0;
    for(unsigned int i = 0; i<lights.size(); i++) {
        int thisSurroundingLights = 0;
        for(unsigned int j = 0; j<lights.size(); j++) {
            if (glm::distance(lights.at(i)->getPosition(), lights.at(j)->getPosition()) < skydomeSize) {
                thisSurroundingLights++;
            }
        }
        if (thisSurroundingLights > maxSurroundingLights) {
            maxSurroundingLights = thisSurroundingLights;
        }
    }
    return maxSurroundingLights;
}

void Level::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass, 
        glm::vec3 center, int chunkRenderDistance, glm::mat4* viewProjectionMatrix,
        std::vector<glm::mat4>* shadowVPs) {
    std::vector<Chunk*> nearChunks = getSurroundingChunks(center, chunkRenderDistance);
    for(unsigned int i = 0; i<nearChunks.size(); i++) {
        nearChunks.at(i)->render(shader, lightingPass, viewProjectionMatrix, shadowVPs);
    }
    for (unsigned int i = 0; i<crossChunkObjects.size(); i++) {
        crossChunkObjects.at(i)->render(shader, lightingPass, viewProjectionMatrix, shadowVPs);
    }
}

void Level::enqueueObjects(Graphics* graphics) {
    std::vector<Chunk*> nearChunks = getSurroundingChunks(cameraCenter->getPosition(), -1);
    for(unsigned int i = 0; i<nearChunks.size(); i++) {
        graphics->enqueueObjects(nearChunks.at(i)->getSortedObjects());
    }
    graphics->enqueueObjects(&sortedCrossChunkObjects);
}

std::vector<Chunk*> Level::getSurroundingChunks(glm::vec3 center, int chunkRenderDistance) {
    int renderDistance = 0;
    if (chunkRenderDistance < 0) {
        if ((int)farPlane % chunkSize == 0) {
            renderDistance = (((int)skydomeSize)+chunkSize/2)/chunkSize;
        }
        else {
            renderDistance = ((((int)skydomeSize)+chunkSize/2)/chunkSize) + 1;
        }
    }
    else {
        renderDistance = chunkRenderDistance;
    }
    int xPosition = ((int)center.x + (terrain.getHeightmapWidth()/2))/chunkSize;
    int zPosition = ((int)center.z + (terrain.getHeightmapHeight()/2))/chunkSize;
    int xStart = xPosition - renderDistance;
    unsigned int xEnd = xPosition + renderDistance;
    int zStart = zPosition - renderDistance;
    unsigned int zEnd = zPosition + renderDistance;
    if (xStart < 0) {
        xStart = 0;
    }
    if (zStart < 0) {
        zStart = 0;
    }
    if (xEnd >= chunks.size()) {
        xEnd = chunks.size()-1;
    }
    if (zEnd >= chunks.at(0).size()) {
        zEnd = chunks.at(0).size()-1;
    }
    std::vector<Chunk*> vector = std::vector<Chunk*>((xEnd - xStart + 1)*(zEnd - zStart + 1));
    int counter = 0;
    for(unsigned int i = xStart; i<=xEnd; i++) {
        for(unsigned int j = zStart; j<=zEnd; j++) {
            vector.at(counter) = &chunks.at(i).at(j);
            counter++;
        }
    }
    return vector;
}

void Level::sortObjects(int textureCount) {
    for(unsigned int i = 0; i<chunks.size(); i++) {
        for(unsigned int j = 0; j<chunks.at(i).size(); j++) {
            chunks.at(i).at(j).sortObjects(textureCount);
        }
    }
    // init
    sortedCrossChunkObjects = std::vector<std::vector<Object*>>(textureCount);
    for(unsigned int i = 0; i<sortedCrossChunkObjects.size(); i++) {
       sortedCrossChunkObjects.at(i) = std::vector<Object*>(); 
    }
    for(unsigned int i = 0; i<crossChunkObjects.size(); i++) {
        sortedCrossChunkObjects.at(crossChunkObjects.at(i)->getMaterial()->getTextureUnit() - 2)
            .push_back(crossChunkObjects.at(i));
    }
}

void Level::update(float runTimeSinceLastUpdate, float runTime, glm::vec2 mouseDelta, 
        KeyboardState* keyboardState) {
    
    // Ignore first two mouse updates, because they are incorrect
    // DON'T try to move this functionallity elsewhere
    static int i = 0;
    if (i <20) {
        i++;
        mouseDelta.x=mouseDelta.y=0;
    }
    
    const int runs = 4;
    
    if(i>=20)
    {
        mouseDelta.x = -mouseDelta.x;
    }
        
    for(int j = runs; j > 0; j--)
    {
        physics.takeUpdateStep(runTimeSinceLastUpdate/runs);
        
        camera.updateRotation(mouseDelta/100.0f/(float)runs);
        physics.updateCameraPos(mouseDelta, 50/runs, camera.getDistance());
            
        camera.setPosition(physics.getCameraPosition());
        camera.setDirection(physics.getCameraToPlayer());
        
        if(keyboardState->wPressed){
            physics.rollForward(camera.getVector(),strength/runs);
        }
        if(keyboardState->aPressed) {
            physics.rollLeft(camera.getVector(),strength/runs);
        }
        if(keyboardState->sPressed) {
            physics.rollBack(camera.getVector(),strength/runs);
        }
        if(keyboardState->dPressed){
            physics.rollRight(camera.getVector(),strength/runs);
        }
     }
        
        if(keyboardState->f1Pressed) {
            physics.forcePlayer(glm::vec3(17.5, 21.0, 87.0));
        }
        if(keyboardState->f2Pressed) {
            physics.forcePlayer(glm::vec3(-78.5, 21.75, 4.5));
        }
        if(keyboardState->f3Pressed) {
            physics.forcePlayer(glm::vec3(-169.5, 21.5, 58.5));
        }
        if(keyboardState->f4Pressed) {
            physics.forcePlayer(glm::vec3(-180.5, 21.75, 58.5));
        }

        if(keyboardState->kPressed)
            camera.setIsPhysicsCamera(true);
        if(keyboardState->lPressed)
            camera.setIsPhysicsCamera(false);
        
        cameraCenter->setPosition(physics.getPos(0));
        cameraCenter->setRotation(physics.getRotation(0));
        
        for(unsigned i = 0; i < physicsObjects.size();i++)
        {
            physicsObjects[i]->setPosition(physics.getPos(i));
            physicsObjects[i]->setRotation(physics.getRotation(i));
        }
        
        skydome.setPosition(glm::vec3(cameraCenter->getPosition().x, 
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

std::vector<shared_ptr<Light>>* Level::getLights() {
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

glm::vec4 Level::getFogColourDay() {
    return fogColourDay;
}

glm::vec4 Level::getFogColourRise() {
    return fogColourRise;
}

glm::vec4 Level::getFogColourNight() {
    return fogColourNight;
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

std::vector<Object*>* Level::getAllObjects() {
    return &allObjects;
}

std::vector<Object*>* Level::getPhysicsObjects() {
    return &physicsObjects;
}

void Level::moveObject(int objectIndex, float strength, float xPos, float yPos, float zPos){
    glm::vec3 position = glm::vec3(xPos, yPos, zPos);
    physics.removePositionConstraint(objectIndex);
    physics.addPositionConstraint(objectIndex, strength, position);
}

void Level::resetPlayer(){
    Loader loader = Loader();
    glm::vec3 newPosition = loader.reloadPlayerPosition(xmlFilePath, this);
    physics.forcePlayer(newPosition);
}

void Level::movePlayer(float xPosition, float yPosition, float zPosition){
    glm::vec3 newPosition = glm::vec3(xPosition, yPosition, zPosition);
    physics.forcePlayer(newPosition);
}

void Level::setPlayerIndex(int index){
    playerIndex = index;
}

void Level::setStrength(float strength) {
    this->strength = strength;
}

void Level::setSkydomeObject(Skydome object){
    this->skydome = object;
}

void Level::addObject(Object* object, bool crossesChunks) {
    allObjects.push_back(object);
    if (crossesChunks) {
        crossChunkObjects.push_back(object);
    }
    else {
        int xPosition = ((int)object->getPosition().x + (terrain.getHeightmapWidth()/2))/chunkSize;
        int zPosition = ((int)object->getPosition().z + (terrain.getHeightmapHeight()/2))/chunkSize;
        chunks.at(xPosition).at(zPosition).addObject(object);
    }
}

void Level::addToSpecificChunk(Object* object, int xPosition, int zPosition) {
    chunks.at(xPosition).at(zPosition).addObject(object);
}

void Level::addPhysicsObject(Object* object) {
    physicsObjects.push_back(object);
}

void Level::setAmbientLight(glm::vec3 colour) {
    this->ambientLight = colour;
}

void Level::setFogColourDay(glm::vec4 colour) {
    this->fogColourDay = colour;
}

void Level::setFogColourRise(glm::vec4 colour) {
    this->fogColourRise = colour;
}

void Level::setFogColourNight(glm::vec4 colour) {
    this->fogColourNight = colour;
}

void Level::setDirectionalLight(Light light) {
    this->directionalLight = light;
}

void Level::setSunDirection(float x, float y, float z){
    glm::vec3 lightPosition = glm::vec3(x,y,z);
    glm::vec3 lightColour = this->directionalLight.getColour();
    float lightIntensity = this->directionalLight.getIntensity();
    this->directionalLight = Light(lightPosition, lightColour, lightIntensity);
}

Physics* Level::getPhysics() {
    return &physics;
}

unsigned int Level::getPhysicsObjectsVectorSize() {
    return physicsObjects.size();
}

void Level::setCameraCenter(Object* object) {
    this->cameraCenter = object;
}

void Level::addLight(Light light) {
    shared_ptr<Light> add_light = shared_ptr<Light>(new Light(light));
    this->lights.push_back(add_light);
}

void Level::preloadLightPosition(float xPos, float yPos, float zPos){
    nextLightPosition = glm::vec3(xPos, yPos, zPos);
}

void Level::addLightByParameters(float redColour, float greenColour, float blueColour,  float intensity, float flameYOffset, float flameHeight, float flameWidth){
    glm::vec3 colour = glm::vec3(redColour, greenColour, blueColour);
    this->addLight(Light(nextLightPosition, colour, intensity, flameYOffset, flameHeight, flameWidth));
}

void Level::deleteFourLights(){
    int indice = lights.size()-4;
    lights.erase(lights.begin() + indice);
    lights.erase(lights.begin() + indice);
    lights.erase(lights.begin() + indice);
    lights.erase(lights.begin() + indice);
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

Skydome* Level::getSkydome() {
    return &skydome;
}

void Level::forceMove(float x, float y, float z, unsigned indice){
    glm::vec3 position = glm::vec3(x,y,z);
    physics.forceMove(position, indice);
}

void Level::activateEndgame(){
    physics.activateEndgame();
}

void Level::setWaterPlane(Object* water) {
    this->waterPlane = water;
}

void Level::setTerrain(Terrain terrain) {
    this->terrain = terrain;
}

void Level::printPosition() {
    printf("Player position: %2.2f, %2.2f, %2.2f\n", cameraCenter->getPosition().x,
            cameraCenter->getPosition().y, cameraCenter->getPosition().z);
}

void Level::generateChunks(int chunkSize) {
    this->chunkSize = chunkSize;
    int numberChunksX = 0;
    if (terrain.getHeightmapWidth() % chunkSize == 0) {
        numberChunksX = terrain.getHeightmapWidth()/chunkSize;
    }
    else {
        numberChunksX = (terrain.getHeightmapWidth()/chunkSize) + 1;
    }
    int numberChunksZ = 0;
    if (terrain.getHeightmapHeight() % chunkSize == 0) {
        numberChunksZ = terrain.getHeightmapHeight()/chunkSize;
    }
    else {
        numberChunksZ = (terrain.getHeightmapHeight()/chunkSize) + 1;
    }
    chunks = std::vector<std::vector<Chunk>> (numberChunksX);
    for(int i = 0; i<numberChunksX; i++) {
        std::vector<Chunk> zChunks = std::vector<Chunk>(numberChunksZ);
        for(int j = 0; j<numberChunksZ; j++) {
            zChunks.at(j) = Chunk();
        }
        chunks.at(i) = zChunks;
    }
}

std::vector<std::vector<Chunk>>* Level::getChunks() {
    return &chunks;
}

Object* Level::getWaterPlane() {
    return waterPlane;
}

bool Level::compareLightDistances(shared_ptr<Light> a, shared_ptr<Light> b) {
    if (glm::distance(cameraCenter->getPosition(), a->getPosition()) <
            glm::distance(cameraCenter->getPosition(), b->getPosition())) {
        return true;
    }
    else {
        return false;
    }
}

std::vector<shared_ptr<Light>>* Level::getClosestLights(unsigned int maximumAmount) {
    closestLights = std::vector<shared_ptr<Light>>(lights);
    std::sort(closestLights.begin(),
        closestLights.end(),
        [this](shared_ptr<Light> a, shared_ptr<Light> b) {return compareLightDistances(a, b); });
    if (lights.size() > maximumAmount) {
        closestLights = std::vector<shared_ptr<Light>>(&closestLights[0],
                &closestLights[maximumAmount]);
    }
    // sort pointers for faster comparisons
    std::sort(closestLights.begin(), closestLights.end());
    return &closestLights;
}
