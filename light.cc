#include "light.hh"

Light::Light(glm::vec3 position, glm::vec3 colour, float intensity, float flameOffset)
    : Entity(position, glm::vec3(0.0f, 0.0f, 0.0f)) {
    this->colour = colour;
    this->intensity = intensity;
    this->flameOffset = flameOffset;
}

Light::Light() {
}

Light::~Light() {
}

glm::vec3 Light::getColour() {
    return colour;
}

float Light::getIntensity() {
    return intensity;
}

float Light::getFlameOffset() {
    return flameOffset;
}
