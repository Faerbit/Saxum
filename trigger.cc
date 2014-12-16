#include "trigger.hh"
#include "level.hh"

Trigger::Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, int functionPointer_int, Level* level) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    void (*functionPointer)(Level*) = NULL;
    switch(functionPointer_int) {
        case 0:
            this->functionPointer =  &Trigger::trigger_function_0;
            break;
        case 1:
            this->functionPointer =  &Trigger::trigger_function_1;
            break;
        case 2:
            this->functionPointer =  &Trigger::trigger_function_2;
            break;
        case 3:
            this->functionPointer =  &Trigger::trigger_function_3;
            break;
        case 4:
            this->functionPointer =  &Trigger::trigger_function_4;
            break;
        default:
            printf("Trigger function could not be found.\n");
    }
    this->level = level;
}

Trigger::Trigger(){
}

Trigger::~Trigger(){
}

void Trigger::triggerUpdate(){
    if (isBigger && (glm::distance(object->getPosition(), position) > distance)) {
        (this->*functionPointer)(level);
    }
    else if (!isBigger && (glm::distance(object->getPosition(), position) < distance)) {
        (this->*functionPointer)(level);
    }
}

void Trigger::trigger_function_0(Level* level) {
    static bool triggered = false;
    if (!triggered) {
        int rand = std::rand() % level->getObjects()->size();
        level->getObjects()->erase(level->getObjects()->begin() + rand);
    }
    triggered = true;
}

void Trigger::trigger_function_1(Level* level) {
}

void Trigger::trigger_function_2(Level* level) {
}

void Trigger::trigger_function_3(Level* level) {
}

void Trigger::trigger_function_4(Level* level) {
}
