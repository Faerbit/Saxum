#include "ourShader.hh"

OurShader::OurShader(std::string filePath, Model model) {
    reference = ACGL::OpenGL::ShaderProgramCreator(filePath).attributeLocations(
            model.getReference()->getAttributeLocations()).create();
    reference->use();
}

OurShader::OurShader() {
}

OurShader::~OurShader() {
}

ACGL::OpenGL::SharedShaderProgram OurShader::getReference() {
    return reference;
}
