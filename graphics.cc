#include "graphics.hh"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <ACGL/OpenGL/glloaders/extensions.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/Utils/StringHelpers.hh>

#include "model.hh"

using namespace std;

Graphics::Graphics() {
}

GLFWwindow* Graphics::getWindow() {
    return window;
}

void Graphics::setGLFWHintsForOpenGLVersion( unsigned int _version )
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

bool Graphics::createWindow()
{
    /////////////////////////////////////////////////////////////////////////////////////
    // Initialise GLFW
    //
    if ( !glfwInit() )
    {
        ACGL::Utils::error() << "Failed to initialize GLFW" << endl;
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
    glfwWindowHint(GLFW_RESIZABLE, false);

    // non-decorated windows can be used as splash screens:
    //glfwWindowHint( GLFW_DECORATED, false );

    /////////////////////////////////////////////////////////////////////////////////////
    // try to create an OpenGL context in a window and check the supported OpenGL version:
    //                                                  R,G,B,A, Depth,Stencil
    window = glfwCreateWindow(windowSize.x, windowSize.y, "SWP MarbleGame Group C", NULL, NULL);
    if (!getWindow()) {
        ACGL::Utils::error() << "Failed to open a GLFW window - requested OpenGL: " <<  ACGL_OPENGL_VERSION << endl;
        return false;
    }
    glfwMakeContextCurrent(window);
    ACGL::init();
    return true;
}

Graphics::Graphics(glm::uvec2 windowSize, float nearPlane, float farPlane) {
    this->windowSize = windowSize;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}


void Graphics::render(Level* level, ACGL::OpenGL::SharedShaderProgram shader)
{
    // clear the framebuffer:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set view and projection matrix
    shader->setUniform("projectionMatrix", buildFrustum(75.0f, 0.1f, 100.0f, (float)windowSize.x/(float)windowSize.y) );
    shader->setUniform("viewMatrix", buildViewMatrix(level));

    //set lighting parameters
    if (level->getLights().size() > 0) {
        shader->setUniform("lightCount", (int) level->getLights().size());

        // TODO look into doing this less often
        // Build light position array
        glm::vec3 lightSources[level->getLights().size()];
        for(unsigned int i = 0; i<level->getLights().size(); i++) {
            lightSources[i] = level->getLights()[i].getPosition();
        }
        glUniform3fv(shader->getUniformLocation("lightSources"),
            sizeof(lightSources),  (GLfloat*) lightSources);
        // Build light colour array
        glm::vec3 lightColours[level->getLights().size()];
        for(unsigned int i = 0; i<level->getLights().size(); i++) {
            lightColours[i] = level->getLights()[i].getColour();
        }
        glUniform3fv(shader->getUniformLocation("lightColors"),
            sizeof(lightColours),  (GLfloat*) lightColours);
        // Build light attenuation array
        float lightIntensities[level->getLights().size()];
        for(unsigned int i = 0; i<level->getLights().size(); i++) {
            lightIntensities[i] = level->getLights()[i].getIntensity();
        }
        glUniform1fv(shader->getUniformLocation("lightIntensities"),
            sizeof(lightIntensities),  (GLfloat*) lightIntensities);
    }

    // set Material Parameters
    shader->setUniform("ambientColor", level->getAmbientLight());
    shader->setUniform("camera", glm::vec3(0.0f, 0.0f, 0.0f));

    // render the level(currently only a bunny):
    level->render();
}

void Graphics::setWindowSize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
}

void resizeCallback(Graphics* graphics, int newWidth, int newHeight)
{
    // store the new window size and adjust the viewport:
    graphics->setWindowSize(glm::uvec2( newWidth, newHeight));
    glViewport( 0, 0, newWidth, newHeight);
}

glm::mat4 Graphics::buildFrustum( float phiInDegree, float _near, float _far, float aspectRatio) {

    float phiHalfInRadians = 0.5*phiInDegree * (M_PI/180.0);
    float top = _near * tan( phiHalfInRadians );
    float bottom = -top;
    float left  = bottom * aspectRatio;
    float right = -left;

    return glm::frustum(left, right, bottom, top, _near, _far);
}

glm::mat4 Graphics::buildViewMatrix(Level* level) {
    glm::vec4 cameraVector = glm::vec4(0.0f, 0.0f, level->getCamera().getDistance(), 0.0f);
    // rotate vector
    glm::mat4 rotationMatrix = glm::rotate<float>(level->getCamera().getRotation()[0], glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate<float>(level->getCamera().getRotation()[1], glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate<float>(level->getCamera().getRotation()[2], glm::vec3(0.0f, 0.0f, 1.0f));
    cameraVector = rotationMatrix * cameraVector;
    //construct lookAt (cameraPosition = cameraCenter + cameraVector
    return glm::lookAt(level->getCameraCenter()->getPosition() + glm::vec3(cameraVector), level->getCameraCenter()->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
}
