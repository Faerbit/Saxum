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
    this->camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
    // load the geometry of the stanford bunny and build a VAO:
    Model model = Model("Bunny.obj", 0.25f);
    // load a texture:
    Material material = Material("clownfishBunny.png", 0.1f, 0.5f, 0.5f, 3.0f);
    //Create object
    Object object = Object(model, material, glm::vec3(0.0f, -1.0f, -2.0f),
        glm::vec3(0.0f, 1.0472f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(object);
    cameraCenter = &objects[0];
    //set lighting parameters
    ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    Light light = Light(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 2.0f);
    lights.push_back(light);
    Light light2 = Light(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
    lights.push_back(light2);
    // load terrain
    this->terrain.load();
}

void Level::render() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        objects[i].render();
    }
    this->terrain.render();
}

void Level::update(float runTime) {
    // rotate bunny
    cameraCenter->setRotation(glm::vec3(0.0f, 1.0472f * runTime, 0.0f));
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
