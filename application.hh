#ifndef APPLICATION_HH_INCLUDED
#define APPLICATION_HH_INCLUDED

#include "physics.hh"
#include "graphics.hh"
#include "level.hh"

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
        void setWindowWidth(int windowWidth);
        void setWindowHeight(int windowHeight);
        void setShadowCubeSize(int shadowCubeSize);
        void setFarPlane(float farPlane);
        void setCompositionsPath(std::string compositionsPath);
        void setShaderPath(std::string shaderPath);
        void setGeometryPath(std::string geometryPath);
        void setTexturePath(std::string texturePath);
        void setScriptPath(std::string scriptPath);
        void setHeightmapPath(std::string heightmapPath);
        void setLevelXmlPath(std::string levelXmlPath);
    private:
        int ignoredMouseUpdates;
        bool focused;
        bool cameraLock;
        Graphics graphics;
        Level level;
        int windowWidth;
        int windowHeight;
        int shadowCubeSize;
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
