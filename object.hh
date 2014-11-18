#ifndef OBJECT_HH_INCLUDED
#define OBJECT_HH_INCLUDED

#include "entity.hh"
#include "model.hh"
#include "material.hh"
#include <string>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Managers.hh>
#include <ACGL/OpenGL/Objects.hh>

class Object : public Entity {
    public:
        Object(Model model, Material material, glm::vec3 position, glm::vec3 rotation,
                ACGL::OpenGL::SharedShaderProgram shader);
        Object();
        ~Object();
        void render();
    private:
        Model model;
        Material material;
        ACGL::OpenGL::SharedShaderProgram shader;
};

#endif
