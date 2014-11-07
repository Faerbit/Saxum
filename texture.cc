#include "texture.hh"

Texture::Texture(std::string filePath) {
    reference = ACGL::OpenGL::Texture2DFileManager::the()->get(ACGL::OpenGL::Texture2DCreator(filePath));
}

Texture::Texture() {
}

Texture::~Texture() {
}

ACGL::OpenGL::SharedTexture2D Texture::getReference() {
    return reference;
}

void Texture::load(){
}
