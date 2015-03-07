#include "flame.hh"

Flame::Flame(glm::vec3 position, glm::vec3 color, glm::vec3 size) : 
    Entity(position, glm::vec3(0.0f, 0.0f, 0.0f)) {
    this->color = color;
    this->size = size;
    float flameData[6] = {};
    flameData[0] = position.x;
    flameData[1] = position.y;
    flameData[2] = position.z;
    flameData[3] = color.r;
    flameData[4] = color.g;
    flameData[5] = color.b;
    ab = SharedArrayBuffer(new ArrayBuffer());
    ab->defineAttribute("aPosition", GL_FLOAT, 3);
    ab->defineAttribute("aColor", GL_FLOAT, 3);
    vao = SharedVertexArrayObject(new VertexArrayObject());
    vao->setMode(GL_POINTS);
    vao->attachAllAttributes(ab);
    ab->setDataElements(1, flameData);
}

Flame::Flame() {
}

void Flame::render(SharedShaderProgram shader, glm::mat4 viewProjectionMatrix, float time,
    bool withColor) {
    glm::mat4 modelMatrix;
    if (!withColor) {
        modelMatrix = glm::scale<float>(size * glm::vec3(1.1f));
    }
    else {
        modelMatrix = glm::scale<float>(size);
    }
    glm::mat4 modelViewProjectionMatrix = viewProjectionMatrix * modelMatrix;
    shader->setUniform("modelViewProjectionMatrix", modelViewProjectionMatrix);
    shader->setUniform("viewProjectionMatrix", viewProjectionMatrix);
    shader->setUniform("withColor", withColor);
    shader->setUniform("time", time);
    shader->setUniform("bottom", true);
    shader->setUniform("left", true);
    vao->render();
    shader->setUniform("left", false);
    vao->render();
    shader->setUniform("bottom", false);
    shader->setUniform("left", true);
    vao->render();
    shader->setUniform("left", false);
    vao->render();
}
