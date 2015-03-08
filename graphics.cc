#include "graphics.hh"
#include "lodepng.h"

#include <iomanip>
#include <sstream>
#include <functional>

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

using namespace ACGL::OpenGL;

const double lightUpdateDelay = 0.5f;

Graphics::Graphics(glm::uvec2 windowSize, float nearPlane, 
    float farPlane, int cube_size,
    unsigned int maxShadowRenderCount) {
    this->windowSize = windowSize;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
    this->cube_size = cube_size;
    this->maxShadowRenderCount = maxShadowRenderCount;
}

Graphics::Graphics() {
}

void Graphics::init(Level* level) {
    // save Level
    this->level = level;

    // OpenGL state:
    glClearColor( 0.0, 0.0, 0.0, 1.0 );
    glEnable( GL_DEPTH_TEST );
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_MULTISAMPLE);

    fullscreen_quad_ab = SharedArrayBuffer(new ArrayBuffer());
    fullscreen_quad_ab->defineAttribute("aPosition", GL_FLOAT, 2);
    fullscreen_quad_ab->defineAttribute("aTexCoord", GL_FLOAT, 2);

    float quadData[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    fullscreen_quad_ab->setDataElements(6, quadData);

    fullscreen_quad = SharedVertexArrayObject(new VertexArrayObject);
    fullscreen_quad->attachAllAttributes(fullscreen_quad_ab);
    
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

    skydomeShader = ShaderProgramCreator("skydome").attributeLocations(
            vao->getAttributeLocations()).create();
    
    depthShader = ShaderProgramCreator("depth")
        .attributeLocations(vao->getAttributeLocations()).create();

    depthCubeShader = ShaderProgramCreator("depth_cube")
        .attributeLocations(vao->getAttributeLocations()).create();

    flame_positions_ab = SharedArrayBuffer(new ArrayBuffer());
    flame_positions_ab->defineAttribute("aPosition", GL_FLOAT, 3);
    flame_positions_ab->defineAttribute("aColor", GL_FLOAT, 3);
    flame_positions = SharedVertexArrayObject(new VertexArrayObject());
    flame_positions->setMode(GL_POINTS);
    flame_positions->attachAllAttributes(flame_positions_ab);

    flameShader = ShaderProgramCreator("flame")
        .attributeLocations(flame_positions->getAttributeLocations()).create();

    flameColorShader = ShaderProgramCreator("flame_color")
        .attributeLocations(fullscreen_quad->getAttributeLocations()).create();

    flamePostShader = ShaderProgramCreator("flame_post")
        .attributeLocations(fullscreen_quad->getAttributeLocations()).create();

    mergeShader = ShaderProgramCreator("merge")
        .attributeLocations(fullscreen_quad->getAttributeLocations()).create();

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &number_of_texture_units);
    printf("Your graphics card supports %d texture units.\n", number_of_texture_units);
    // Exit if we need more texture units
    if (number_of_texture_units < 19) {
        printf("You need at least 19 texture units to run this application. Exiting\n");
        exit(-1);
    }

    depth_directionalMaps = std::vector<SharedTexture2D>(3);
    framebuffer_directional = std::vector<SharedFrameBufferObject>(3);
    for (unsigned int i = 0; i<depth_directionalMaps.size(); i++) {
        depth_directionalMaps.at(i) = SharedTexture2D( new Texture2D(windowSize, GL_DEPTH_COMPONENT24));
        depth_directionalMaps.at(i)->setMinFilter(GL_NEAREST);
        depth_directionalMaps.at(i)->setMagFilter(GL_NEAREST);
        depth_directionalMaps.at(i)->setWrapS(GL_CLAMP_TO_EDGE);
        depth_directionalMaps.at(i)->setWrapT(GL_CLAMP_TO_EDGE);
        depth_directionalMaps.at(i)->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
    }

    for (unsigned int i = 0; i<framebuffer_directional.size(); i++) {
        framebuffer_directional.at(i) = SharedFrameBufferObject(new FrameBufferObject());
        framebuffer_directional.at(i)->setDepthTexture(depth_directionalMaps.at(i));
        framebuffer_directional.at(i)->validate();
    }

    lightingShader->use();



    // always generate and bind 10 cube maps, because otherwise the shader won't work
    depth_cubeMaps = std::vector<ACGL::OpenGL::SharedTextureCubeMap>(10);
    for (unsigned int i = 0; i<depth_cubeMaps.size(); i++) {
        depth_cubeMaps.at(i) = SharedTextureCubeMap(new TextureCubeMap(glm::vec2(cube_size, cube_size), GL_DEPTH_COMPONENT24));
        depth_cubeMaps.at(i)->setMinFilter(GL_NEAREST);
        depth_cubeMaps.at(i)->setMagFilter(GL_NEAREST);
        depth_cubeMaps.at(i)->setWrapS(GL_CLAMP_TO_EDGE);
        depth_cubeMaps.at(i)->setWrapT(GL_CLAMP_TO_EDGE);
        depth_cubeMaps.at(i)->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
    }
    
    framebuffer_cube = SharedFrameBufferObject(new FrameBufferObject());


    light_fbo_color_texture = SharedTexture2D(new Texture2D(windowSize, GL_RGBA8));
    light_fbo_color_texture->setMinFilter(GL_NEAREST);
    light_fbo_color_texture->setMagFilter(GL_NEAREST);
    light_fbo_color_texture->setWrapS(GL_CLAMP_TO_BORDER);
    light_fbo_color_texture->setWrapT(GL_CLAMP_TO_BORDER);
    light_fbo_depth_texture = SharedTexture2D(new Texture2D(windowSize, GL_DEPTH24_STENCIL8));
    light_fbo_depth_texture->setMinFilter(GL_NEAREST);
    light_fbo_depth_texture->setMagFilter(GL_NEAREST);
    light_fbo_depth_texture->setWrapS(GL_CLAMP_TO_BORDER);
    light_fbo_depth_texture->setWrapT(GL_CLAMP_TO_BORDER);
    framebuffer_light = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer_light->attachColorTexture("oColor", light_fbo_color_texture);
    framebuffer_light->setDepthTexture(light_fbo_depth_texture);
    framebuffer_light->setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    framebuffer_light->validate();

    flamePostShader->use();
    flamePostShader->setUniform("windowSizeX", int(windowSize.x));
    flamePostShader->setUniform("windowSizeY", int(windowSize.y));


    flame_fbo_color_texture = SharedTexture2D(new Texture2D(windowSize, GL_RGBA8));
    flame_fbo_color_texture->setMinFilter(GL_NEAREST);
    flame_fbo_color_texture->setMagFilter(GL_NEAREST);
    flame_fbo_color_texture->setWrapS(GL_CLAMP_TO_BORDER);
    flame_fbo_color_texture->setWrapT(GL_CLAMP_TO_BORDER);
    framebuffer_flame = SharedFrameBufferObject(new FrameBufferObject());
    framebuffer_flame->attachColorTexture("oColor", flame_fbo_color_texture);
    framebuffer_flame->setDepthTexture(light_fbo_depth_texture);
    framebuffer_flame->setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
    framebuffer_flame->validate();

    bindTextureUnits();

    updateClosestLights();
}

glm::uvec2 Graphics::getWindowSize() {
    return windowSize;
}

void Graphics::bindTextureUnits() {
    lightingShader->use();
    for (unsigned int i = 0; i<depth_directionalMaps.size(); i++) {
        // start with texture unit 1 because the first is reserved for the texture
        lightingShader->setTexture("shadowMap_directional" + std::to_string(i), depth_directionalMaps.at(i), i+1);
    }
    if (level->getLights()->size() > 0) {
        for(unsigned int i = 0; i<depth_cubeMaps.size(); i++){
            // start with texture unit 4 because the first four are used by the texture and the directional shadow map
            lightingShader->setTexture("shadowMap_cube" + std::to_string(i), depth_cubeMaps.at(i), i+4);
        }
    }
    flamePostShader->use();
    flamePostShader->setTexture("light_fbo", light_fbo_color_texture, 14);
    skydomeShader->use();
    skydomeShader->setTexture("nightTexture", level->getSkydome()->getNightTexture()->getReference(), 15);
    mergeShader->use();
    mergeShader->setTexture("flame_fbo", flame_fbo_color_texture, 16);
    mergeShader->setTexture("light_fbo", light_fbo_color_texture, 17);

    flameColorShader->use();
    flameColorShader->setTexture("flame_fbo", flame_fbo_color_texture, 18);
}

void Graphics::render(double time)
{
    // At first render shadows
    depthCubeShader->use();
    depthCubeShader->setUniform("farPlane", farPlane);
    // render depth textures for point lights
    glViewport(0, 0, cube_size, cube_size);
    glm::mat4 depthProjectionMatrix_pointlights = glm::perspective(1.571f, (float)cube_size/(float)cube_size, 0.1f,  farPlane);
    glm::vec3 looking_directions[6] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 upvectors[6] = {glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)};


    framebuffer_cube->bind();
    for (unsigned int i_pointlight = 0; i_pointlight<closestLights.size() && i_pointlight < maxShadowRenderCount; i_pointlight++) {
        // render each side of the cube
        for (int i_face = 0; i_face<6; i_face++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, depth_cubeMaps.at(i_pointlight)->getObjectName(), 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glm::mat4 viewMatrix = glm::lookAt(closestLights.at(i_pointlight).getPosition(),
                closestLights.at(i_pointlight).getPosition() + looking_directions[i_face], upvectors[i_face]);
            glm::mat4 depthViewProjectionMatrix_face = depthProjectionMatrix_pointlights * viewMatrix;
            std::vector<glm::mat4> viewMatrixVector = std::vector<glm::mat4>();
            viewMatrixVector.push_back(viewMatrix);
            level->render(depthCubeShader, false, &depthViewProjectionMatrix_face, &viewMatrixVector);
            if (!framebuffer_cube->isFrameBufferObjectComplete()) {
                printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
            }
        }
    }

    // render depth textures for sun
    depthShader->use();
    glViewport(0, 0, windowSize.x, windowSize.y);
   
    std::vector<glm::mat4> depthViewProjectionMatrices = std::vector<glm::mat4>(framebuffer_directional.size());
    glm::vec3 sunVector = (level->getCameraCenter()->getPosition() + level->getDirectionalLight()->getPosition());

    for (unsigned int i = 0; i<framebuffer_directional.size(); i++) {
        framebuffer_directional.at(i)->bind(); 
        glClear(GL_DEPTH_BUFFER_BIT);
        float projection_size = 0.0f;
        switch(i) {
            case 0:
                projection_size = 10.0f;
                break;
            case 1:
                projection_size = 30.0f;
                break;
            case 2:
                projection_size = farPlane/1.5f;
                break;
        }
        depthViewProjectionMatrices.at(i) =  glm::ortho<float>(-projection_size, projection_size, -projection_size, projection_size, -farPlane/1.5f, farPlane/1.5f) *
            glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
        level->render(depthShader, false, &depthViewProjectionMatrices.at(i));
        if (!framebuffer_directional.at(i)->isFrameBufferObjectComplete()) {
            printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
        }
    }
    
    // lighting render pass
    framebuffer_light->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set view and projection matrix
    glm::mat4 lightingViewProjectionMatrix = glm::perspective(1.571f, (float)windowSize.x/(float)windowSize.y, 0.1f, farPlane) * buildViewMatrix(level);

    //render skydome
    skydomeShader->use();
    // set fog Parameters
    skydomeShader->setUniform("farPlane", farPlane);
    skydomeShader->setUniform("skydomeSize", level->getSkydomeSize());
    skydomeShader->setUniform("fogColor", level->getFogColour());
    skydomeShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());
    skydomeShader->setUniform("directionalVector", level->getDirectionalLight()->getPosition());
    skydomeShader->setUniform("sunColor", level->getDirectionalLight()->getColour());
    level->getSkydome()->render(skydomeShader, false, true, &lightingViewProjectionMatrix);
    
    lightingShader->use();

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

    std::vector<glm::mat4> depthBiasVPs = std::vector<glm::mat4>(depthViewProjectionMatrices.size());
    for (unsigned int i = 0; i<depthBiasVPs.size(); i++) {
        depthBiasVPs.at(i) = biasMatrix * depthViewProjectionMatrices.at(i);
    }

    lightingShader->setUniform("farPlane", farPlane);
    
    // set fog Parameters
    lightingShader->setUniform("fogColor", level->getFogColour());
    lightingShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());
    
    // set Material Parameters
    lightingShader->setUniform("ambientColor", level->getAmbientLight());
    lightingShader->setUniform("camera", level->getPhysics()->getCameraPosition());
    
    // render the level
    level->render(lightingShader, true, &lightingViewProjectionMatrix, &depthBiasVPs);

    // draw flames on top
    flameShader->use();
    framebuffer_flame->bind();
    glClear(GL_COLOR_BUFFER_BIT);
    // cull faces to get consistent color while using alpha
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // draw with colors
    flameShader->setUniform("viewProjectionMatrix", lightingViewProjectionMatrix);
    flameShader->setUniform("modelViewProjectionMatrix", lightingViewProjectionMatrix);
    flameShader->setUniform("withColor", true);
    flameShader->setUniform("time", (float) time);
    flameShader->setUniform("bottom", true);
    flameShader->setUniform("left", true);
    flame_positions->render();
    flameShader->setUniform("left", false);
    flame_positions->render();
    flameShader->setUniform("bottom", false);
    flameShader->setUniform("left", true);
    flame_positions->render();
    flameShader->setUniform("left", false);
    flame_positions->render();
    glDisable(GL_CULL_FACE);

    glDepthMask(GL_FALSE);
    flameColorShader->use();
    fullscreen_quad->render();

    framebuffer_light->bind();
    mergeShader->use();
    glDisable(GL_DEPTH_TEST);
    fullscreen_quad->render();
    glEnable(GL_DEPTH_TEST);


    // draw slightly larger only for stencil buffer to blur edges
    flameShader->use();
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF); //Set any stencil to 1
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);//write to stencil buffer
    glClear(GL_STENCIL_BUFFER_BIT);//clear stencil buffer

    glm::mat4 modelMatrix = glm::scale(glm::vec3(1.1f));
    flameShader->setUniform("viewProjectionMatrix", lightingViewProjectionMatrix);
    flameShader->setUniform("modelViewProjectionMatrix", lightingViewProjectionMatrix * modelMatrix);
    flameShader->setUniform("withColor", false);
    flameShader->setUniform("time", (float) time);
    flameShader->setUniform("bottom", true);
    flameShader->setUniform("left", true);
    flame_positions->render();
    flameShader->setUniform("left", false);
    flame_positions->render();
    flameShader->setUniform("bottom", false);
    flameShader->setUniform("left", true);
    flame_positions->render();
    flameShader->setUniform("left", false);
    flame_positions->render();

    glStencilFunc(GL_EQUAL, 1, 0xFF); //Pass test if stencil value is 1
    glStencilMask(0x00);// don't write to stencil buffer

    flamePostShader->use();
    fullscreen_quad->render();
    glDepthMask(GL_TRUE);

    glDisable(GL_STENCIL_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_light->getObjectName());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, windowSize.x, windowSize.y, 0, 0, windowSize.x, windowSize.y,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

bool Graphics::compareLightDistances(Light a, Light b) {
    if (glm::distance(this->level->getCameraCenter()->getPosition(), a.getPosition()) < 
            glm::distance(this->level->getCameraCenter()->getPosition(), b.getPosition())) {
        return true;
    }
    else {
        return false;
    }
}

void Graphics::updateClosestLights() {
    closestLights = std::vector<Light>(*level->getLights());
    std::sort(closestLights.begin(),
        closestLights.end(),
        [this](Light a, Light b) {return compareLightDistances(a, b); });
    if (level->getLights()->size() > 32) {
        closestLights = std::vector<Light>(&closestLights[0],
                &closestLights[31]);
    }
}

void Graphics::updateLights() {
    updateClosestLights();
    if (closestLights.size() > 0) {
        lightingShader->setUniform("lightCount", (int) closestLights.size());
        lightingShader->setUniform("maxShadowRenderCount", std::min((int) closestLights.size(), (int)maxShadowRenderCount));

        // Build light position array
        glm::vec3 lightSources[closestLights.size()];
        for(unsigned int i = 0; i<closestLights.size(); i++) {
            lightSources[i] = closestLights.at(i).getPosition();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightSources"),
            sizeof(lightSources),  (GLfloat*) lightSources);
        // Build light colour array
        glm::vec3 lightColours[closestLights.size()];
        for(unsigned int i = 0; i<closestLights.size(); i++) {
            lightColours[i] = closestLights.at(i).getColour();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightColors"),
            sizeof(lightColours),  (GLfloat*) lightColours);
        // Build light attenuation array
        float lightIntensities[closestLights.size()];
        for(unsigned int i = 0; i<closestLights.size(); i++) {
            lightIntensities[i] = closestLights.at(i).getIntensity();
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
    float* flameData = new float[closestLights.size() * 6];
    int flameIndex = 0;
    for (unsigned int i = 0; i<closestLights.size(); i++) {
        if (closestLights.at(i).getFlameYOffset() != 0.0f) {
            flameData[flameIndex + 0] = closestLights.at(i).getPosition().x;
            flameData[flameIndex + 1] = closestLights.at(i).getPosition().y + closestLights.at(i).getFlameYOffset();
            flameData[flameIndex + 2] = closestLights.at(i).getPosition().z;
            flameData[flameIndex + 3] = closestLights.at(i).getColour().r;
            flameData[flameIndex + 3] = closestLights.at(i).getColour().r;
            flameData[flameIndex + 4] = closestLights.at(i).getColour().g;
            flameData[flameIndex + 5] = closestLights.at(i).getColour().b;
            flameIndex+=6;
        }
    }
    flame_positions_ab->setDataElements(flameIndex, flameData);
}

void Graphics::resize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
    for (unsigned int i = 0; i<depth_directionalMaps.size(); i++) {
        depth_directionalMaps.at(i)->resize(glm::vec2(windowSize.x, windowSize.y));
    }
    light_fbo_color_texture->resize(windowSize);
    light_fbo_depth_texture->resize(windowSize);
    flame_fbo_color_texture->resize(windowSize);
    flamePostShader->setUniform("windowSizeX", int(windowSize.x));
    flamePostShader->setUniform("windowSizeY", int(windowSize.y));
    bindTextureUnits();
}

glm::mat4 Graphics::buildViewMatrix(Level* level) {
    //construct lookAt (cameraPosition = cameraCenter + cameraVector)
    if(level->getCamera()->getIsPhysicsCamera())
        return glm::lookAt(level->getCamera()->getPosition(), level->getCamera()->getPosition() + level->getCamera()->getDirection(), glm::vec3(0.0f, 1.0f, 0.0f));
    
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
