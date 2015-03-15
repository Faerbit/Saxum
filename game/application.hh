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
        void initLevel();
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
        void setMaxShadowRenderCount(int count);
        void setCompositionsPath(std::string compositionsPath);
        void setShaderPath(std::string shaderPath);
        void setGeometryPath(std::string geometryPath);
        void setTexturePath(std::string texturePath);
        void setScriptPath(std::string scriptPath);
        void setHeightmapPath(std::string heightmapPath);
        void setLevelXmlPath(std::string levelXmlPath);
        void setLoadingScreenPath(std::string path);
        void setLoadingScreenContinuePath(std::string path);
        bool isGameStarted();
        void startGame();
        void setFullscreen(bool state);
        bool makeFullscreen();
    private:
        bool fullscreen;
        bool gameStarted;
        int ignoredMouseUpdates;
        bool focused;
        bool cameraLock;
        Graphics graphics;
        Level level;
        int windowWidth;
        int windowHeight;
        int shadowCubeSize;
        int maxShadowRenderCount;
        float farPlane;
        std::string compositionsPath;
        std::string shaderPath;
        std::string geometryPath;
        std::string texturePath;
        std::string scriptPath;
        std::string heightmapPath;
        std::string levelXmlPath;
        std::string screenPath;
        std::string screenContinuePath;
};

#endif
