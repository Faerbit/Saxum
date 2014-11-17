#ifndef LEVEL_HH_INCLUDED
#define LEVEL_HH_INCLUDED

#include <string>
#include "object.hh"
#include "light.hh"
#include "entity.hh"
#include "terrain.hh"
#include "material.hh"
#include "camera.hh"
#include "physics.hh"

class Level {
    public:
        Level(std::string filePath);
        Level();
        ~Level();
        void load(ACGL::OpenGL::SharedShaderProgram shader); // Shader is necessary for correct texture assigning
        void update(float runTime, glm::vec2 mouseDelta,bool wPressed, bool aPressed,bool sPressed, bool dPressed);
        void render();
        glm::vec3 getAmbientLight();
        std::vector<Light> getLights();
        Object* getCameraCenter();
        Camera* getCamera();
    private:
        std::string filePath;
        std::vector<Object> objects;
        std::vector<Light> lights;
        glm::vec3 ambientLight;
        Object* cameraCenter;
        Physics physics;
        Camera camera;
        Terrain terrain;
};

#endif
