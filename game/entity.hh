#ifndef ENTITY_HH_INCLUDED
#define ENTITY_HH_INCLUDED

#include <ACGL/Math/Math.hh>
#include <mutex>

class Entity {
    public:
        Entity(glm::vec3 position, glm::vec3 rotation);
        Entity(glm::vec3 position, glm::mat4 rotation);
        Entity(const Entity &other);
        Entity(Entity &&other);
        Entity& operator= (const Entity& other);
        Entity& operator= (Entity &&other);
        Entity();
        ~Entity();
        void setPosition(glm::vec3 positon);
        void setRotation(glm::vec3 rotation);
        void setRotation(glm::mat4 rotation);
        glm::vec3 getPosition();
        glm::mat4 getRotation();
    private:
        glm::vec3 position;
        glm::mat4 rotation;
        mutable std::mutex mutex;
};

#endif
