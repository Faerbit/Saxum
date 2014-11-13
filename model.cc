#include "model.hh"

Model::Model(std::string filePath, float scale) {
    reference = ACGL::OpenGL::VertexArrayObjectCreator(filePath).create();
    reference->bind();
    this->scale = scale;
}

Model::Model(std::string filePath) {
    reference = ACGL::OpenGL::VertexArrayObjectCreator(filePath).create();
    reference->bind();
    this->scale = 1.0f;
}

Model::Model(){
}

Model::~Model() {
}

ACGL::OpenGL::SharedVertexArrayObject Model::getReference() {
    return reference;
}

void Model::setScale(float scale) {
    this->scale = scale;
}

float Model::getScale() {
    return scale;
}
