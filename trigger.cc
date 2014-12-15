#include "trigger.hh"

Trigger::Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, void (*functionPointer)()) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    this->functionPointer = functionPointer;   
}

Trigger::Trigger(){
}

Trigger::~Trigger(){
}

void Trigger::triggerUpdate(){
    if (isBigger && (glm::distance(object->getPosition(), position) > distance)) {
        (*functionPointer)();
    }
    else if (!isBigger && (glm::distance(object->getPosition(), position) < distance)) {
        (*functionPointer)();
    }
}
