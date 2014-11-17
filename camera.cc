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
}

glm::vec2 Camera::getRotation() {
    return rotation;
}

void Camera::setRotation(glm::vec2 rotation) {
    this->rotation = rotation;
}

void Camera::updateRotation(glm::vec2 rotation) {
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
}
