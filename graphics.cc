#include "graphics.hh"

#include <iomanip>
#include <sstream>

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

using namespace ACGL::OpenGL;

Graphics::Graphics(glm::uvec2 windowSize, float nearPlane, float farPlane) {
    this->windowSize = windowSize;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

Graphics::Graphics() {
}

void Graphics::init() {
    // construct VAO to give shader correct Attribute locations
    SharedArrayBuffer ab = SharedArrayBuffer(new ArrayBuffer());
    ab->defineAttribute("aPosition", GL_FLOAT, 3);
    ab->defineAttribute("aTexCoord", GL_FLOAT, 2);
    ab->defineAttribute("aNormal", GL_FLOAT, 3);
    SharedVertexArrayObject vao = SharedVertexArrayObject(new VertexArrayObject());
    vao->attachAllAttributes(ab);

    // look up all shader files starting with 'phong' and build a ShaderProgram from it:
    lightingShader = ShaderProgramCreator("phong").attributeLocations(
            vao->getAttributeLocations()).create();

    depthTexture = SharedTexture2D( new Texture2D(glm::vec2(windowSize.x, windowSize.y), GL_DEPTH24_STENCIL8));
    depthTexture->setMinFilter(GL_LINEAR);
    depthTexture->setMagFilter(GL_LINEAR);
    depthTexture->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);

    framebuffer = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer->setDepthTexture(depthTexture);
    framebuffer->validate();

    depthShader = ShaderProgramCreator("depth")
        .attributeLocations(vao->getAttributeLocations())
        .fragmentDataLocations(framebuffer->getAttachmentLocations())
        .create();
}

GLFWwindow* Graphics::getWindow() {
    return window;
}

glm::uvec2 Graphics::getWindowSize() {
    return windowSize;
}

void Graphics::render(Level* level)
{
    // render depth texture for sun
    framebuffer->bind(); 
    glClear(GL_DEPTH_BUFFER_BIT);
    depthShader->use();
    glm::vec3 sunVector = (level->getCameraCenter()->getPosition() + level->getDirectionalLight()->getPosition());
    glm::mat4 depthViewProjectionMatrix =  glm::ortho<float>(-20, 20, -20, 20, -20, 40) * 
        glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
    depthShader->setUniform("viewProjectionMatrix", depthViewProjectionMatrix);
    level->render(depthShader, false);
    if (!framebuffer->isFrameBufferObjectComplete()) {
        printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
    }

    // final render pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader->use();

    //set view and projection matrix
    glm::mat4 lightingViewProjectionMatrix = buildFrustum(75.0f, 0.1f, farPlane, (float)windowSize.x/(float)windowSize.y) * buildViewMatrix(level);
    lightingShader->setUniform("lightingViewProjectionMatrix", lightingViewProjectionMatrix);


    // convert texture to homogenouse coordinates
    glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
    glm::mat4 depthBiasMVP = biasMatrix*depthViewProjectionMatrix;
    
    lightingShader->setUniform("shadowMVP", depthBiasMVP);
    lightingShader->setTexture("shadowMap", depthTexture, 1);

    //set lighting parameters
    if (level->getLights().size() > 0) {
        lightingShader->setUniform("lightCount", (int) level->getLights().size());

        // TODO look into doing this less often
        // Build light position array
        glm::vec3 lightSources[level->getLights().size()];
        for(unsigned int i = 0; i<level->getLights().size(); i++) {
            lightSources[i] = level->getLights()[i].getPosition();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightSources"),
            sizeof(lightSources),  (GLfloat*) lightSources);
        // Build light colour array
        glm::vec3 lightColours[level->getLights().size()];
        for(unsigned int i = 0; i<level->getLights().size(); i++) {
            lightColours[i] = level->getLights()[i].getColour();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightColors"),
            sizeof(lightColours),  (GLfloat*) lightColours);
        // Build light attenuation array
        float lightIntensities[level->getLights().size()];
        for(unsigned int i = 0; i<level->getLights().size(); i++) {
            lightIntensities[i] = level->getLights()[i].getIntensity();
        }
        glUniform1fv(lightingShader->getUniformLocation("lightIntensities"),
            sizeof(lightIntensities),  (GLfloat*) lightIntensities);
    }
    // set directional Light
    if(level->getDirectionalLight()) {
        lightingShader->setUniform("directionalLightVector",
            level->getDirectionalLight()->getPosition());
        lightingShader->setUniform("directionalColor",
            level->getDirectionalLight()->getColour());
        lightingShader->setUniform("directionalIntensity",
            level->getDirectionalLight()->getIntensity());
    }

    // set fog Parameters
    lightingShader->setUniform("fogEnd", (farPlane)-35.0f);
    lightingShader->setUniform("fogColor", level->getFogColor());
    lightingShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());

    // set Material Parameters
    lightingShader->setUniform("ambientColor", level->getAmbientLight());
    lightingShader->setUniform("camera", level->getCameraPosition());

    // render the level
    level->render(lightingShader, true);
}

void Graphics::resize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
    depthTexture->resize(glm::vec2(windowSize.x, windowSize.y));
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
    //construct lookAt (cameraPosition = cameraCenter + cameraVector
    return glm::lookAt((level->getCameraCenter()->getPosition() + level->getCamera()->getVector()),
            level->getCameraCenter()->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
}

float Graphics::getFarPlane() {
    return farPlane;
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
    // Initialise GLFW
    if ( !glfwInit() )
    {
        ACGL::Utils::error() << "Failed to initialize GLFW" << std::endl;
        exit( -1 );
    }

    // Configure OpenGL context
    setGLFWHintsForOpenGLVersion( ACGL_OPENGL_VERSION );

    // activate multisampling (second parameter is the number of samples):
    //glfwWindowHint( GLFW_SAMPLES, 8 );

    // request an OpenGL debug context:
    glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, true );

    // define whether the window can get resized:
    glfwWindowHint(GLFW_RESIZABLE, true);

    // try to create an OpenGL context in a window and check the supported OpenGL version:
    //                                                  R,G,B,A, Depth,Stencil
    window = glfwCreateWindow(windowSize.x, windowSize.y, "SWP MarbleGame Group C", NULL, NULL);
    if (!getWindow()) {
        ACGL::Utils::error() << "Failed to open a GLFW window - requested OpenGL: " <<  ACGL_OPENGL_VERSION << std::endl;
        return false;
    }
    glfwMakeContextCurrent(window);
    ACGL::init();
    return true;
}
