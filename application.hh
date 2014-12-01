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
        ACGL::OpenGL::SharedShaderProgram getShader();
        void init();
        void setFocused(bool focused);
        bool isFocused();
        void setCameraLock(bool locked);
        bool isLocked();
        void ignoreNextMouseUpdate();
        int getIgnoredMouseUpdates();
        void ignoredOneMouseUpdate();
    private:
        int ignoredMouseUpdates;
        bool focused;
        bool cameraLock;
        Graphics graphics;
        Level level;
        ACGL::OpenGL::SharedShaderProgram shader;
};

#endif
