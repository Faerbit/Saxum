#include "skydome.hh"
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>

Skydome::Skydome(Model model, string dayTexturePath, string nightTexturePath) :
    Object(model, Material(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true) {
    this->dayTexture = Texture2DFileManager::the()->get(Texture2DCreator(dayTexturePath));
    this->nightTexture = Texture2DFileManager::the()->get(Texture2DCreator(nightTexturePath));
}

Skydome::Skydome() {
}

SharedTexture2D Skydome::getDayTexture() {
    return dayTexture;
}

SharedTexture2D Skydome::getNightTexture() {
    return nightTexture;
}
