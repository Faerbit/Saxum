#include "camera.hh"

Camera::Camera(glm::vec3 rotation, float distance) {
    this->rotation = rotation;
    this->distance = distance;
}

Camera::Camera() {
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
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

glm::vec3 Camera::getRotation() {
    return rotation;
}

void Camera::updateRotation(glm::vec3 rotation) {
    this->rotation += rotation;;
}
