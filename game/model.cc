#include "model.hh"

Model::Model(std::string filePath, float scale) {
    #ifdef CMAKE_CXX_FLAGS_DEBUG
        printf("Loading %s\n", filePath.c_str());
    #endif
    reference = ACGL::OpenGL::VertexArrayObjectCreator(filePath).create();
    this->scale = scale;
}

Model::Model(ACGL::OpenGL::SharedVertexArrayObject vao, float scale){
    reference = vao;
    this->scale = scale;
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
