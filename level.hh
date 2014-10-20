#ifndef LEVEL_HH_INCLUDED
#define LEVEL_HH_INCLUDED

#include <string>
#include "entity.hh"
#include "terrain.hh"

class Level {
    public:
        Level(std::string filePath);
        ~Level();
        void load();
    private:
        std::string filePath;
        std::vector<Entity> entities;
        Terrain terrain;
};

#endif
