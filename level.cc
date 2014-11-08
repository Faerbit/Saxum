#include "level.hh"

Level::Level(std::string filePath){
    this->filePath = filePath;
    this->terrain = Terrain(filePath + "/terrain");
}

Level::Level() {
}

Level::~Level() {
}

void Level::load(Shader shader) {
    //this->terrain.load();



    // currently hard coded should later read this stuff out of a file
    // load the geometry of the stanford bunny and build a VAO:
    Model model = Model("Bunny.obj");
    // load a texture:
    Texture texture = Texture("clownfishBunny.png");
    //Create object
    Object object = Object(model, texture, glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), shader);
    objects.push_back(object);
    //set lighting parameters
    ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    Light light = Light(glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 2.0f);
    lights.push_back(light);
    Light light2 = Light(glm::vec3(3.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
    lights.push_back(light2);
}

void Level::render() {
    for(int i = 0; i<objects.size(); i++) {
        objects[i].render();
    }
    //this->terrain.render();
}

glm::vec3 Level::getAmbientLight() {
    return ambientLight;
}

std::vector<Light> Level::getLights() {
    return lights;
}
