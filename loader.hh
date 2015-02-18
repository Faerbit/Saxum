#pragma once

#include "application.hh"
#include "level.hh"
#include "tinyxml2.hh"

using namespace tinyxml2;

class Loader {
    public:
        Loader();
        void loadConfig(Application* application);
        void load(std::string filePath, Level* level, std::string compositionsPath, std::string scriptPath);
        glm::vec3 reloadPlayerPosition(std::string filePath, Level* level);
    private:
        float queryFloat(XMLElement* element, const char* attribute);
        float queryFloat(XMLDocument*& element, const char* attribute);
        int queryInt(XMLElement* element, const char* attribute);
        int queryInt(XMLDocument*& element, const char* attribute);
        bool queryBool(XMLElement* element, const char* attribute);
        bool queryBool(XMLDocument*& element, const char* attribute);
        std::string queryString(XMLElement* element, const char* attribute);
        std::string queryString(XMLDocument*& element, const char* attribute);
        void errorCheck(tinyxml2::XMLError error);
};
