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
#include "keyboardState.hh"
#include "chunk.hh"

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
#include "LuaBridge.h"

class Level {
    public:
        Level(std::string xmlFilePath, float farPlane);
        Level();
        ~Level();
        void load();
        void update(float runTimeSinceLastUpdate, float runTime, glm::vec2 mouseDelta,
                KeyboardState* keyboardState);
        void render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass,
            glm::mat4* viewProjectionMatrix, std::vector<glm::mat4>* shadowVPs=0);
        glm::vec3 getAmbientLight();
        Light* getDirectionalLight();
        std::vector<Light>* getLights();
        Object* getCameraCenter();
        Camera* getCamera();
        glm::vec3 getCameraPosition();
        glm::vec4 getFogColourDay();
        glm::vec4 getFogColourRise();
        glm::vec4 getFogColourNight();
        void setSkydomeSize(float size);
        float getSkydomeSize();
        void setWaterPlane(Object* water);
        Skydome* getSkydome();
        std::vector<Object*>* getAllObjects();
        std::vector<Object*>* getPhysicsObjects();
        void moveObject(int objectIndex, float strength, float xPos, float yPos, float zPos);
        void setStrength(float strength);
        void setSkydomeObject(Skydome object);
        void addObject(Object* object);
        void addPhysicsObject(Object* object);
        void setAmbientLight(glm::vec3 colour);
        void setFogColourDay(glm::vec4 colour);
        void setFogColourRise(glm::vec4 colour);
        void setFogColourNight(glm::vec4 colour);
        void setDirectionalLight(Light light);
        void setSunDirection(float x, float y, float z);
        unsigned int getPhysicsObjectsVectorSize();
        Physics* getPhysics();
        void setCameraCenter(Object* object);
        void addLight(Light light);
        void preloadLightPosition(float xPos, float yPos, float zPos);
        void addLightByParameters(float redColour, float greenColour, float blueColour, float intensity, float flameYOffset, float flameHeight, float flameWidth);
        void deleteFourLights();
        void addTrigger(Trigger trigger);
        lua_State* getLuaState();
        Terrain* getTerrain();
        void resetPlayer();
        void movePlayer(float xPosition, float yPosition, float zPosition);
        void setPlayerIndex(int index);
        void forceMove(float x, float y, float z, unsigned indice);
        void activateEndgame();
        void setTerrain(Terrain terrain);
        void printPosition();
        void generateChunks(int chunkSize);
        std::vector<std::vector<Chunk>>* getChunks();
        void addToSpecificChunk(Object* object, int xPosition, int zPosition);
    private:
        lua_State* luaState=nullptr;
        std::vector<Object*> crossChunkObjects;
        std::vector<Object*> allObjects;
        std::vector<Object*> physicsObjects;
        std::vector<std::vector<Chunk>> chunks;
        std::vector<Light> lights;
        std::vector<Trigger> triggers;
        Object* waterPlane;
        glm::vec3 ambientLight;
        glm::vec4 fogColourDay;
        glm::vec4 fogColourRise;
        glm::vec4 fogColourNight;
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
        glm::vec3 nextLightPosition;
        int chunkSize;
        float farPlane;
};

#endif
