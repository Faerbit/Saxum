#include "main.hh"

#include <iomanip>
#include <sstream>

static void resizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    // store the new window size and adjust the viewport:
    app.getGraphics()->resize(glm::uvec2( newWidth, newHeight));
    glViewport( 0, 0, newWidth, newHeight);
}

static void keyCallback(GLFWwindow* _window, int _key, int, int _action, int)
{
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS) {
        if (app.isFocused() && !app.isLocked()) {
            glfwSetWindowShouldClose( _window, GL_TRUE );
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        app.setCameraLock(false);
    }
}

static void mouseCallback(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(window, app.getGraphics()->getWindowSize().x/2, app.getGraphics()->getWindowSize().y/2);
        app.setCameraLock(true);
    }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    app.getLevel()->getCamera()->updateDistance(-(float)yoffset);
}

static void focusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        // Hide mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        app.setFocused(focused);
    }
    else {
        // Show mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        app.setFocused(focused);
        app.setCameraLock(focused);
    }
}

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

bool createWindow()
{
    // Initialise GLFW
    if ( !glfwInit() )
    {
        ACGL::Utils::error() << "Failed to initialize GLFW" << std::endl;
        exit( -1 );
    }
    
    // Configure OpenGL context
    setGLFWHintsForOpenGLVersion( ACGL_OPENGL_VERSION );
    
    // activate multisampling (second parameter is the number of samples):
    //glfwWindowHint( GLFW_SAMPLES, 4);
    
    // request an OpenGL debug context:
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );
    
    // define whether the window can get resized:
    glfwWindowHint(GLFW_RESIZABLE, true);
    
    // try to create an OpenGL context in a window and check the supported OpenGL version:
    //                                                  R,G,B,A, Depth,Stencil
    window = glfwCreateWindow(app.getGraphics()->getWindowSize().x, app.getGraphics()->getWindowSize().y, "SWP MarbleGame Group C", NULL, NULL);
    if (!window) {
        ACGL::Utils::error() << "Failed to open a GLFW window - requested OpenGL: " <<  ACGL_OPENGL_VERSION << std::endl;
        return false;
    }
    glfwMakeContextCurrent(window);
    ACGL::init();
    return true;
}

int main( int argc, char *argv[] )
{
    // app gets created as global variable, to work properly with GLFW
    
    // Create OpenGL capable window:
    if ( !createWindow() ) {
        glfwTerminate();
        exit( -1 );
    }
    
    // Set window title to binary name (without the path):
    std::vector<std::string> tmp = ACGL::Utils::StringHelpers::split( std::string( argv[0] ), '/' );
    glfwSetWindowTitle(window, tmp[tmp.size()-1].c_str() );
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    // set Callbacks
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetKeyCallback(window, keyCallback );
    glfwSetScrollCallback(window, scrollCallback );
    glfwSetWindowFocusCallback(window, focusCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    // Enable vertical sync (on cards that support it) with parameter 1 - 0 means off
    glfwSwapInterval( 0 );
    
    app.init();
    
    int frameCount = 0;
    
    const double FPSdelay = 2.0;
    double startTimeInSeconds = glfwGetTime();
    double showNextFPS = startTimeInSeconds + FPSdelay;
    
    double lastUpdate=0.0f;
    
    do {
        
        double now = glfwGetTime()- startTimeInSeconds;
        
        if (showNextFPS <= now) {
            std::stringstream sstream (std::stringstream::in | std::stringstream::out);
            sstream << std::setprecision(1) << std::fixed
                    << tmp[tmp.size()-1] << " - FPS: " << frameCount / (now-showNextFPS + FPSdelay) << " " << 1000 * (now-showNextFPS + FPSdelay)/frameCount << " msec";
            glfwSetWindowTitle(window, sstream.str().c_str() );
            showNextFPS = now + FPSdelay;
            frameCount = 0;
        }
        
        
        if (app.isLocked() && app.getIgnoredMouseUpdates() == 0) {
            int stateW = glfwGetKey(window, GLFW_KEY_W);
            int stateA = glfwGetKey(window, GLFW_KEY_A);
            int stateS = glfwGetKey(window, GLFW_KEY_S);
            int stateD = glfwGetKey(window, GLFW_KEY_D);
            int stateK = glfwGetKey(window, GLFW_KEY_K);
            int stateL = glfwGetKey(window, GLFW_KEY_L);
            
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            glfwSetCursorPos(window, app.getGraphics()->getWindowSize().x/2, app.getGraphics()->getWindowSize().y/2);
            app.getLevel()->update(now - lastUpdate, now,
                    glm::vec2((float)ypos-app.getGraphics()->getWindowSize().y/2,
                            (float)xpos-app.getGraphics()->getWindowSize().x/2),
                        stateW == GLFW_PRESS,stateA == GLFW_PRESS,stateS == GLFW_PRESS,stateD == GLFW_PRESS,stateK == GLFW_PRESS,stateL == GLFW_PRESS);
        }
        else {
            app.getLevel()->update(now - lastUpdate, now, glm::vec2(0.0f, 0.0f), false, false, false, false,false,false);
            if (app.isLocked()) {
                app.ignoredOneMouseUpdate();
            }
        }
        
        app.getGraphics()->render(now);
        lastUpdate = now;
        
        openGLCriticalError();
        
        // MacOS X will not swap correctly is another FBO is bound:
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glfwSwapBuffers(window);
        glfwPollEvents();
        frameCount++;
        
    } // Check if the window was closed
    while( !glfwWindowShouldClose(window) );
    
    glfwTerminate();
    exit(0);
}

