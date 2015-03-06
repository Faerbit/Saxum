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
#include "trigger.hh"
#include "skydome.hh"

extern "C" {
#include "extern/lua/src/lua.h"
#include "extern/lua/src/lualib.h"
#include "extern/lua/src/lauxlib.h"
}
#include "extern/luabridge/LuaBridge.h"

class Level {
    public:
        Level(std::string heightmapFilePath, std::string xmlFilePath);
        Level();
        ~Level();
        void load();
        void update(float runTimeSinceLastUpdate, float runTime, glm::vec2 mouseDelta,bool wPressed, bool aPressed,bool sPressed, bool dPressed, bool kPressed, bool lPressed);
        void render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass,
            glm::mat4* viewProjectionMatrix, std::vector<glm::mat4>* shadowVPs=0);
        glm::vec3 getAmbientLight();
        Light* getDirectionalLight();
        std::vector<Light>* getLights();
        Object* getCameraCenter();
        Camera* getCamera();
        glm::vec3 getCameraPosition();
        glm::vec4 getFogColour();
        void setSkydomeSize(float size);
        float getSkydomeSize();
        Skydome* getSkydome();
        std::vector<Object*>* getObjects();
        std::vector<Object*>* getPhysicsObjects();
        void deleteObject(int objectIndex);
        void moveObject(int objectIndex, float strength, float xPos, float yPos, float zPos);
        void setStrength(float strength);
        void setSkydomeObject(Skydome object);
        void addObject(Object* object);
        void addPhysicsObject(Object* object);
        void setAmbientLight(glm::vec3 colour);
        void setFogColour(glm::vec4 colour);
        void setDirectionalLight(Light light);
        void setSunDirection(float x, float y, float z);
        Physics* getPhysics();
        unsigned int getObjectsVectorSize();
        unsigned int getPhysicsObjectsVectorSize();
        void setCameraCenter(Object* object);
        void addLight(Light light);
        void addTrigger(Trigger trigger);
        lua_State* getLuaState();
        Terrain* getTerrain();
        void resetPlayer();
        void movePlayer(float xPosition, float yPosition, float zPosition);
        void setPlayerIndex(int index);
        void forceMove(float x, float y, float z, unsigned indice);
    private:
        lua_State* luaState=nullptr;
        std::vector<Object*> objects;
        std::vector<Object*> physicsObjects;
        std::vector<Light> lights;
        std::vector<Trigger> triggers;
        glm::vec3 ambientLight;
        glm::vec4 fogColour;
        Light directionalLight;
        Object* cameraCenter;
        int playerIndex;
        Skydome skydome;
        Physics physics;
        Camera camera;
        Terrain terrain;
        float skydomeSize;
        float strength;
        std::string xmlFilePath;
};

#endif
