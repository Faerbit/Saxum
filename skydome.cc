#include "skydome.hh"

Skydome::Skydome(Model model, Material material, Material nightTexture) :
    Object(model, material, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true){
        this->nightTexture = nightTexture;
}

Skydome::Skydome() {
}

Material* Skydome::getNightTexture() {
    return &nightTexture;
}
