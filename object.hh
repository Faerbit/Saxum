#ifndef OBJECT_HH_INCLUDED
#define OBJECT_HH_INCLUDED

#include "entity.hh"
#include "model.hh"
#include "texture.hh"
#include "shader.hh"
#include <string>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Objects.hh>

class Object : Entity {
    public:
        Object(Model model, Texture texture, glm::vec3 position, glm::vec3 rotation, 
                glm::vec3 velocity, glm::vec3 angularVelocity, Shader shader);
	Object();
        ~Object();
        void render();
    private:
        ACGL::OpenGL::SharedVertexArrayObject model;
        ACGL::OpenGL::SharedTexture2D texture;
        glm::vec3 velocity;
        glm::vec3 angularVelocity;
        ACGL::OpenGL::SharedShaderProgram shader;
};

#endif
