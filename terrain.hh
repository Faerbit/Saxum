#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "texture.hh"

class Terrain {
    public:
        Terrain();
        ~Terrain();
        void load();
        void render();
    private:
        float friction;
        Texture texture;
        std::string filePath;
};

#endif
