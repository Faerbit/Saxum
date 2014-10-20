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

#include <ACGL/OpenGL/glloaders/extensions.hh>

using namespace std;
using namespace ACGL::OpenGL;
using namespace ACGL::Base;
using namespace ACGL::Utils;

glm::uvec2 g_windowSize( 640, 480 );
float g_nearPlane =   0.1f;
float g_farPlane  = 100.0f;
bool glfwWindowClosed = false;

GLFWwindow* g_window;

void setGLFWHintsForOpenGLVersion( unsigned int _version )
{
#ifdef __APPLE__
#if (ACGL_OPENGL_VERSION >= 30)
    // request OpenGL 3.2, will return a 4.1 context on Mavericks
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
#else
// non-apple
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, _version / 10 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, _version % 10 );
    #ifdef ACGL_OPENGL_PROFILE_CORE
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif
#endif
}

/**********************************************************************************************************************
 * Returns true if a window with the desired context could get created.
 * Requested OpenGL version gets set by ACGL defines.
 */
bool createWindow()
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Initialise GLFW
    //
    if ( !glfwInit() )
    {
        error() << "Failed to initialize GLFW" << endl;
        exit( -1 );
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Configure OpenGL context
    //
    setGLFWHintsForOpenGLVersion( ACGL_OPENGL_VERSION );

    // activate multisampling (second parameter is the number of samples):
    //glfwWindowHint( GLFW_SAMPLES, 8 );

    // request an OpenGL debug context:
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );

    // define whether the window can get resized:
    //glfwWindowHint( GLFW_RESIZABLE, true );

    // non-decorated windows can be used as splash screens:
    //glfwWindowHint( GLFW_DECORATED, false );

    // request an sRGB framebuffer:
    //glfwWindowHint( GLFW_SRGB_CAPABLE, true );

    //glfwWindowHint( , true );
    //glfwWindowHint( , true );
    //glfwWindowHint( , true );
    //glfwWindowHint( , true );


    /////////////////////////////////////////////////////////////////////////////////////
    // try to create an OpenGL context in a window and check the supported OpenGL version:
    //                                                  R,G,B,A, Depth,Stencil
    g_window = glfwCreateWindow( g_windowSize.x, g_windowSize.y, "ACGL GLFWExamples", NULL, NULL);
    if (!g_window) {
        error() << "Failed to open a GLFW window - requested OpenGL: " <<  ACGL_OPENGL_VERSION << endl;
        return false;
    }
    glfwMakeContextCurrent(g_window);
    ACGL::init();

    /////////////////////////////////////////////////////////////////////////////////////
    // Init debug-extension
    //
    if (ACGL_ARB_debug_output()) {
        //debug() << "GL_ARB_DEBUG_OUTPUT is supported, register callback" << endl;
        //glDebugMessageCallbackARB( debugCallback, NULL);

        // filter out the strange performance warnings about shader recompiles:
        //glDebugMessageControlARB( GL_DEBUG_SOURCE_API_ARB, GL_DEBUG_TYPE_PERFORMANCE_ARB, GL_DEBUG_SEVERITY_MEDIUM_ARB, 0, NULL, GL_FALSE );
    } else {
        //debug() << "GL_ARB_DEBUG_OUTPUT is missing!" << endl;
    }
    return true;
}

glm::mat4 buildFrustum( float phiInDegree, float _near, float _far, float aspectRatio) {

    float phiHalfInRadians = 0.5*phiInDegree * (M_PI/180.0);
    float top = _near * tan( phiHalfInRadians );
    float bottom = -top;
    float left  = bottom * aspectRatio;
    float right = -left;

    return glm::frustum(left, right, bottom, top, _near, _far);
}

glm::mat4 lookAt(const glm::vec3 &camPos, const glm::vec3 &viewDirection, const glm::vec3 &up) {

    glm::vec3 right   = glm::normalize( glm::cross( viewDirection, up ) );
    glm::vec3 orthoUp = glm::normalize( glm::cross( right, viewDirection ) );

    glm::vec3 viewDir = glm::normalize( viewDirection );
    glm::mat4 m;

    m[0][0] = right.x;
    m[1][0] = right.y;
    m[2][0] = right.z;
    m[3][0] = 0;

    m[0][1] = orthoUp.x;
    m[1][1] = orthoUp.y;
    m[2][1] = orthoUp.z;
    m[3][1] = 0;

    m[0][2] = -viewDir.x;
    m[1][2] = -viewDir.y;
    m[2][2] = -viewDir.z;
    m[3][2] = 0;

    m[0][3] = 0;
    m[1][3] = 0;
    m[2][3] = 0;
    m[3][3] = 1.0;

    glm::mat4 tr = glm::translate(glm::mat4(),glm::vec3( -camPos.x, -camPos.y, -camPos.z));

    return m*tr;
}

static void keyCallback(GLFWwindow* _window, int _key, int, int _action, int)
{
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS) {
        glfwSetWindowShouldClose( _window, GL_TRUE );
    }
}


/**********************************************************************************************************************
 * Generic main for different example apps
 */
int main( int argc, char *argv[] )
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Create OpenGL capable window:
    //
    if ( !createWindow() ) {
        glfwTerminate();
        exit( -1 );
    }

    /////////////////////////////////////////////////////////////////////////////////////
    // Set window title to binary name (without the path):
    //
    std::vector<std::string> tmp = StringHelpers::split( std::string( argv[0] ), '/' );
    glfwSetWindowTitle( g_window, tmp[tmp.size()-1].c_str() );
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode( g_window, GLFW_STICKY_KEYS, 1 );
    glfwSetWindowSizeCallback(  g_window, resizeCallback );
    glfwSetKeyCallback( g_window, keyCallback );

    // Enable vertical sync (on cards that support it) with parameter 1 - 0 means off
    glfwSwapInterval( 0 );

    /////////////////////////////////////////////////////////////////////////////////////
    // OpenGL state:
    //
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    initCustomResources();

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
            glfwSetWindowTitle( g_window, sstream.str().c_str() );
            showNextFPS = now + FPSdelay;
            frameCount = 0;
        }

        draw( now - startTimeInSeconds );
        openGLCriticalError();

        // MacOS X will not swap correctly is another FBO is bound:
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glfwSwapBuffers( g_window );
        glfwPollEvents();
        frameCount++;
    } // Check if the window was closed
    while( !glfwWindowShouldClose( g_window ) );

    deleteCustomResources();

    glfwTerminate();
    exit(0);
}

