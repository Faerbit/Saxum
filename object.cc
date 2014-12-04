#include "object.hh"

Object::Object(Model model, Material material, glm::vec3 position, glm::vec3 rotation) :
        Entity(position, rotation) {
    this->model = model;
    this->material = material;
}

Object::Object() {
}

Object::~Object() {
}

void Object::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass) {
    if (lightingPass) {
    // set lightning parameters for this object
        shader->setUniform("ambientFactor", material.getAmbientFactor());
        shader->setUniform("diffuseFactor", material.getDiffuseFactor());
        shader->setUniform("specularFactor", material.getSpecularFactor());
        shader->setUniform("shininess", material.getShininess());
        shader->setTexture("uTexture", material.getReference(), 0);
    }
    // set model matrix
    glm::mat4 modelMatrix = glm::translate(getPosition()) * getRotation() * glm::scale<float>(glm::vec3(model.getScale()));
    shader->setUniform( "modelMatrix", modelMatrix);
    // draw
    model.getReference()->render();
}
