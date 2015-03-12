#ifndef TRIGGER_HH_INCLUDED
#define TRIGGER_HH_INCLUDED

#include <vector>
#include "object.hh"
extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
#include "LuaBridge.h"

class Level;

class Trigger {
    public:
        Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, std::string luaScript, lua_State* L, int objectToChange, std::string scriptPath, bool undo);
        Trigger();
        ~Trigger();
        void triggerUpdate();
        bool deleteNotification(int deletedObjectIndex);
    private:
        glm::vec3 position;
        float distance;
        bool isBigger;
        Object* object;
        std::string luaScript;
        lua_State* luaState;
        int objectToChange;
        bool undo;
};

#endif
