#include "entity.hh"

Entity::Entity(glm::vec3 position, glm::vec3 rotation) {
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

glm::vec3 Entity::getRotation() {
    return rotation;
}

void Entity::setPosition(glm::vec3 position) {
    this->position = position;
}

void Entity::setRotation(glm::vec3 rotation) {
    this->rotation = rotation;
}
