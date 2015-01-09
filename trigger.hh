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
        Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, int functionPointer_int, lua_State* L, Level* level);
	    Trigger();
        ~Trigger();
        void triggerUpdate();
    private:
        glm::vec3 position;
        float distance;
        bool isBigger;
        Object* object;
        void (Trigger::*functionPointer)(Level*);
        Level* level;
        bool triggered;
        lua_State* L;
        void trigger_function_0(Level* level);
        void trigger_function_1(Level* level);
        void trigger_function_2(Level* level);
        void trigger_function_3(Level* level);
        void trigger_function_4(Level* level);
};

#endif
