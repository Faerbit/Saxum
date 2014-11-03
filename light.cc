#include "light.hh"

Light::Light(glm::vec3 position, glm::vec3 rotation, glm::vec3 colour, float intensity)
    : Entity(position, rotation) {
    this->colour = colour;
    this->intensity = intensity;
}

Light::~Light() {
}

glm::vec3 Light::getColour() {
    return colour;
}

float Light::getIntensity() {
    return intensity;
}
