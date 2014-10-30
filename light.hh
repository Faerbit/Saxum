#ifndef LIGHT_HH_INCLUDED
#define LIGHT_HH_INCLUDED

#include "entity.hh"
#include <ACGL/Math/Math.hh>

//To distinguish various light types

class Light : Entity {
    public:
        Light(glm::vec3 position, glm::vec3 rotation, glm::vec3 colour, float intensity);
        ~Light();
    private:
        float intensity;
        glm::vec3 colour;
};

#endif
