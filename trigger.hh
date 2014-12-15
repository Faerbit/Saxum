#ifndef TRIGGER_HH_INCLUDED
#define TRIGGER_HH_INCLUDED

#include <vector>
#include "object.hh"

class Trigger {
    public:
        Trigger(glm::vec3 position, float distance, bool isBigger, Object* object, void (*functionPointer)());
	    Trigger();
        ~Trigger();
        void triggerUpdate();
    private:
        glm::vec3 position;
        float distance;
        bool isBigger;
        Object* object;
        void (*functionPointer)();
};

#endif
