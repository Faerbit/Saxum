#include "shader.hh"

Shader::Shader(std::string filePath, Model model) {
    reference = ACGL::OpenGL::ShaderProgramCreator(filePath).attributeLocations(
            model.getReference()->getAttributeLocations()).create();
    reference->use();
}

Shader::Shader() {
}

Shader::~Shader() {
}

ACGL::OpenGL::SharedShaderProgram Shader::getReference() {
    return reference;
}
