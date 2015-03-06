#ifndef MATERIAL_HH_INCLUDED
#define MATERIAL_HH_INCLUDED

#include <string>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

class Material{
    public:
        Material(std::string filePath, float ambientFactor,
            float diffuseFactor, float specularFactor, float shininess, bool movingTexture = false);
        Material();
        ACGL::OpenGL::SharedTexture2D getReference();
        ~Material();
        float getAmbientFactor();
        float getDiffuseFactor();
        float getSpecularFactor();
        float getShininess();
        bool isMoving();

    private:
        ACGL::OpenGL::SharedTexture2D reference;
        float ambientFactor;
        float diffuseFactor;
        float specularFactor;
        float shininess;
        bool movingTexture;
};

#endif
