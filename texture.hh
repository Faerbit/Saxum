#ifndef TEXTURE_HH_INCLUDED
#define TEXTURE_HH_INCLUDED

#include <string>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

class Texture{
    public:
        Texture(std::string filePath);
	Texture();
        ACGL::OpenGL::SharedTexture2D getReference();
        ~Texture();
    private:
        ACGL::OpenGL::SharedTexture2D reference;
};

#endif
