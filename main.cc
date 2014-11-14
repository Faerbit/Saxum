#include "main.hh"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <ACGL/OpenGL/Objects.hh>
#include <ACGL/OpenGL/Data/TextureData.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

#include <ACGL/OpenGL/glloaders/extensions.hh>
#include <ACGL/Base/Settings.hh>

#include "model.hh"

using namespace std;
using namespace ACGL::OpenGL;
using namespace ACGL::Base;
using namespace ACGL::Utils;

Application::Application() {
    graphics = Graphics(glm::uvec2(1024, 786), 0.1f, 100.0f);
}

Graphics* Application::getGraphics() {
    return &graphics;
}

Level* Application::getLevel() {
    return &level;
}

ACGL::OpenGL::SharedShaderProgram Application::getShader() {
    return shader;
}

void Application::init()
{
    // define where shaders and textures can be found:
    ACGL::Base::Settings::the()->setResourcePath("../");
    ACGL::Base::Settings::the()->setShaderPath("Shader/");
    ACGL::Base::Settings::the()->setTexturePath("Geometry/");
    ACGL::Base::Settings::the()->setGeometryPath("Geometry/");

    // load Model to give shader correct Attribute locations
    // TODO look up if this is really necessary, since this looks really stupid.
    Model model = Model("Bunny.obj");

    // look up all shader files starting with 'phong' and build a ShaderProgram from it:
    shader = ACGL::OpenGL::ShaderProgramCreator("phong").attributeLocations(
            model.getReference()->getAttributeLocations()).create();
    shader->use();

    // load Level
    level.load(shader);

    // just in case: check for errors
    openGLCriticalError();
}

/**********************************************************************************************************************
 * Returns true if a window with the desired context could get created.
 * Requested OpenGL version gets set by ACGL defines.
 */

static void keyCallback(GLFWwindow* _window, int _key, int, int _action, int)
{
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS) {
        glfwSetWindowShouldClose( _window, GL_TRUE );
    }
}


int main( int argc, char *argv[] )
{
    Application app = Application();

    /////////////////////////////////////////////////////////////////////////////////////
    // Create OpenGL capable window:
    //
    if ( !app.getGraphics()->createWindow() ) {
        glfwTerminate();
        exit( -1 );
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Set window title to binary name (without the path):
    //
    std::vector<std::string> tmp = StringHelpers::split( std::string( argv[0] ), '/' );
    glfwSetWindowTitle(app.getGraphics()->getWindow(), tmp[tmp.size()-1].c_str() );
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_STICKY_KEYS, 1 );
    //glfwSetWindowSizeCallback(app.getGraphics(), resizeCallback);
    glfwSetKeyCallback(app.getGraphics()->getWindow(), keyCallback );

    // Enable vertical sync (on cards that support it) with parameter 1 - 0 means off
    glfwSwapInterval( 0 );

    /////////////////////////////////////////////////////////////////////////////////////
    // OpenGL state:
    //
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    app.init();

    int frameCount = 0;

    const double FPSdelay = 2.0;
    double startTimeInSeconds = glfwGetTime();
    double showNextFPS = startTimeInSeconds + FPSdelay;

    do {
        double now = glfwGetTime();

        if (showNextFPS <= now) {
            stringstream sstream (stringstream::in | stringstream::out);
            sstream << setprecision(1) << std::fixed
                    << tmp[tmp.size()-1] << " - FPS: " << frameCount / (now-showNextFPS + FPSdelay) << " " << 1000 * (now-showNextFPS + FPSdelay)/frameCount << " msec";
            glfwSetWindowTitle(app.getGraphics()->getWindow(), sstream.str().c_str() );
            showNextFPS = now + FPSdelay;
            frameCount = 0;
        }

        app.getLevel()->update(now - startTimeInSeconds);
        app.getGraphics()->render(app.getLevel(), app.getShader());

        openGLCriticalError();

        // MacOS X will not swap correctly is another FBO is bound:
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glfwSwapBuffers(app.getGraphics()->getWindow());
        glfwPollEvents();
        frameCount++;
    } // Check if the window was closed
    while( !glfwWindowShouldClose(app.getGraphics()->getWindow()) );

    glfwTerminate();
    exit(0);
}

