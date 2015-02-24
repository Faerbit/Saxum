#ifndef LIGHT_HH_INCLUDED
#define LIGHT_HH_INCLUDED

#include "entity.hh"
#include <ACGL/Math/Math.hh>

class Light : public Entity {
    public:
        Light(glm::vec3 position, glm::vec3 colour, float intensity, float flameOffset = 0.0f);
        Light();
        glm::vec3 getColour();
        float getIntensity();
        float getFlameOffset();
        ~Light();
    private:
        float flameOffset;
        float intensity;
        glm::vec3 colour;
};

#endif
