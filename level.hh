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
#include "tinyxml2.hh"

class Level {
    public:
        Level(std::string levelNum);
        Level();
        ~Level();
        void load();
        void update(float runTime, glm::vec2 mouseDelta,bool wPressed, bool aPressed,bool sPressed, bool dPressed);
        void render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass);
        glm::vec3 getAmbientLight();
        Light* getDirectionalLight();
        std::vector<Light>* getLights();
        Object* getCameraCenter();
        Camera* getCamera();
        glm::vec3 getCameraPosition();
        glm::vec4 getFogColour();
        void setSkydomeSize(float size);
    private:
        void errorCheck(tinyxml2::XMLError error);
        std::string levelNum;
        std::vector<Object*> objects;
        std::vector<Object*> physicObjects;
        std::vector<Light> lights;
        glm::vec3 ambientLight;
        glm::vec4 fogColour;
        Light directionalLight;
        Object* cameraCenter;
        Object* skydome;
        Physics physics;
        Camera camera;
        Terrain terrain;
        float skydomeSize;
};

#endif
