#pragma once
#include "object.hh"

class Skydome : public Object {
    public:
        Skydome(Model model, Material material, Material nightTexture);
        Skydome();
        Material* getNightTexture();
    private:
        Material nightTexture;
};
