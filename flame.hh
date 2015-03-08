#pragma once
#include "entity.hh"
#include <ACGL/OpenGL/Objects.hh>

using namespace ACGL::OpenGL;

class Flame : public Entity {
    public:
        Flame(glm::vec3 position, glm::vec3 color, glm::vec3 size);
        Flame();
        void render(SharedShaderProgram shader, glm::mat4 viewProjectionMatrix,
            float time, bool withColor, glm::vec2 skewing, glm::vec3 camera);
    private:
        glm::vec3 color;
        glm::vec3 size;
        SharedVertexArrayObject vao;
        SharedArrayBuffer ab;
};
