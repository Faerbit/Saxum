#ifndef CAMERA_HH_INCLUDED
#define CAMERA_HH_INCLUDED

#include <ACGL/Math/Math.hh>

class Camera {
    public:
        Camera(glm::vec2 rotation, float distance);
        Camera();
        ~Camera();
        float getDistance();
        void setDistance(float distance);
        void updateDistance(float distance); //adds to current distance
        glm::vec2 getRotation();
        void setRotation(glm::vec2 rotation);
        void updateRotation(glm::vec2 rotation); //adds to current rotation
        glm::vec3 getVector();
        void setPosition(glm::vec3 pos);
        glm::vec3 getPosition();
        void setDirection(glm::vec3 dir);
        glm::vec3 getDirection();
        
    private:
        void updatePosition();
        float distance;
        glm::vec2 rotation;
        glm::vec3 vector;
        glm::vec3 position;
        glm::vec3 direction;
};

#endif
