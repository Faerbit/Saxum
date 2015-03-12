#include "light.hh"

Light::Light(glm::vec3 position, glm::vec3 colour, float intensity, float flameYOffset, float flameHeight, float flameWidth)
    : Entity(position, glm::vec3(0.0f, 0.0f, 0.0f)) {
    this->colour = colour;
    this->intensity = intensity;
    if (flameYOffset != 0.0f) {
        this->withFlame = true;
        glm::vec3 flamePosition = glm::vec3(position.x, position.y + flameYOffset, position.z);
        glm::vec3 flameSize = glm::vec3(flameWidth, flameHeight, flameWidth);
        this->flame = Flame(flamePosition, colour, flameSize);
    }
    else {
        this->withFlame = false;
    }
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

Flame* Light::getFlame() {
    return &flame;
}

bool Light::isFlame() {
    return withFlame;
}
