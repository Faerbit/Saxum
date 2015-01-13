#include "trigger.hh"

Trigger::Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, std::string luaScript, lua_State* L, int objectToChange) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    this->luaScript="../Levels/scripts/" + luaScript + ".lua";
    this->L = L;
    if(L == nullptr){
        printf("L is NULL in trigger!\n");
    }
    this->objectToChange = objectToChange;
    triggered = false;
}

Trigger::Trigger(){
}

Trigger::~Trigger(){
}

void Trigger::triggerUpdate(){
    if (!triggered){
        if (isBigger && (glm::distance(object->getPosition(), position) > distance)) {
            luaL_dofile(L, luaScript.c_str());
            luabridge::getGlobal(L, "trigger")(objectToChange);
            triggered = true;
        }
        else if (!isBigger && (glm::distance(object->getPosition(), position) < distance)) {
            luaL_dofile(L, luaScript.c_str());
            luabridge::getGlobal(L, "trigger")(objectToChange);
            triggered = true;
        }
    }
}
