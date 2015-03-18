#ifndef MATERIAL_HH_INCLUDED
#define MATERIAL_HH_INCLUDED

#include <string>
#include <set>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

using namespace ACGL::OpenGL;

class Material{
    public:
        Material(std::string filePath, float ambientFactor,
            float diffuseFactor, float specularFactor, float shininess, bool movingTexture = false);
        Material();
        SharedTexture2D getReference();
        ~Material();
        float getAmbientFactor();
        float getDiffuseFactor();
        float getSpecularFactor();
        float getShininess();
        bool isMoving();
        static std::vector<SharedTexture2D>* getAllTextures();

    private:
        ACGL::OpenGL::SharedTexture2D reference;
        float ambientFactor;
        float diffuseFactor;
        float specularFactor;
        float shininess;
        bool movingTexture;
        static std::set<SharedTexture2D> allTexturesSet;
        static std::vector<SharedTexture2D> allTexturesVector;
};

#endif
