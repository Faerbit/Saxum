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

void Object::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass,
    glm::mat4* viewProjectionMatrix, std::vector<glm::mat4>* shadowVPs) {
    glm::mat4 modelMatrix = glm::translate(getPosition()) * getRotation() * glm::scale<float>(glm::vec3(model.getScale()));
    if (lightingPass) {
    // set lightning parameters for this object
        shader->setUniform("ambientFactor", material.getAmbientFactor());
        shader->setUniform("diffuseFactor", material.getDiffuseFactor());
        shader->setUniform("specularFactor", material.getSpecularFactor());
        shader->setUniform("shininess", material.getShininess());
        shader->setTexture("uTexture", material.getReference(), 0);
        // set model matrix
        shader->setUniform("modelMatrix", modelMatrix);
        // set shadowMVPs
        glm::mat4 shadowMVPs[5];
        for(unsigned int i = 0; (i<shadowVPs->size() && i<5); i++) {
            shadowMVPs[i] = shadowVPs->at(i) * modelMatrix;
        }
        glUniformMatrix4fv(shader->getUniformLocation("shadowMVPs"),
                sizeof(shadowMVPs), false, (GLfloat*) shadowMVPs);
    }
    else {
        //shader->setUniform("modelViewMatrix", shadowVPs->at(0) * modelMatrix);
    }
    glm::mat4 mvp = (*viewProjectionMatrix) * modelMatrix;
    shader->setUniform("modelViewProjectionMatrix", mvp);
    // draw
    model.getReference()->render();
}
