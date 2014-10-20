#ifndef OBJECT_HH_INCLUDED
#define OBJECT_HH_INCLUDED

#include "entity.hh"
#include "model.hh"
#include "texture.hh"   
#include <ACGL/Math/Math.hh>

class Object : Entity {
    public:
        Object(Model model, Texture texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 velocity, glm::vec3 angularVelocity);
        ~Object();
    private:
        Model model;
        Texture texture;
        glm::vec3 velocity;
        glm::vec3 angularVelocity;
};

#endif
