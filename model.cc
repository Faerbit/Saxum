#include "model.hh"

Model::Model(std::string filePath) {
    reference = ACGL::OpenGL::VertexArrayObjectCreator(filePath).create();
    reference->bind();
}

Model::Model(){
}

Model::~Model() {
}

ACGL::OpenGL::SharedVertexArrayObject Model::getReference() {
    return reference;
}
