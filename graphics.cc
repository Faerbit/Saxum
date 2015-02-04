#include "graphics.hh"
#include "lodepng.h"

#include <iomanip>
#include <sstream>

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

using namespace ACGL::OpenGL;

const int Graphics::cube_size = 1024;
const double lightUpdateDelay = 0.5f;

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

    // update lights on creation
    lastUpdate = -lightUpdateDelay;

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

    depthTexture = SharedTexture2D( new Texture2D(windowSize, GL_DEPTH_COMPONENT16));
    depthTexture->setMinFilter(GL_NEAREST);
    depthTexture->setMagFilter(GL_NEAREST);
    depthTexture->setWrapS(GL_CLAMP_TO_EDGE);
    depthTexture->setWrapT(GL_CLAMP_TO_EDGE);
    depthTexture->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);

    framebuffer = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer->setDepthTexture(depthTexture);
    framebuffer->validate();

    /*depth_cubeMaps = std::vector<ACGL::OpenGL::SharedTextureCubeMap>(level->getLights()->size());
    for (unsigned int i = 0; i<depth_cubeMaps.size(); i++) {*/
    depth_cubeMaps = std::vector<ACGL::OpenGL::SharedTextureCubeMap>(std::min(int(level->getLights()->size()), 1));
    for (unsigned int i = 0; i<1 && i<depth_cubeMaps.size(); i++) {
        depth_cubeMaps.at(i) = SharedTextureCubeMap(new TextureCubeMap(glm::vec2(cube_size, cube_size), GL_DEPTH_COMPONENT16));
        depth_cubeMaps.at(i)->setMinFilter(GL_NEAREST);
        depth_cubeMaps.at(i)->setMagFilter(GL_NEAREST);
        depth_cubeMaps.at(i)->setWrapS(GL_CLAMP_TO_EDGE);
        depth_cubeMaps.at(i)->setWrapT(GL_CLAMP_TO_EDGE);
        depth_cubeMaps.at(i)->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
    }

    framebuffer_cube = SharedFrameBufferObject(new FrameBufferObject());
}

glm::uvec2 Graphics::getWindowSize() {
    return windowSize;
}

void Graphics::render(double time)
{
    // At first render shadows
    depthShader->use();
    depthShader->setUniform("farPlane", farPlane);
    // render depth textures for point lights
    glViewport(0, 0, cube_size, cube_size);
    glm::mat4 depthProjectionMatrix_pointlights = glm::perspective(1.571f, (float)cube_size/(float)cube_size, 0.1f,  farPlane);
    glm::vec3 looking_directions[6] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 upvectors[6] = {glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)};

    framebuffer_cube->bind();
    //for (unsigned int i_pointlight = 0; i_pointlight<level->getLights()->size(); i_pointlight++) {
    for (unsigned int i_pointlight = 0; i_pointlight<1 && i_pointlight<level->getLights()->size(); i_pointlight++) {
        // render each side of the cube
        for (int i_face = 0; i_face<6; i_face++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, depth_cubeMaps.at(i_pointlight)->getObjectName(), 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glm::mat4 viewMatrix = glm::lookAt(level->getLights()->at(i_pointlight).getPosition(),
                level->getLights()->at(i_pointlight).getPosition() + looking_directions[i_face], upvectors[i_face]);
            glm::mat4 depthViewProjectionMatrix_face = depthProjectionMatrix_pointlights * viewMatrix;
            std::vector<glm::mat4> viewMatrixVector = std::vector<glm::mat4>();
            viewMatrixVector.push_back(viewMatrix);
            level->render(depthShader, false, &depthViewProjectionMatrix_face, &viewMatrixVector);
            level->render(depthShader, false, &viewMatrix);
            if (!framebuffer_cube->isFrameBufferObjectComplete()) {
                printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
            }
        }
    }
    // render depth texture for sun
    glViewport(0, 0, windowSize.x, windowSize.y);
    
    // far pass
    framebuffer->bind(); 
    glClear(GL_DEPTH_BUFFER_BIT);
    glm::vec3 sunVector = (level->getCameraCenter()->getPosition() + level->getDirectionalLight()->getPosition());
    glm::mat4 depthViewProjectionMatrix =  glm::ortho<float>(-farPlane/2.0f, farPlane/2.0f, -farPlane/2.0f, farPlane/2.0f, -farPlane/2.0f, farPlane/2.0f) * 
        glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
    level->render(depthShader, false, &depthViewProjectionMatrix);
    if (!framebuffer->isFrameBufferObjectComplete()) {
        printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
    }

    // final render pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lightingShader->use();

    if (level->getLights()->size() > 0) {
        lightingShader->setTexture("shadowMap_cube", depth_cubeMaps.at(0), 4);
    }

    //set lighting parameters

    // TODO look into doing this less often, offload to another thread?
    // TODO figure out how to deal with bigger numbers of lights. load the nearest on demand?
    double nextUpdate = lastUpdate + lightUpdateDelay;
    if (time >= nextUpdate)
    {
        updateLights();
        lastUpdate = time;
    }

    // convert texture to homogenouse coordinates
    glm::mat4 biasMatrix(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
    glm::mat4 depthBiasVP = biasMatrix*depthViewProjectionMatrix;

    lightingShader->setTexture("shadowMap", depthTexture, 1);

    lightingShader->setUniform("farPlane", farPlane);

    // set fog Parameters
    lightingShader->setUniform("fogColor", level->getFogColour());
    lightingShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());

    // set Material Parameters
    lightingShader->setUniform("ambientColor", level->getAmbientLight());
    lightingShader->setUniform("camera", level->getCameraPosition());

    //set view and projection matrix
    glm::mat4 lightingViewProjectionMatrix = glm::perspective(1.571f, (float)windowSize.x/(float)windowSize.y, 0.1f, farPlane) * buildViewMatrix(level);

    std::vector<glm::mat4> shadowVPs = std::vector<glm::mat4>();
    shadowVPs.push_back(depthBiasVP);

    // render the level
    level->render(lightingShader, true, &lightingViewProjectionMatrix, &shadowVPs);
}

void Graphics::updateLights() {
    if (level->getLights()->size() > 0) {
        lightingShader->setUniform("lightCount", (int) level->getLights()->size());

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
}

void Graphics::resize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
    depthTexture->resize(glm::vec2(windowSize.x, windowSize.y));
}

glm::mat4 Graphics::buildViewMatrix(Level* level) {
    //construct lookAt (cameraPosition = cameraCenter + cameraVector)
    //return glm::lookAt(level->getCamera()->getPosition(), level->getCamera()->getPosition() + level->getCamera()->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
    
    return glm::lookAt((level->getCameraCenter()->getPosition() + level->getCamera()->getVector()),
            level->getCameraCenter()->getPosition(), glm::vec3(0.0f, 1.0f, 0.0f));
}

float Graphics::getFarPlane() {
    return farPlane;
}

void Graphics::saveDepthBufferToDisk(int face, std::string filename) {
    printf("Starting saving of depth buffer...\n");
    float *depthbuffer = new float[1024*1024];
    std::vector<unsigned char> image (1024 * 1024 * 4);

    glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depthbuffer);
    for (unsigned int i = 0; i<1024*1024; i++) {
        image[i * 4 + 0] = depthbuffer[i] * 255;
        image[i * 4 + 1] = depthbuffer[i] * 255;
        image[i * 4 + 2] = depthbuffer[i] * 255;
        image[i * 4 + 3] = 255;
    }
    unsigned error = lodepng::encode(filename.c_str(), image, 1024, 1024);
    if (error) {
        std::cout << "Encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
    else {
        printf("Saving complete!\n");
    }
    delete [] depthbuffer;
}
