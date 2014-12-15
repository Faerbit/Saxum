#include "trigger.hh"

Trigger::Trigger(std::vector<float> position, float distance, bool isBigger, Object* object, void (*functionPointer)()) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    this->functionPointer=functionPointer;
}

Trigger::Trigger(){
}

Trigger::~Trigger(){
}

void Trigger::triggerUpdate(){

}
