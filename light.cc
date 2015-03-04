#include "light.hh"

Light::Light(glm::vec3 position, glm::vec3 colour, float intensity, float flameYOffset, float flameHeight, float flameWidth)
    : Entity(position, glm::vec3(0.0f, 0.0f, 0.0f)) {
    this->colour = colour;
    this->intensity = intensity;
    this->flameYOffset = flameYOffset;
    this->flameHeight = flameHeight;
    this->flameWidth = flameWidth;
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

float Light::getFlameYOffset() {
    return flameYOffset;
}
