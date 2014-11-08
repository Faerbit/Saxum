#ifndef MATERIAL_HH_INCLUDED
#define MATERIAL_HH_INCLUDED

#include <string>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

class Material{
    public:
        Material(std::string filePath);
	    Material();
        ACGL::OpenGL::SharedTexture2D getReference();
        ~Material();
    private:
        ACGL::OpenGL::SharedTexture2D reference;
};

#endif
