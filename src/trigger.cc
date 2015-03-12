#include "trigger.hh"
#include <sys/stat.h>

Trigger::Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, std::string luaScript, lua_State* luaState, int objectToChange, std::string scriptPath, bool undo) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    this->luaScript= scriptPath + luaScript;
    struct stat buf;
    if(stat(this->luaScript.c_str(), &buf) != 0){
        std::cout << "The lua file " << this->luaScript << " does not exist." << std::endl;
        exit(-1);
    }
    this->luaState = luaState;
    if(luaState == nullptr){
        printf("The Lua state is NULL in trigger!\n");
    }
    this->objectToChange = objectToChange;
    this->undo = undo;
}

Trigger::Trigger(){
}

Trigger::~Trigger(){
}

void Trigger::triggerUpdate(){
    if (isBigger && (glm::distance(object->getPosition(), position) > distance)) {
        int error = luaL_dofile(luaState, luaScript.c_str());
        if (error != 0) {
            std::cout << "Couldn't load file: " << this->luaScript << std::endl;
            exit(-1);
        }
        if (undo){
            luabridge::getGlobal(luaState, "triggerUndo")(objectToChange);
        }else{
            luabridge::getGlobal(luaState, "trigger")(objectToChange);
        }
    }
    else if (!isBigger && (glm::distance(object->getPosition(), position) < distance)) {
        int error = luaL_dofile(luaState, luaScript.c_str());
        if (error != 0) {
            std::cout << "Couldn't load file: " << this->luaScript << std::endl;
            exit(-1);
        }
        if (undo){
            luabridge::getGlobal(luaState, "triggerUndo")(objectToChange);
        }else{
            luabridge::getGlobal(luaState, "trigger")(objectToChange);
        }
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
