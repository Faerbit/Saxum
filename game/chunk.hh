#pragma once

#include <ACGL/OpenGL/Objects.hh>
#include "object.hh"

using namespace ACGL::OpenGL;

class Chunk {
    public:
        Chunk();
        ~Chunk();
        void render(SharedShaderProgram shader, bool lightingPass, bool texturePass,
            glm::mat4* viewProjcetionMatrix, std::vector<glm::mat4>* additionalMatrices=0);
        void addObject(Object* object);
        void sortObjects(int textureCount);
        std::vector<std::vector<Object*>>* getSortedObjects();
    private:
        std::vector<Object*> objects;
        std::vector<std::vector<Object*>> sortedObjects;
};
