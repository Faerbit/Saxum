#ifndef TRIGGER_HH_INCLUDED
#define TRIGGER_HH_INCLUDED

#include <vector>

class Trigger {
    public:
        Trigger(std::vector<float> position, float distance, bool isBigger, int objectIndex, int functionPointer);
	    Trigger();
        ~Trigger();
    private:
        
};

#endif
