#ifndef CAMERA_HH_INCLUDED
#define CAMERA_HH_INCLUDED

#include <ACGL/Math/Math.hh>

class Camera {
    public:
        Camera(glm::vec3 rotation, float distance);
        Camera();
        ~Camera();
        float getDistance();
        void setDistance(float distance);
        glm::vec3 getRotation();
        void updateRotation(glm::vec3 rotation); //adds to current rotation
    private:
        float distance;
        glm::vec3 rotation;
};

#endif
