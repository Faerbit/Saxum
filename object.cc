#include "object.hh"

Object::Object(Model model, Material material, glm::vec3 position, glm::vec3 rotation, bool renderable) :
        Entity(position, rotation) {
    this->model = model;
    this->material = material;
    this->renderable = renderable;
}

Object::Object() {
}

Object::~Object() {
}

void Object::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass,
    bool texturePass, glm::mat4* viewProjectionMatrix,
    std::vector<glm::mat4>* additionalMatrices) {
    if (!renderable) {
        return;
    }
    glm::mat4 modelMatrix = glm::translate(getPosition()) * getRotation() * glm::scale<float>(glm::vec3(model.getScale()));
    if(texturePass) {
        if (material.isMoving()) {
            shader->setUniform("movingTexture", true);
        }
        else {
            shader->setUniform("movingTexture", false);
        }
        shader->setTexture("uTexture", material.getReference(), 0);
        shader->setUniform("modelMatrix", modelMatrix);
    }
    if (lightingPass) {
    // set lightning parameters for this object
        shader->setUniform("ambientFactor", material.getAmbientFactor());
        shader->setUniform("diffuseFactor", material.getDiffuseFactor());
        shader->setUniform("specularFactor", material.getSpecularFactor());
        shader->setUniform("shininess", material.getShininess());
        // set model matrix
        shader->setUniform("modelMatrix", modelMatrix);
        // set shadowMVPs
        glm::mat4 shadowMVPs[5];
        for(unsigned int i = 0; (i<additionalMatrices->size() && i<5); i++) {
            shadowMVPs[i] = additionalMatrices->at(i) * modelMatrix;
        }
        glUniformMatrix4fv(shader->getUniformLocation("shadowMVPs"),
                sizeof(shadowMVPs), false, (GLfloat*) shadowMVPs);
    }
    else {
        if (additionalMatrices) {
            shader->setUniform("modelViewMatrix", additionalMatrices->at(0) * modelMatrix);
        }
    }
    glm::mat4 mvp = (*viewProjectionMatrix) * modelMatrix;
    shader->setUniform("modelViewProjectionMatrix", mvp);
    // draw
    model.getReference()->render();
}
