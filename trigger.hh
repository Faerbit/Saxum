#ifndef TRIGGER_HH_INCLUDED
#define TRIGGER_HH_INCLUDED

#include <vector>
#include "object.hh"

class Level;

class Trigger {
    public:
        Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, int functionPointer_int, Level* level);
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
        void trigger_function_0(Level* level);
        void trigger_function_1(Level* level);
        void trigger_function_2(Level* level);
        void trigger_function_3(Level* level);
        void trigger_function_4(Level* level);
};

#endif
