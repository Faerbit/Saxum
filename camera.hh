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
        glm::vec2 getRotation();
        void setRotation(glm::vec2 rotation);
        void updateRotation(glm::vec2 rotation); //adds to current rotation
        glm::vec3 getVector();
    private:
        void updatePosition();
        float distance;
        glm::vec2 rotation;
        glm::vec3 vector;
};

#endif
