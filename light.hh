#ifndef LIGHT_HH_INCLUDED
#define LIGHT_HH_INCLUDED

#include "entity.hh"
#include <ACGL/Math/Math.hh>

//To distinguish various light types
enum LightType {POINT, AMBIENT};

class Light : Entity {
    public:
        Light(glm::vec3 positon, glm::vec3 rotation, glm::vec3 colour, float intensity);
        ~Light();
    private:
        float intensity;
        glm::vec3 colour;
        Type type;
};

#endif
