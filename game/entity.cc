#include "entity.hh"

Entity::Entity(glm::vec3 position, glm::vec3 rotation) {
    this->position = position;
    setRotation(rotation);
}

Entity::Entity(glm::vec3 position, glm::mat4 rotation) {
    this->position = position;
    this->rotation = rotation;
}

Entity::Entity(const Entity &other) {
    std::lock_guard<std::mutex> lock(other.mutex);
    position = other.position;
    rotation = other.rotation;
}

Entity::Entity(Entity &&other) {
    std::lock_guard<std::mutex> lock(other.mutex);
    position = std::move(other.position);
    rotation = std::move(other.rotation);
}

Entity& Entity::operator= (const Entity &other) {
    std::lock(mutex, other.mutex);
    std::lock_guard<std::mutex> self_lock(mutex, std::adopt_lock);
    std::lock_guard<std::mutex> other_lock(other.mutex, std::adopt_lock);
    position = other.position;
    rotation = other.rotation;
    return *this;
}

Entity& Entity::operator= (Entity &&other) {
    std::lock(mutex, other.mutex);
    std::lock_guard<std::mutex> self_lock(mutex, std::adopt_lock);
    std::lock_guard<std::mutex> other_lock(other.mutex, std::adopt_lock);
    position = std::move(other.position);
    rotation = std::move(other.rotation);
    return *this;
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
