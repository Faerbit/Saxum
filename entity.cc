#include "entity.hh"

Entity::Entity(glm::vec3 position, glm::vec3 rotation) {
    this->position = position;
    setRotation(rotation);
}

Entity::Entity(glm::vec3 position, glm::mat4 rotation) {
    this->position = position;
    this->rotation = rotation;
}

Entity::Entity(){
}

Entity::~Entity(){
}

glm::vec3 Entity::getPosition() {
    return position;
}

glm::mat4 Entity::getRotation() {
    return rotation;
}

void Entity::setPosition(glm::vec3 position) {
    this->position = position;
}

void Entity::setRotation(glm::vec3 rotation) {
    this->rotation = glm::rotate(rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
        * glm::rotate(rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
        * glm::rotate(rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Entity::setRotation(glm::mat4 rotation) {
    this->rotation = rotation;
}
