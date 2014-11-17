#ifndef LIGHT_HH_INCLUDED
#define LIGHT_HH_INCLUDED

#include "entity.hh"
#include <ACGL/Math/Math.hh>

class Light : public Entity {
    public:
        Light(glm::vec3 position, glm::vec3 colour, float intensity);
        Light();
        glm::vec3 getColour();
        float getIntensity();
        ~Light();
    private:
        float intensity;
        glm::vec3 colour;
};

#endif
