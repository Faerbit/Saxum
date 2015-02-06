#ifndef APPLICATION_HH_INCLUDED
#define APPLICATION_HH_INCLUDED

#include "physics.hh"
#include "graphics.hh"
#include "level.hh"
#include "tinyxml2.hh"

class Application {
    public:
        Application();
        Graphics* getGraphics();
        Level* getLevel();
        void init();
        void setFocused(bool focused);
        bool isFocused();
        void setCameraLock(bool locked);
        bool isLocked();
        void ignoreNextMouseUpdate();
        int getIgnoredMouseUpdates();
        void ignoredOneMouseUpdate();
    private:
        void loadConfig();
        void errorCheck(tinyxml2::XMLError error);
        int ignoredMouseUpdates;
        bool focused;
        bool cameraLock;
        Graphics graphics;
        Level level;
        int windowWidth;
        int windowHeight;
        float farPlane;
        std::string compositionsPath;
        std::string shaderPath;
        std::string geometryPath;
        std::string texturePath;
        std::string scriptPath;
        std::string heightmapPath;
        std::string levelXmlPath;
};

#endif
