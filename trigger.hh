#ifndef TRIGGER_HH_INCLUDED
#define TRIGGER_HH_INCLUDED

#include <vector>
#include "object.hh"
extern "C" {
#include "extern/lua/src/lua.h"
#include "extern/lua/src/lualib.h"
#include "extern/lua/src/lauxlib.h"
}
#include "extern/luabridge/LuaBridge.h"

class Level;

class Trigger {
    public:
        Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, std::string luaScript, lua_State* L, int objectToChange);
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
        lua_State* L;
        int objectToChange;
};

#endif
