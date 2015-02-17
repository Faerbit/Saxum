#include "flame.hh"

Flame::Flame(float offset, glm::vec3 position, glm::vec3 rotation) :
        Entity(position, rotation) {
    this->offset = offset;
}

Flame::Flame() {
}

Flame::~Flame() {
}

float Flame::getOffset(){
    return offset;
}
