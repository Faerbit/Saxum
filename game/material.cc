#include "material.hh"

Material::Material(std::string filePath, float ambientFactor, float diffuseFactor,
        float specularFactor, float shininess, bool movingTexture) {
    reference = ACGL::OpenGL::Texture2DFileManager::the()->get(ACGL::OpenGL::Texture2DCreator(filePath));
    reference->generateMipmaps();
    reference->setMinFilter(GL_NEAREST_MIPMAP_LINEAR);
    reference->setMagFilter(GL_LINEAR);
    this->ambientFactor = ambientFactor;
    this->diffuseFactor = diffuseFactor;
    this->specularFactor = specularFactor;
    this->shininess = shininess;
    this->movingTexture = movingTexture;
}

Material::Material() {
}

Material::~Material() {
}

ACGL::OpenGL::SharedTexture2D Material::getReference() {
    return reference;
}

float Material::getAmbientFactor(){
    return ambientFactor;
}

float Material::getDiffuseFactor(){
    return diffuseFactor;
}

float Material::getSpecularFactor() {
    return specularFactor;
}

float Material::getShininess() {
    return shininess;
}

bool Material::isMoving(){
    return movingTexture;
}
