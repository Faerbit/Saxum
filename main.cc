#include "main.hh"

#include <iomanip>
#include <sstream>

void resizeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
    // store the new window size and adjust the viewport:
    app.getGraphics()->setWindowSize(glm::uvec2( newWidth, newHeight));
    glViewport( 0, 0, newWidth, newHeight);
}

static void keyCallback(GLFWwindow* _window, int _key, int, int _action, int)
{
    if (_key == GLFW_KEY_ESCAPE && _action == GLFW_PRESS) {
        if (app.isFocused() && !app.isLocked()) {
            glfwSetWindowShouldClose( _window, GL_TRUE );
        }
        glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        app.setCameraLock(false);
    }
}

static void mouseCallback(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetCursorPos(app.getGraphics()->getWindow(), app.getGraphics()->getWindowSize().x/2, app.getGraphics()->getWindowSize().y/2);
        app.setCameraLock(true);
    }
}

static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    app.getLevel()->getCamera()->updateDistance(-(float)yoffset);
}

static void focusCallback(GLFWwindow* window, int focused) {
    if (focused) {
        // Hide mouse cursor
        glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        app.setFocused(focused);
    }
    else {
        // Show mouse cursor
        glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        app.setFocused(focused);
        app.setCameraLock(focused);
    }
}

int main( int argc, char *argv[] )
{
    //Application app = Application();

    // Create OpenGL capable window:
    if ( !app.getGraphics()->createWindow() ) {
        glfwTerminate();
        exit( -1 );
    }

    // Set window title to binary name (without the path):
    std::vector<std::string> tmp = ACGL::Utils::StringHelpers::split( std::string( argv[0] ), '/' );
    glfwSetWindowTitle(app.getGraphics()->getWindow(), tmp[tmp.size()-1].c_str() );
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(app.getGraphics()->getWindow(), GLFW_STICKY_KEYS, 1);
    // set Callbacks
    glfwSetWindowSizeCallback(app.getGraphics()->getWindow(), resizeCallback);
    glfwSetKeyCallback(app.getGraphics()->getWindow(), keyCallback );
    glfwSetScrollCallback(app.getGraphics()->getWindow(), scrollCallback );
    glfwSetWindowFocusCallback(app.getGraphics()->getWindow(), focusCallback);
    glfwSetMouseButtonCallback(app.getGraphics()->getWindow(), mouseCallback);

    // Enable vertical sync (on cards that support it) with parameter 1 - 0 means off
    glfwSwapInterval( 0 );

    // OpenGL state:
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
            glfwSetWindowTitle(app.getGraphics()->getWindow(), sstream.str().c_str() );
            showNextFPS = now + FPSdelay;
            frameCount = 0;
        }


        if (app.isLocked() && app.getIgnoredMouseUpdates() == 0) {
            int stateW = glfwGetKey(app.getGraphics()->getWindow(), GLFW_KEY_W);
            int stateA = glfwGetKey(app.getGraphics()->getWindow(), GLFW_KEY_A);
            int stateS = glfwGetKey(app.getGraphics()->getWindow(), GLFW_KEY_S);
            int stateD = glfwGetKey(app.getGraphics()->getWindow(), GLFW_KEY_D);
            double xpos, ypos;
            glfwGetCursorPos(app.getGraphics()->getWindow(), &xpos, &ypos);
            glfwSetCursorPos(app.getGraphics()->getWindow(), app.getGraphics()->getWindowSize().x/2, app.getGraphics()->getWindowSize().y/2);
            app.getLevel()->update(now - lastUpdate,
                    glm::vec2((float)ypos-app.getGraphics()->getWindowSize().y/2,
                            (float)xpos-app.getGraphics()->getWindowSize().x/2),
                        stateW == GLFW_PRESS,stateA == GLFW_PRESS,stateS == GLFW_PRESS,stateD == GLFW_PRESS);
        }
        else {
            app.getLevel()->update(now - lastUpdate, glm::vec2(0.0f, 0.0f), false, false, false, false);
            if (app.isLocked()) {
                app.ignoredOneMouseUpdate();
            }
        }

        lastUpdate = now;
        app.getGraphics()->render(app.getLevel());
        
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

