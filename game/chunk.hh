#pragma once

#include "object.hh"
#include <ACGL/OpenGL/Objects.hh>

using namespace ACGL::OpenGL;

class Chunk {
    public:
        Chunk();
        ~Chunk();
        void render(SharedShaderProgram shader, bool lightingPass, bool texturePass,
            glm::mat4* viewProjcetionMatrix, std::vector<glm::mat4>* additionalMatrices=0);
        void addObject(Object* object);
    private:
        std::vector<Object*> objects;
};