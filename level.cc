#include "level.hh"

Level::Level(std::string filePath){
    this->filePath = filePath;
    this->terrain = Terrain(filePath + "/terrain");
}

Level::Level() {
}

Level::~Level() {
}

void Level::load(ACGL::OpenGL::SharedShaderProgram shader) {
    // currently hard coded should later read this stuff out of a file
    this->camera = Camera(glm::vec2(-0.8f, 0.0f), 3.0f);
    // load the geometry of the stanford bunny and build a VAO:
    Model model = Model("Bunny.obj", 0.25f);
    // load a texture:
    Material material = Material("clownfishBunny.png", 0.1f, 0.5f, 0.5f, 3.0f);
    //Create object
    Object object = Object(model, material, glm::vec3(0.0f, 0.0f, -2.0f),
        glm::vec3(0.0f, 1.0472f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);


    //set lighting parameters
    ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    Light light = Light(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f);
    lights.push_back(light);
    Light light2 = Light(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 2.0f);
    lights.push_back(light2);


    // load terrain
    this->terrain.load();
    Model terrainModel = this->terrain.getModel();
    // load a texture:
    Material terrainMaterial = Material("clownfishBunny.png", 0.1f, 0.7f, 0.3f, 1.0f);
    //Create object
    Object terrainObject = Object(terrainModel, terrainMaterial,
	glm::vec3(-0.5f*(float)this->terrain.getHeightmapHeight(), 0.0f, -0.5f*(float)this->terrain.getHeightmapWidth()),
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(object);
    objects.push_back(terrainObject);
    cameraCenter = &objects[0];
}

void Level::render() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        objects[i].render();
    }
//    this->terrain.render();
}

void Level::update(float runTime, glm::vec2 mouseDelta) {
    // rotate bunny
    //cameraCenter->setRotation(glm::vec3(0.0f, 1.0472f * runTime, 0.0f));
    camera.updateRotation(mouseDelta/50.0f);
}

glm::vec3 Level::getAmbientLight() {
    return ambientLight;
}

std::vector<Light> Level::getLights() {
    return lights;
}

Camera Level::getCamera() {
    return camera;
}

Object* Level::getCameraCenter() {
    return cameraCenter;
}
