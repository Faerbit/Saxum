#include "object.hh"

Object::Object(Model model, Material material, glm::vec3 position, glm::vec3 rotation,
        glm::vec3 velocity, glm::vec3 angularVelocity, ACGL::OpenGL::SharedShaderProgram shader) :
        Entity(position, rotation) {
    this->model = model;
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
    // set lightning parameters for this object
    shader->setUniform("ambientFactor", material.getAmbientFactor());
    shader->setUniform("diffuseFactor", material.getDiffuseFactor());
    shader->setUniform("specularFactor", material.getSpecularFactor());
    shader->setUniform("shininess", material.getShininess());
    shader->setTexture("uTexture", material.getReference(), 0);
    // set model matrix
    glm::mat4 rotationMatrix = glm::rotate<float>(this->getRotation()[0], glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate<float>(this->getRotation()[1], glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate<float>(this->getRotation()[2], glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 modelMatrix = glm::translate(this->getPosition()) * rotationMatrix * glm::scale<float>(glm::vec3(model.getScale()));
    shader->setUniform( "modelMatrix", modelMatrix);
    // draw
    model.getReference()->render();
}
