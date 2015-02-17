#ifndef FLAME_HH_INCLUDED
#define FLAME_HH_INCLUDED

#include "entity.hh"


class Flame : public Entity {
    public:
        Flame(float offset, glm::vec3 position, glm::vec3 rotation);
        Flame();
        ~Flame();
        float getOffset();
    private:
        float offset;
};

#endif
