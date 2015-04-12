#pragma once
#include "object.hh"
#include <ACGL/OpenGL/Objects.hh>

using namespace ACGL::OpenGL;
using namespace std;

class Skydome : public Object{
    public:
        Skydome(Model model, string dayTexturePath, string nightTexturePath);
        Skydome();
        SharedTexture2D getNightTexture();
        SharedTexture2D getDayTexture();
    private:
        SharedTexture2D nightTexture;
        SharedTexture2D dayTexture;

};
