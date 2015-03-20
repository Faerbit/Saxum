#include "chunk.hh"

Chunk::Chunk() {
}

Chunk::~Chunk() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        delete objects.at(i);
    }
}

void Chunk::render(SharedShaderProgram shader, bool lightingPass, bool texturePass,
    glm::mat4* viewProjcetionMatrix, std::vector<glm::mat4>* additionalMatrices) {
    for(unsigned int i = 0; i<objects.size(); i++) {
        objects.at(i)->render(shader, lightingPass, texturePass, viewProjcetionMatrix, additionalMatrices);
    }
}

void Chunk::enqueueObjects(std::vector<std::vector<Object*>>* renderQueue) {
    for(unsigned int i = 0; i<objects.size(); i++) {
        renderQueue->at(objects.at(i)->getMaterial()->getTextureUnit() - 2).push_back(objects.at(i));
    }
}

void Chunk::addObject(Object* object) {
    objects.push_back(object);
}
