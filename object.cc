#include "object.hh"

Object::Object(Model model, Texture texture, glm::vec3 position, glm::vec3 rotation, 
        glm::vec3 velocity, glm::vec3 angularVelocity, Shader shader) :
        Entity(position, rotation) {
    this->model = model.getReference();
    this->texture = texture.getReference();
    this->velocity = velocity;
    this->angularVelocity = angularVelocity;
    this->shader = shader.getReference();
}

Object::~Object() {
}

void Object::render() {
    shader->setTexture("uTexture", texture, 0);
}
