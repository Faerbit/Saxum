#ifndef ENTITY_HH_INCLUDED
#define ENTITY_HH_INCLUDED

#include <ACGL/Math/Math.hh>

class Entity {
    public:
        Entity(glm::vec3 position, glm::vec3 rotation);
        ~Entity();
        void setPosition(glm::vec3 positon);
        void setRotation(glm::vec3 rotation);
        glm::vec3 getPosition();
        glm::vec3 getRotation();
    private:
        glm::vec3 position;
        glm::vec3 rotation;
};

#endif
