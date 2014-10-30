#include "entity.hh"

Entity::Entity(glm::vec3 position, glm::vec3 rotation) {
    this->position = position;
    this->rotation = rotation;
}

Entity::Entity(){
}

Entity::~Entity(){
}
