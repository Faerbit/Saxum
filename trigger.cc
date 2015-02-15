#include "trigger.hh"

Trigger::Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, std::string luaScript, lua_State* luaState, int objectToChange, std::string scriptPath) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    this->luaScript= scriptPath + luaScript;
    this->luaState = luaState;
    if(luaState == nullptr){
        printf("The Lua state is NULL in trigger!\n");
    }
    this->objectToChange = objectToChange;
}

Trigger::Trigger(){
}

Trigger::~Trigger(){
}

void Trigger::triggerUpdate(){
        if (isBigger && (glm::distance(object->getPosition(), position) > distance)) {
            luaL_dofile(luaState, luaScript.c_str());
            luabridge::getGlobal(luaState, "trigger")(objectToChange);
        }
        else if (!isBigger && (glm::distance(object->getPosition(), position) < distance)) {
            luaL_dofile(luaState, luaScript.c_str());
            luabridge::getGlobal(luaState, "trigger")(objectToChange);
        }
}

bool Trigger::deleteNotification(int deletedObjectIndex){
    if (deletedObjectIndex < objectToChange){
        objectToChange-=1;
    }
    if (deletedObjectIndex == objectToChange){
        return true;
    }
    return false;
}
