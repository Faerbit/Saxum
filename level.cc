#include "level.hh"



Level::Level(std::string filePath){
    this->filePath = filePath;
    this->terrain = Terrain(filePath + "/terrain"); //TODO change this, fileSystem got reworked
    skyboxSize = 50.0f;
}

Level::Level() {
}

Level::~Level() {
}

void Level::load(ACGL::OpenGL::SharedShaderProgram shader) {

    this->physics = Physics();
    this->physics.init();
    
    // currently hard coded should later read this stuff out of a file
    this->camera = Camera(glm::vec2(-0.8f, 0.0f), 3.0f);
    // load the geometry of the stanford bunny and build a VAO:
    Model model = Model("MarbleSmooth.obj", 0.75f);
    // load a texture:
    Material material = Material("marbleTexture_small.png", 0.1f, 0.5f, 0.5f, 3.0f);
    //Create object
    Object object = Object(model, material, glm::vec3(0.0f, 10.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);
    //add player to phy    
    this->physics.addPlayer(1.25f,0.0f,10.0f,0.0f,1.0f,0);
    objects.push_back(object);

    Model skyboxModel = Model("skybox.obj", skyboxSize);
    Material skyboxMaterial = Material("skybox.png", 0.7f, 0.0f, 0.0f, 0.0f);
    Object skyboxObject = Object(skyboxModel, skyboxMaterial, glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(skyboxObject);
    
    //physics.addStaticGroundPlane();

    Model torchModel = Model("torch.obj", 0.75f);
    Material torchMaterial = Material("torchTexture.png", 0.1f, 0.3f, 0.7f, 10.0f);
    //Create object
    Object torchObject = Object(torchModel, torchMaterial, glm::vec3(-3.0f, 6.0f, 0.0f),
        glm::vec3(0.0f, 1.0472f, 0.0f), shader);
    objects.push_back(torchObject);
    
    Model blockModel = Model("Block.obj", 1.0f);
    Material blockMaterial = Material("blockTexture_small.png", 0.1f, 0.6, 0.4f, 2.0f);
    Object blockObject = Object(blockModel, blockMaterial, glm::vec3(2.0f, 7.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(blockObject);

    Model columnModel = Model("Column.obj", 1.0f);
    Material columnMaterial = Material("columnTexture_small.png", 0.1f, 0.6, 0.4f, 2.0f);
    Object columnObject = Object(columnModel, columnMaterial, glm::vec3(-2.0f, 7.0f, -2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(columnObject);

    //set lighting parameters
    ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    fogColor = glm::vec4(0.10f, 0.14f, 0.14f, 1.0f);
    directionalLight = Light(glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), 0.4f);
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
    Object terrainObject = Object(terrainModel, terrainMaterial,
	glm::vec3(-0.5f*(float)this->terrain.getHeightmapHeight(), 0.0f, -0.5f*(float)this->terrain.getHeightmapWidth()),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(terrainObject);
    cameraCenter = &objects[0];
    skybox = &objects[1];
    
    //addTerrainPhysic
	physics.addTerrain(terrain.getHeightmapWidth(), terrain.getHeightmapHeight(), terrain.getHeightmap());
}

void Level::render() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        objects[i].render();
    }
}

void Level::update(float runTime, glm::vec2 mouseDelta, bool wPressed, bool aPressed, bool sPressed, bool dPressed) {
    // rotate bunny
    //cameraCenter->setRotation(glm::vec3(0.0f, 1.0472f * runTime, 0.0f));
    // Ignore first two mouse updates, because they are incorrect
    static int i = 0;
    if (i <2) {
        i++;
    }
    else {
        mouseDelta.x = -mouseDelta.x;
        camera.updateRotation(mouseDelta/100.0f);
    }    
    
    if(wPressed){
        physics.rollForward(camera.getVector(),1.0f);
    }
    if(aPressed) {
        physics.rollLeft(camera.getVector(),1.0f);
    }
    if(sPressed) {
        physics.rollBack(camera.getVector(),1.0f);
    }
    if(dPressed){
        physics.rollRight(camera.getVector(),1.0f);
    }
    
    physics.takeUpdateStep(runTime);
    
    objects[0].setPosition(physics.getPos(0));
    objects[0].setRotation(physics.getRotation(0));
    skybox->setPosition(glm::vec3(cameraCenter->getPosition().x, 
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

void Level::setSkyboxSize(float size) {
    skyboxSize = size;
}
