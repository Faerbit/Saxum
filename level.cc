#include "level.hh"



Level::Level(std::string filePath){
    this->filePath = filePath;
    this->terrain = Terrain(filePath + "/terrain");
    skydomeSize = 50.0f;
}

Level::Level() {
}

Level::~Level() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        delete(objects.at(i));
    }
}

void Level::load() {

    this->physics = Physics();
    this->physics.init();
    
    // currently hard coded should later read this stuff out of a file
    this->camera = Camera(glm::vec2(-0.8f, 0.0f), 3.0f);
    
    //add player
    Model model = Model("MarbleSmooth.obj", 0.75f);
    Material material = Material("marbleTexture_small.png", 0.1f, 0.5f, 0.5f, 3.0f);
    Object* object = new Object(model, material, glm::vec3(2.0f, 10.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(object);    
    physicObjects.push_back(object);
    this->physics.addPlayer(1.25f,*object,8.0f,physicObjects.size());
    cameraCenter = object;
   
    Model skydomeModel = Model("skydome.obj", skydomeSize);
    Material skydomeMaterial = Material("skydome.png", 0.7f, 0.0f, 0.0f, 0.0f);
    Object* skydomeObject = new Object(skydomeModel, skydomeMaterial, glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(skydomeObject);
    skydome = skydomeObject;

    Model torchModel = Model("torch.obj", 0.75f);
    Material torchMaterial = Material("torchTexture.png", 0.1f, 0.3f, 0.7f, 10.0f);
    //Create object
    Object* torchObject = new Object(torchModel, torchMaterial, glm::vec3(-3.0f, 6.0f, 0.0f),
        glm::vec3(0.0f, 1.0472f, 0.0f));
    objects.push_back(torchObject);
    
    Model blockModel = Model("Block.obj", 1.0f);
    Material blockMaterial = Material("blockTexture_small.png", 0.1f, 0.6, 0.4f, 2.0f);
    Object* blockObject = new Object(blockModel, blockMaterial, glm::vec3(0.0f, 10.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(blockObject);
    physicObjects.push_back(blockObject);
    physics.addBox(1,3.0f,1,*blockObject,2,physicObjects.size());
    
    Object* blockObject2 = new Object(blockModel, blockMaterial, glm::vec3(5.0f, 10.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(blockObject2);
    physicObjects.push_back(blockObject2);
    physics.addBox(1,3.0f,1,*blockObject2,2,physicObjects.size());

    Model columnModel = Model("Column.obj", 1.0f);
    Material columnMaterial = Material("columnTexture_small.png", 0.1f, 0.6, 0.4f, 2.0f);
    Object* columnObject = new Object(columnModel, columnMaterial, glm::vec3(-2.0f, 7.0f, -2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(columnObject);

    //make non physics objects


    //set lighting parameters
    ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    fogColor = glm::vec4(0.10f, 0.14f, 0.14f, 1.0f);
    directionalLight = Light(glm::vec3(-1.0f, 1.5f, 1.0f), glm::vec3(1.0f, 1.0f, 0.9f), 0.2f);
    Light light = Light(glm::vec3(-3.0f, 7.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 5.0f);
    lights.push_back(light);
    Light light2 = Light(glm::vec3(3.0f, 7.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 10.0f);
    lights.push_back(light2);

    // load terrain
    this->terrain.load();
    Model terrainModel = Model(this->terrain.getModel());
    // load a texture:
    Material terrainMaterial = Material("seamlessTerrain.png", 0.1f, 0.8f, 0.2f, 3.0f);
    //Create object
    Object* terrainObject = new Object(terrainModel, terrainMaterial,
	glm::vec3(-0.5f*(float)this->terrain.getHeightmapHeight(), 0.0f, -0.5f*(float)this->terrain.getHeightmapWidth()),
        glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(terrainObject);
    
    //addTerrainPhysic
	physics.addTerrain(terrain.getHeightmapWidth(), terrain.getHeightmapHeight(), terrain.getHeightmap());
}

void Level::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass) {
    for(unsigned int i = 0; i<objects.size(); i++) {
        // do not project shadow of skydome 
        if(lightingPass || objects.at(i) != skydome) {
            objects.at(i)->render(shader, lightingPass);
        }
    }
}

void Level::update(float runTime, glm::vec2 mouseDelta, bool wPressed, bool aPressed, bool sPressed, bool dPressed) {
    // Ignore first two mouse updates, because they are incorrect
    // DON'T try to move this functionallity elsewhere
    static int i = 0;
    if (i <2) {
        i++;
    }
    else {
        mouseDelta.x = -mouseDelta.x;
        camera.updateRotation(mouseDelta/100.0f);
    }    
    
    float str = 20;
    
    if(wPressed){
        physics.rollForward(camera.getVector(),str);
    }
    if(aPressed) {
        physics.rollLeft(camera.getVector(),str);
    }
    if(sPressed) {
        physics.rollBack(camera.getVector(),str);
    }
    if(dPressed){
        physics.rollRight(camera.getVector(),str);
    }
    
    physics.takeUpdateStep(runTime);
    
    cameraCenter->setPosition(physics.getPos(0));
    cameraCenter->setRotation(physics.getRotation(0));
    
    for(unsigned i = 0; i < physicObjects.size();i++)
    {
        physicObjects[i]->setPosition(physics.getPos(i));
        physicObjects[i]->setRotation(physics.getRotation(i));
    }
    
    skydome->setPosition(glm::vec3(cameraCenter->getPosition().x, 
        0.0f, cameraCenter->getPosition().z));
}

glm::vec3 Level::getAmbientLight() {
    return ambientLight;
}

std::vector<Light> Level::getLights() {
    return lights;
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

glm::vec4 Level::getFogColor() {
    return fogColor;
}

glm::vec3 Level::getCameraPosition() {
   return cameraCenter->getPosition() + camera.getVector();
}

void Level::setSkydomeSize(float size) {
    skydomeSize = size;
}
