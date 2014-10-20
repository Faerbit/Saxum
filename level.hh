#ifndef LEVEL_HH_INCLUDED
#define LEVEL_HH_INCLUDED

#include <string>
#include "entity.hh"

#endif

class Level {
    public:
        Level(string filePath);
        ~Level();
        void load();
    private:
        string filePath;
        std::vector entities;
}
