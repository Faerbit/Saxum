#include "main.hh"

#include <iomanip>
#include <sstream>
#include "keyboardState.hh"

static void resizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    // store the new window size and adjust the viewport:
    app.getGraphics()->resize(glm::uvec2( newWidth, newHeight));
    glViewport( 0, 0, newWidth, newHeight);
}

static void keyCallback(GLFWwindow* _window, int _key, int, int _action, int)
{
    if (!app.isGameStarted() && _action == GLFW_PRESS && _key != GLFW_KEY_ESCAPE) {
        app.startGame();
    }
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS) {
        if (app.isFocused() && !app.isLocked()) {
            glfwSetWindowShouldClose( _window, GL_TRUE );
        }
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        app.setCameraLock(false);
    }
#ifdef SAXUM_DEBUG
    if (_key == GLFW_KEY_F5 && _action == GLFW_PRESS) {
        app.getGraphics()->setRenderShadows(!app.getGraphics()->getRenderShadows());
    }
    if (_key == GLFW_KEY_F6 && _action == GLFW_PRESS) {
        app.getGraphics()->setRenderFlames(!app.getGraphics()->getRenderFlames());
    }
    if (_key == GLFW_KEY_F7 && _action == GLFW_PRESS) {
        app.getGraphics()->setRenderDebug(!app.getGraphics()->getRenderDebug());
    }
    if (_key == GLFW_KEY_F8 && _action == GLFW_PRESS) {
        app.getGraphics()->setRenderWorld(!app.getGraphics()->getRenderWorld());
    }
    if (_key == GLFW_KEY_F9 && _action == GLFW_PRESS) {
        app.getLevel()->printPosition();
    }
#endif
}

static void mouseCallback(GLFWwindow* window, int button, int action, int) {
    if (!app.isGameStarted() && action == GLFW_PRESS) {
        app.startGame();
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(window, ((double)app.getGraphics()->getWindowSize().x)/2.0, ((double)app.getGraphics()->getWindowSize().y)/2.0);
        app.setCameraLock(true);
    }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    app.getLevel()->getCamera()->updateDistance(-(float)yoffset);
}

static void focusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        // Hide mouse cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    glfwWindowHint( GLFW_SAMPLES, 4);

    #ifdef SAXUM_DEBUG
        // request an OpenGL debug context:
        glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );
    #endif
    
    // define whether the window can get resized:
    glfwWindowHint(GLFW_RESIZABLE, true);
    
    if(app.makeFullscreen()) {
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        app.getGraphics()->resize(glm::uvec2(mode->width, mode->height));
        window = glfwCreateWindow(mode->width, mode->height, "Saxum", glfwGetPrimaryMonitor(), NULL);

        if (!window) {
            ACGL::Utils::error() << "Failed to open a GLFW window - requested OpenGL: " <<  ACGL_OPENGL_VERSION << std::endl;
            return false;
        }
    }
    else {
        // try to create an OpenGL context in a window and check the supported OpenGL version:
        //                                                  R,G,B,A, Depth,Stencil
        window = glfwCreateWindow(app.getGraphics()->getWindowSize().x, app.getGraphics()->getWindowSize().y, "Saxum", NULL, NULL);
        if (!window) {
            ACGL::Utils::error() << "Failed to open a GLFW window - requested OpenGL: " <<  ACGL_OPENGL_VERSION << std::endl;
            return false;
        }
    }
    glfwMakeContextCurrent(window);
    #ifdef SAXUM_DEBUG
        ACGL::init(true);
    #else
        ACGL::init(false); // do not request debug context
    #endif
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
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    // set Callbacks
    glfwSetWindowSizeCallback(window, resizeCallback);
    glfwSetWindowFocusCallback(window, focusCallback);
    
    // Enable vertical sync (on cards that support it) with parameter 1 - 0 means off
    glfwSwapInterval( 0 );
    
    app.init();
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glfwSwapBuffers(window);
    app.initLevel();

    glfwSetKeyCallback(window, keyCallback );
    glfwSetScrollCallback(window, scrollCallback );
    glfwSetMouseButtonCallback(window, mouseCallback);
    
    int frameCount = 0;
    
    const double FPSdelay = 1.0;
    double startTimeInSeconds = glfwGetTime();
    double showNextFPS = startTimeInSeconds + FPSdelay;
    
    double lastUpdate=0.0f;

    KeyboardState keyboardState;
    
    do {
        
        double now = glfwGetTime()- startTimeInSeconds;

        #ifdef SAXUM_DEBUG
            if (showNextFPS <= now) {
                std::stringstream sstream (std::stringstream::in | std::stringstream::out);
                sstream << std::setprecision(1) << std::fixed
                        << "Saxum" << " - FPS: " << frameCount / (now-showNextFPS + FPSdelay) << " " << 1000 * (now-showNextFPS + FPSdelay)/frameCount << " msec";
                glfwSetWindowTitle(window, sstream.str().c_str() );
                showNextFPS = now + FPSdelay;
                frameCount = 0;
            }
        #endif
        if (app.isGameStarted()) {
            static float gameStart = now;
            if (app.isLocked() && app.getIgnoredMouseUpdates() == 0) {
                keyboardState.wPressed  = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
                keyboardState.aPressed  = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
                keyboardState.sPressed  = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
                keyboardState.dPressed  = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
                #ifdef SAXUM_DEBUG
                    keyboardState.lPressed  = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
                    keyboardState.kPressed  = glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS;
                    keyboardState.f1Pressed = glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS;
                    keyboardState.f2Pressed = glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS;
                    keyboardState.f3Pressed = glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS;
                    keyboardState.f4Pressed = glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS;
                #else
                    keyboardState.lPressed  = false;
                    keyboardState.kPressed  = false;
                    keyboardState.f1Pressed = false;
                    keyboardState.f2Pressed = false;
                    keyboardState.f3Pressed = false;
                    keyboardState.f4Pressed = false;
                #endif

                
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                glfwSetCursorPos(window, ((double)app.getGraphics()->getWindowSize().x)/2.0, ((double)app.getGraphics()->getWindowSize().y)/2.0);
                app.getLevel()->update(now - lastUpdate, now - gameStart,
                    glm::vec2((float)ypos-app.getGraphics()->getWindowSize().y/2,
                    (float)xpos-app.getGraphics()->getWindowSize().x/2),
                    &keyboardState);
            }
            else {
                keyboardState.wPressed  = false;
                keyboardState.aPressed  = false;
                keyboardState.sPressed  = false;
                keyboardState.dPressed  = false;
                keyboardState.lPressed  = false;
                keyboardState.kPressed  = false;
                keyboardState.f1Pressed = false;
                keyboardState.f2Pressed = false;
                keyboardState.f3Pressed = false;
                keyboardState.f4Pressed = false;
                app.getLevel()->update(now - lastUpdate, now-gameStart, glm::vec2(0.0f, 0.0f), 
                        &keyboardState);
                if (app.isLocked()) {
                    app.ignoredOneMouseUpdate();
                }
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

