#include "material.hh"

std::set<SharedTexture2D> Material::allTexturesSet = std::set<SharedTexture2D>();
std::vector<SharedTexture2D> Material::allTexturesVector = std::vector<SharedTexture2D>();

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
    unsigned int textureCount = allTexturesSet.size();
    allTexturesSet.insert(reference);
    if (allTexturesSet.size() != textureCount) {
        allTexturesVector.push_back(reference);
    }
    textureUnit = std::distance(Material::getAllTextures()->begin(),
            std::find(std::begin(*Material::getAllTextures()),
        // first two texture units are used by the loading screen
            std::end(*Material::getAllTextures()), reference)) + 2;
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

std::vector<SharedTexture2D>* Material::getAllTextures() {
    return &allTexturesVector;
}

int Material::getTextureUnit() {
    return textureUnit;
}
