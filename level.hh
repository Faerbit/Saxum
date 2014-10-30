#ifndef LEVEL_HH_INCLUDED
#define LEVEL_HH_INCLUDED

#include <string>
#include "object.hh"
#include "light.hh"
#include "entity.hh"
#include "terrain.hh"
#include "shader.hh"

class Level {
    public:
        Level(std::string filePath);
        Level();
        ~Level();
        void load(Shader shader); // Shader is necessary for correct texture assigning
        void render();
    private:
        std::string filePath;
        std::vector<Object> objects;
        std::vector<Light> lights;
        glm::vec3 ambientLight;
        Terrain terrain;
};

#endif
