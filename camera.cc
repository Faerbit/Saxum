#include "camera.hh"

Camera::Camera(glm::vec2 rotation, float distance) {
    this->rotation = rotation;
    this->distance = distance;
}

Camera::Camera() {
    rotation = glm::vec2(0.0f, 0.0f);
    distance = 1.0f;
}

Camera::~Camera() {
}

float Camera::getDistance() {
    return distance;
}

void Camera::setDistance(float distance) {
    this->distance = distance;
    updatePosition();
}

glm::vec2 Camera::getRotation() {
    return rotation;
}

void Camera::setRotation(glm::vec2 rotation) {
    this->rotation = rotation;
    updatePosition();
}

void Camera::updateRotation(glm::vec2 rotation) {
    this->rotation += rotation;
    if((this->rotation.x + rotation.x) >= 1.57f) {
        this->rotation.x = 1.57;
        this->rotation.y += rotation.y;
    }
    else if ((this->rotation.x + rotation.x) <= -1.57f) {
        this->rotation.x = -1.57f;
        this->rotation.y += rotation.y;
    }
    else {
        this-> rotation += rotation;
    }
    updatePosition();
}

void Camera:: updateDistance(float distance) {
    if (this->distance + distance <= 1.0f) {
        this->distance = 1.0f;
    }
    else if (this->distance + distance >= 30.0f) {
        this->distance = 30.f;
    }
    else {
        this->distance += distance;
    }
    updatePosition();
}

void Camera::setPosition(glm::vec3 pos)
{
    position = pos;    
}

glm::vec3 Camera::getPosition()
{
    return position;
}

void Camera::setDirection(glm::vec3 dir)
{
    direction = dir;
}

glm::vec3 Camera::getDirection()
{
    return direction;
}


void Camera::updatePosition() {
    glm::vec4 cameraVector = glm::vec4(0.0f, 0.0f, distance, 0.0f);
    // rotate vector
    glm::mat4 rotationMatrix =  
        glm::rotate<float>(rotation[1], glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate<float>(rotation[0], glm::vec3(1.0f, 0.0f, 0.0f));
    this->vector = glm::vec3(rotationMatrix * cameraVector);
}

glm::vec3 Camera::getVector() {
    return vector;
}

