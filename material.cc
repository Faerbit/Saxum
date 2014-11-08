#include "material.hh"

Material::Material(std::string filePath) {
    reference = ACGL::OpenGL::Texture2DFileManager::the()->get(ACGL::OpenGL::Texture2DCreator(filePath));
}

Material::Material() {
}

Material::~Material() {
}

ACGL::OpenGL::SharedTexture2D Material::getReference() {
    return reference;
}
