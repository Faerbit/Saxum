#include "trigger.hh"
#include "level.hh"

Trigger::Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, std::string luaScript, lua_State* L, int objectToChange, Level* level) {
    this->position=position;
    this->distance=distance;
    this->isBigger=isBigger;
    this->object=object;
    this->triggerLuaScript=("../Levels/scripts/" + luaScript + ".lua").c_str();
    printf("Hallo\n");
    printf(this->triggerLuaScript);
    printf("Hallo\n");
    printf("\n");
    this->level = level;
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
            printf(triggerLuaScript);
            printf("\n");
            printf(triggerLuaScript);
            printf("\nTriggering\n");
            //luaL_dofile(L, luaScript);
            //luabridge::getGlobal(L, "trigger")(objectToChange);
            triggered = true;
        }
        else if (!isBigger && (glm::distance(object->getPosition(), position) < distance)) {
            printf(triggerLuaScript);
            printf("\n");
            printf(triggerLuaScript);
            printf("\nTriggering\n");
            //luaL_dofile(L, luaScript);
            //luabridge::getGlobal(L, "trigger")(objectToChange);
            triggered = true;
        }
    }
}

void Trigger::trigger_function_0(Level* level) {
    if (!triggered) {
        int rand = std::rand() % level->getObjects()->size();
        level->getObjects()->erase(level->getObjects()->begin() + rand);
    }
}

void Trigger::trigger_function_1(Level* level) {
}

void Trigger::trigger_function_2(Level* level) {
}

void Trigger::trigger_function_3(Level* level) {
}

void Trigger::trigger_function_4(Level* level) {
}
