#ifndef LIGHT_HH_INCLUDED
#define LIGHT_HH_INCLUDED

#include "entity.hh"
#include <ACGL/Math/Math.hh>

class Light : public Entity {
    public:
        Light(glm::vec3 position, glm::vec3 colour, float intensity, float flameYOffset = 0.0f, float flameHeight = 0.0f, float flameWidth = 0.0f);
        Light();
        glm::vec3 getColour();
        float getIntensity();
        float getFlameYOffset();
        ~Light();
    private:
        float flameYOffset;
        float flameHeight;
        float flameWidth;
        float intensity;
        glm::vec3 colour;
};

#endif
