#ifndef MAIN_HH_INCLUDED
#define MAIN_HH_INCLUDED
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/Math/Math.hh>

#include <GLFW/glfw3.h>
#include <ACGL/OpenGL/Objects/ArrayBuffer.hh>
////////////////////////////////////////////////////////////////////

#include "physics.hh"
#include "graphics.hh"
#include "level.hh"

/////////////////////////////////////////////////////////////////
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

Application app;
#endif
