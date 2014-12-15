#include "graphics.hh"

#include <iomanip>
#include <sstream>

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

using namespace ACGL::OpenGL;

const int Graphics::cube_size = 1024;

Graphics::Graphics(glm::uvec2 windowSize, float nearPlane, float farPlane) {
    this->windowSize = windowSize;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
}

Graphics::Graphics() {
}

void Graphics::init(Level* level) {
    // save Level
    this->level = level;

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

    depthShader = ShaderProgramCreator("depth")
        .attributeLocations(vao->getAttributeLocations()).create();

    depthTexture_near = SharedTexture2D( new Texture2D(windowSize, GL_DEPTH_COMPONENT16));
    depthTexture_near->setMinFilter(GL_NEAREST);
    depthTexture_near->setMagFilter(GL_NEAREST);
    depthTexture_near->setWrapS(GL_CLAMP_TO_EDGE);
    depthTexture_near->setWrapT(GL_CLAMP_TO_EDGE);
    depthTexture_near->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);

    framebuffer_near = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer_near->setDepthTexture(depthTexture_near);
    framebuffer_near->validate();

    depthTexture_middle = SharedTexture2D( new Texture2D(windowSize, GL_DEPTH_COMPONENT16));
    depthTexture_middle->setMinFilter(GL_NEAREST);
    depthTexture_middle->setMagFilter(GL_NEAREST);
    depthTexture_middle->setWrapS(GL_CLAMP_TO_EDGE);
    depthTexture_middle->setWrapT(GL_CLAMP_TO_EDGE);
    depthTexture_middle->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);

    framebuffer_middle = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer_middle->setDepthTexture(depthTexture_middle);
    framebuffer_middle->validate();

    depthTexture_far = SharedTexture2D( new Texture2D(windowSize, GL_DEPTH_COMPONENT16));
    depthTexture_far->setMinFilter(GL_NEAREST);
    depthTexture_far->setMagFilter(GL_NEAREST);
    depthTexture_far->setWrapS(GL_CLAMP_TO_EDGE);
    depthTexture_far->setWrapT(GL_CLAMP_TO_EDGE);
    depthTexture_far->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);

    framebuffer_far = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer_far->setDepthTexture(depthTexture_far);
    framebuffer_far->validate();

    depth_cubeMaps = std::vector<ACGL::OpenGL::SharedTextureCubeMap>(level->getLights()->size());
    for (unsigned int i = 0; i<depth_cubeMaps.size(); i++) {
        depth_cubeMaps.at(i) = SharedTextureCubeMap(new TextureCubeMap(glm::vec2(cube_size, cube_size), GL_DEPTH_COMPONENT16));
        depth_cubeMaps.at(i)->setMinFilter(GL_NEAREST);
        depth_cubeMaps.at(i)->setMagFilter(GL_NEAREST);
        depth_cubeMaps.at(i)->setWrapS(GL_CLAMP_TO_EDGE);
        depth_cubeMaps.at(i)->setWrapT(GL_CLAMP_TO_EDGE);
        depth_cubeMaps.at(i)->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
    }

    framebuffer_cube = SharedFrameBufferObject(new FrameBufferObject());
}

GLFWwindow* Graphics::getWindow() {
    return window;
}

glm::uvec2 Graphics::getWindowSize() {
    return windowSize;
}

void Graphics::render()
{
    // At first render shadows
    depthShader->use();
    // render depth textures for point lights
    glViewport(0, 0, cube_size, cube_size);
    glm::mat4 depthProjectionMatrix_pointlights = glm::perspective(1.571f, (float)cube_size/(float)cube_size, 0.1f,  farPlane);
    glm::vec3 looking_directions[6] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)};

    framebuffer_cube->bind();
    for (unsigned int i_pointlight = 0; i_pointlight<level->getLights()->size(); i_pointlight++) {
        // render each side of the cube
        for (int i_face = 0; i_face<6; i_face++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, depth_cubeMaps.at(i_pointlight)->getObjectName(), 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glm::mat4 depthViewProjectionMatrix_face = depthProjectionMatrix_pointlights * glm::lookAt(level->getLights()->at(i_pointlight).getPosition(),
                level->getLights()->at(i_pointlight).getPosition() + looking_directions[i_face], glm::vec3(0.0f, 1.0f, 0.0f));
            depthShader->setUniform("viewProjectionMatrix", depthViewProjectionMatrix_face);
            level->render(depthShader, false, &depthViewProjectionMatrix_face);
            if (!framebuffer_cube->isFrameBufferObjectComplete()) {
                printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
            }
        }
    }
    // render depth texture for sun
    glViewport(0, 0, windowSize.x, windowSize.y);
    // near pass
    framebuffer_near->bind(); 
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::vec3 sunVector = (level->getCameraCenter()->getPosition() + level->getDirectionalLight()->getPosition());
    glm::mat4 depthViewProjectionMatrix_near =  glm::ortho<float>(-5, 5, -5, 5, -5, 5) * 
        glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
    level->render(depthShader, false, &depthViewProjectionMatrix_near);
    if (!framebuffer_near->isFrameBufferObjectComplete()) {
        printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
    }

    // middle pass
    framebuffer_middle->bind(); 
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 depthViewProjectionMatrix_middle =  glm::ortho<float>(-20, 20, -20, 20, -20, 20) * 
        glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
    level->render(depthShader, false, &depthViewProjectionMatrix_middle);
    if (!framebuffer_middle->isFrameBufferObjectComplete()) {
        printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
    }
    
    // far pass
    framebuffer_far->bind(); 
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::mat4 depthViewProjectionMatrix_far =  glm::ortho<float>(-farPlane/2.0f, farPlane/2.0f, -farPlane/2.0f, farPlane/2.0f, -farPlane/2.0f, farPlane/2.0f) * 
        glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
    level->render(depthShader, false, &depthViewProjectionMatrix_far);
    if (!framebuffer_far->isFrameBufferObjectComplete()) {
        printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
    }

    // final render pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader->use();

    //set lighting parameters
    if (level->getLights()->size() > 0) {
        lightingShader->setUniform("lightCount", (int) level->getLights()->size());

        // TODO look into doing this less often, offload to another thread?
        // TODO figure out how to deal with bigger numbers of lights. load the nearest on demand?
        // Build light position array
        glm::vec3 lightSources[level->getLights()->size()];
        for(unsigned int i = 0; i<level->getLights()->size(); i++) {
            lightSources[i] = level->getLights()->at(i).getPosition();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightSources"),
            sizeof(lightSources),  (GLfloat*) lightSources);
        // Build light colour array
        glm::vec3 lightColours[level->getLights()->size()];
        for(unsigned int i = 0; i<level->getLights()->size(); i++) {
            lightColours[i] = level->getLights()->at(i).getColour();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightColors"),
            sizeof(lightColours),  (GLfloat*) lightColours);
        // Build light attenuation array
        float lightIntensities[level->getLights()->size()];
        for(unsigned int i = 0; i<level->getLights()->size(); i++) {
            lightIntensities[i] = level->getLights()->at(i).getIntensity();
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

    // convert texture to homogenouse coordinates
    glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
    glm::mat4 depthBiasVP_near = biasMatrix*depthViewProjectionMatrix_near;
    glm::mat4 depthBiasVP_middle = biasMatrix*depthViewProjectionMatrix_middle;
    glm::mat4 depthBiasVP_far = biasMatrix*depthViewProjectionMatrix_far;

    lightingShader->setTexture("shadowMap_near", depthTexture_near, 1);
    lightingShader->setTexture("shadowMap_middle", depthTexture_middle, 2);
    lightingShader->setTexture("shadowMap_far", depthTexture_far, 3);

    // set fog Parameters
    lightingShader->setUniform("fogEnd", (farPlane)-35.0f);
    lightingShader->setUniform("fogColor", level->getFogColour());
    lightingShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());

    // set Material Parameters
    lightingShader->setUniform("ambientColor", level->getAmbientLight());
    lightingShader->setUniform("camera", level->getCameraPosition());

    //set view and projection matrix
    glm::mat4 lightingViewProjectionMatrix = glm::perspective(1.571f, (float)windowSize.x/(float)windowSize.y, 0.1f, farPlane) * buildViewMatrix(level);

    std::vector<glm::mat4> shadowVPs = std::vector<glm::mat4>();
    shadowVPs.push_back(depthBiasVP_near);
    shadowVPs.push_back(depthBiasVP_middle);
    shadowVPs.push_back(depthBiasVP_far);

    // render the level
    level->render(lightingShader, true, &lightingViewProjectionMatrix, &shadowVPs);
}

void Graphics::resize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
    depthTexture_near->resize(glm::vec2(windowSize.x, windowSize.y));
    depthTexture_middle->resize(glm::vec2(windowSize.x, windowSize.y));
    depthTexture_far->resize(glm::vec2(windowSize.x, windowSize.y));
}

glm::mat4 Graphics::buildViewMatrix(Level* level) {
    //construct lookAt (cameraPosition = cameraCenter + cameraVector)
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
