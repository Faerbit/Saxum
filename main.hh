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
    private:
        Graphics graphics;
        Level level;
        ACGL::OpenGL::SharedShaderProgram shader;
};

#endif
