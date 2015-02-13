#pragma once

#include "application.hh"
#include "level.hh"
#include "tinyxml2.hh"

class Loader {
    public:
        Loader();
        void loadConfig(Application* application);
        void load(std::string filePath, Level* level, std::string compositionsPath, std::string scriptPath);
        glm::vec3 reloadPlayerPosition(std::string filePath, Level* level);
    private:
        void errorCheck(tinyxml2::XMLError error);
};
