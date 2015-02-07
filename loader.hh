#pragma once

#include "application.hh"
#include "level.hh"
#include "tinyxml2.hh"

class Loader {
    public:
        Loader();
        void load(std::string filePath, Level* level, std::string compositionsPath, std::string scriptPath);
        void loadConfig(Application* application);
    private:
        void errorCheck(tinyxml2::XMLError error);
};
