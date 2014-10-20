#ifndef TEXTURE_HH_INCLUDED
#define TEXTURE_HH_INCLUDED

#include <string>

class Texture{
    public:
        Texture(std::string filePath);
        ~Texture();
        void load();
    private:
        std::string filePath;
};

#endif
