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
    updatePosition();
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

