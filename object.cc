#include "object.hh"

Object::Object(Model model, Material material, glm::vec3 position, glm::vec3 rotation,
        glm::vec3 velocity, glm::vec3 angularVelocity, ACGL::OpenGL::SharedShaderProgram shader) :
        Entity(position, rotation) {
    this->model = model.getReference();
    this->material = material;
    this->velocity = velocity;
    this->angularVelocity = angularVelocity;
    this->shader = shader;
}

Object::Object() {
}

Object::~Object() {
}

void Object::render() {
    shader->setUniform("ambientFactor", material.getAmbientFactor());
    shader->setUniform("diffuseFactor", material.getDiffuseFactor());
    shader->setUniform("specularFactor", material.getSpecularFactor());
    shader->setUniform("shininess", material.getShininess());
    shader->setTexture("uTexture", material.getReference(), 0);
    model->render();
}
