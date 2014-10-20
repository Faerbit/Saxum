#ifndef MODEL_HH_INCLUDED
#define MODEL_HH_INCLUDED

#include <string>

class Model {
    public:
        Model(std::string filePath);
        ~Model();
        void load();
        void render();
    private:
        std::string filePath;
};

#endif
