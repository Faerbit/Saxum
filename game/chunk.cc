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

void Chunk::addObject(Object* object) {
    objects.push_back(object);
}

void Chunk::sortObjects(int materialCount) {
    // init
    sortedObjects = std::vector<std::vector<Object*>>(materialCount);
    for(unsigned int i = 0; i<sortedObjects.size(); i++) {
        sortedObjects.at(i) = std::vector<Object*>();
    }
    for(unsigned int i = 0; i<objects.size(); i++){
        sortedObjects.at(objects.at(i)->getMaterial()->getMaterialId()).push_back(objects.at(i));
    }
}

std::vector<std::vector<Object*>>* Chunk::getSortedObjects() {
    return &sortedObjects;
}
