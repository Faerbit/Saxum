#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "texture.hh"
#include <fstream>
#include <iostream>
#include "png.h"
//#include <winsock.h>		//on windows
#include <netinet/in.h>		//on Unix

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
	unsigned int** heightmap;
};

#endif
