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

    // construct VAO to give shader correct Attribute locations
    ACGL::OpenGL::SharedArrayBuffer ab = std::make_shared<ACGL::OpenGL::ArrayBuffer>();
    ab->defineAttribute("aPosition", GL_FLOAT, 3);
    ab->defineAttribute("aTexCoord", GL_FLOAT, 2);
    ab->defineAttribute("aNormal", GL_FLOAT, 3);
    ACGL::OpenGL::SharedVertexArrayObject vao = std::make_shared<ACGL::OpenGL::VertexArrayObject>();
    vao->attachAllAttributes(ab);

    // look up all shader files starting with 'phong' and build a ShaderProgram from it:
    shader = ACGL::OpenGL::ShaderProgramCreator("phong").attributeLocations(
            vao->getAttributeLocations()).create();
    shader->use();

    // load Level
    level.load(shader);

    // just in case: check for errors
    openGLCriticalError();
}

void resizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    // store the new window size and adjust the viewport:
    app.getGraphics()->setWindowSize(glm::uvec2( newWidth, newHeight));
    glViewport( 0, 0, newWidth, newHeight);
}

static void keyCallback(GLFWwindow* _window, int _key, int, int _action, int)
{
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS) {
        glfwSetWindowShouldClose( _window, GL_TRUE );
    }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    app.getLevel()->getCamera()->updateDistance(-(float)yoffset);
}


int main( int argc, char *argv[] )
{
    //Application app = Application();

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
    std::vector<std::string> tmp = ACGL::Utils::StringHelpers::split( std::string( argv[0] ), '/' );
    glfwSetWindowTitle(app.getGraphics()->getWindow(), tmp[tmp.size()-1].c_str() );
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_STICKY_KEYS, 1);
    // Hide mouse cursor
    glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetWindowSizeCallback(app.getGraphics()->getWindow(), resizeCallback);
    glfwSetKeyCallback(app.getGraphics()->getWindow(), keyCallback );
    glfwSetScrollCallback(app.getGraphics()->getWindow(), scrollCallback );

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
            std::stringstream sstream (std::stringstream::in | std::stringstream::out);
            sstream << std::setprecision(1) << std::fixed
                    << tmp[tmp.size()-1] << " - FPS: " << frameCount / (now-showNextFPS + FPSdelay) << " " << 1000 * (now-showNextFPS + FPSdelay)/frameCount << " msec";
            glfwSetWindowTitle(app.getGraphics()->getWindow(), sstream.str().c_str() );
            showNextFPS = now + FPSdelay;
            frameCount = 0;
        }

        double xpos, ypos;
        glfwGetCursorPos(app.getGraphics()->getWindow(), &xpos, &ypos);
        glfwSetCursorPos(app.getGraphics()->getWindow(), app.getGraphics()->getWindowSize().x/2, app.getGraphics()->getWindowSize().y/2);

        app.getLevel()->update(now - startTimeInSeconds, glm::vec2((float)ypos-app.getGraphics()->getWindowSize().y/2,
                    (float)xpos-app.getGraphics()->getWindowSize().x/2));
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

