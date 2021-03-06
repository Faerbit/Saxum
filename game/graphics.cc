#include "graphics.hh"
#include <lodepng/lodepng.h>

#include <iomanip>
#include <sstream>
#include <functional>

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include "LinearMath/btIDebugDraw.h"

using namespace ACGL::OpenGL;

const double lightUpdateDelay = 0.5f;
const double windUpdateDelay = 0.5f;
const int maxShadowSampleCount = 26;

Graphics::Graphics(glm::uvec2 windowSize, float nearPlane, 
    float farPlane, int cube_size,
    unsigned int maxShadowRenderCount,
    std::string screenPath,
    std::string screenContinuePath) {
    this->windowSize = windowSize;
    this->nearPlane = nearPlane;
    if (farPlane > 0) {
        this->farPlane = farPlane;
    }
    else {
        this->farPlane = 0;
    }
    if (cube_size > 0) {
        this->cube_size = cube_size;
    }
    else {
        this->cube_size = 0;
    }
    if (maxShadowRenderCount < maxShadowSampleCount) {
        this->maxShadowRenderCount = maxShadowRenderCount;
    }
    else {
        this->maxShadowRenderCount = 0;
    }
    this->loadingScreenPath = screenPath;
    this->loadingScreenContinuePath = screenContinuePath;
    gameStart = false;
    renderShadows = true;
    renderFlames = true;
    renderWorld = true;
    renderDebug = false;
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
    glEnable(GL_MULTISAMPLE);
    
    lastLightUpdate = 0;

    lastWindUpdate = - windUpdateDelay;
    windTarget = 0.0f;
    wind = glm::vec2(0.0f, 0.0f);
    windDirection = glm::vec2(1.0f, 1.0f);
    windDirectionTarget = glm::vec2(1.0f, 1.0f);
    textureMovementPosition = glm::vec2(0.0, 0.0);
    
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

    SharedArrayBuffer flame_positions_ab = SharedArrayBuffer(new ArrayBuffer());
    flame_positions_ab->defineAttribute("aPosition", GL_FLOAT, 3);
    flame_positions_ab->defineAttribute("aColor", GL_FLOAT, 3);
    SharedVertexArrayObject flame_positions = SharedVertexArrayObject(new VertexArrayObject());
    flame_positions->attachAllAttributes(flame_positions_ab);

    flameShader = ShaderProgramCreator("flame")
        .attributeLocations(flame_positions->getAttributeLocations()).create();

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

    flamePostShader = ShaderProgramCreator("flame_post")
        .attributeLocations(fullscreen_quad->getAttributeLocations()).create();

    debug_ab = SharedArrayBuffer(new ArrayBuffer());
    debug_ab->defineAttribute("aPosition", GL_FLOAT, 3);
    debug_ab->defineAttribute("aColor", GL_FLOAT, 3);
    debug_vao = SharedVertexArrayObject(new VertexArrayObject());
    debug_vao->attachAllAttributes(debug_ab);
    debug_vao->setMode(GL_LINES);

    debugShader = ShaderProgramCreator("debug")
        .attributeLocations(debug_vao->getAttributeLocations()).create();

    debugDrawer = DebugDraw();

    level->getPhysics()->getWorld()->setDebugDrawer(&debugDrawer);


    depth_directionalMaps = std::vector<SharedTexture2D>(5);
    framebuffer_directional = std::vector<SharedFrameBufferObject>(5);
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

    // always generate and bind all cube maps, because otherwise the shader won't work
    depth_cubeMaps = std::vector<ACGL::OpenGL::SharedTextureCubeMap>(maxShadowSampleCount);
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

    bindTextureUnits();

    // set shader variables that stay the same across the runtime of the application
    skydomeShader->use();
    skydomeShader->setUniform("farPlane", farPlane);
    skydomeShader->setUniform("skydomeSize", level->getSkydomeSize());
    skydomeShader->setUniform("fogColorDay", level->getFogColourDay());
    skydomeShader->setUniform("fogColorRise", level->getFogColourRise());
    skydomeShader->setUniform("fogColorNight", level->getFogColourNight());
    skydomeShader->setUniform("sunColor", level->getDirectionalLight()->getColour());

    lightingShader->use(); 
    lightingShader->setUniform("farPlane", farPlane);
    lightingShader->setUniform("fogColorDay", level->getFogColourDay());
    lightingShader->setUniform("fogColorRise", level->getFogColourRise());
    lightingShader->setUniform("fogColorNight", level->getFogColourNight());
    lightingShader->setUniform("ambientColor", level->getAmbientLight());

    if(level->getDirectionalLight()) {
        lightingShader->setUniform("directionalLightVector",
            level->getDirectionalLight()->getPosition());
        lightingShader->setUniform("directionalColor",
            level->getDirectionalLight()->getColour());
        lightingShader->setUniform("targetDirectionalIntensity",
            level->getDirectionalLight()->getIntensity());
    }

    depthCubeShader->use();
    depthCubeShader->setUniform("farPlane", farPlane);

    level->sortObjects(Material::getAllTextures()->size());

    #ifdef SAXUM_DEBUG
        std::cout << "There were " << Material::getAllTextures()->size()
                <<  " materials used in this level." << std::endl;
        cout << "There are " << level->checkMaxSurroundingLights() << " max surrounding lights." << endl;
    #endif

    initShadowRenderQueue();
    updateLights();
}

void Graphics::bindTextureUnits(){
    unsigned int textureCount = Material::getAllTextures()->size();
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &number_of_texture_units);
    printf("Your graphics card supports %d texture units.\n", number_of_texture_units);
    // Exit if we need more texture units
    if (number_of_texture_units < (int)textureCount + maxShadowSampleCount + 9) {
        printf("You need at least %d texture units to run this application. Exiting\n", textureCount + maxShadowSampleCount + 9);
        exit(-1);
    }

    loadingShader->use();
    loadingShader->setTexture("screen", loadingScreen, 0);
    loadingShader->setTexture("screenContinue", loadingContinueScreen, 1);

    lightingShader->use();
    for(unsigned int i = 0; i<Material::getAllTextures()->size(); i++) {
        lightingShader->setTexture("uTexture", Material::getAllTextures()->at(i), i+2);
    }

    for (unsigned int i = 0; i<depth_directionalMaps.size(); i++) {
        lightingShader->setTexture("shadowMap_directional" + std::to_string(i), depth_directionalMaps.at(i), textureCount + i + 2);
    }

    if (level->getLights()->size() > 0) {
        for(unsigned int i = 0; i<depth_cubeMaps.size(); i++){
            lightingShader->setTexture("shadowMap_cube" + std::to_string(i), depth_cubeMaps.at(i), textureCount + i + 7);
        }
    }
    flamePostShader->use();
    flamePostShader->setTexture("light_fbo", light_fbo_color_texture, textureCount + maxShadowSampleCount + 7);

    skydomeShader->use();
    skydomeShader->setTexture("dayTexture", level->getSkydome()->getDayTexture(), textureCount + maxShadowSampleCount + 8);
    skydomeShader->setTexture("nightTexture", level->getSkydome()->getNightTexture(), textureCount + maxShadowSampleCount + 9);

    printf("This application used %d texture units.\n", textureCount + maxShadowSampleCount + 9);
}

void Graphics::renderLoadingScreen() {
    loadingScreen = Texture2DFileManager::the()->get(Texture2DCreator(loadingScreenPath));
    loadingScreen->generateMipmaps();
    loadingScreen->setMinFilter(GL_NEAREST_MIPMAP_LINEAR);
    loadingScreen->setMagFilter(GL_LINEAR);
    loadingContinueScreen = Texture2DFileManager::the()->get(Texture2DCreator(loadingScreenContinuePath));
    loadingContinueScreen->generateMipmaps();
    loadingContinueScreen->setMinFilter(GL_NEAREST_MIPMAP_LINEAR);
    loadingContinueScreen->setMagFilter(GL_LINEAR);
    loadingScreenWidth = (float)loadingScreen->getWidth();
    loadingScreenHeight = (float)loadingScreen->getHeight();

    fullscreen_quad_ab_loading = SharedArrayBuffer(new ArrayBuffer());
    fullscreen_quad_ab_loading->defineAttribute("aPosition", GL_FLOAT, 2);
    fullscreen_quad_ab_loading->defineAttribute("aTexCoord", GL_FLOAT, 2);
    
    float quadData[24];
    if (loadingScreenWidth/loadingScreenHeight < ((float)windowSize.x)/((float)windowSize.y)) {
        float quadTemp[24] ={
        -(((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)),  1.0f,  0.0f, 1.0f,
         (((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)),  1.0f,  1.0f, 1.0f,
         (((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)), -1.0f,  1.0f, 0.0f,

         (((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)), -1.0f,  1.0f, 0.0f,
        -(((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)), -1.0f,  0.0f, 0.0f,
        -(((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)),  1.0f,  0.0f, 1.0f
        };
        for(int i = 0; i<24; i++) {
            quadData[i] = quadTemp[i];
        }
    }
    else {
        float quadTemp[24] = {
            -1.0f,  ((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  0.0f, 1.0f,
             1.0f,  ((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  1.0f, 1.0f,
             1.0f, -((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  1.0f, 0.0f,

             1.0f, -((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  1.0f, 0.0f,
            -1.0f, -((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  0.0f, 0.0f,
            -1.0f,  ((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  0.0f, 1.0f
        };
        for(int i = 0; i<24; i++) {
            quadData[i] = quadTemp[i];
        }
    }

    fullscreen_quad_ab_loading->setDataElements(6, quadData);

    fullscreen_quad_loading = SharedVertexArrayObject(new VertexArrayObject);
    fullscreen_quad_loading->attachAllAttributes(fullscreen_quad_ab_loading);
    loadingShader = ShaderProgramCreator("loading")
        .attributeLocations(fullscreen_quad_loading->getAttributeLocations()).create();
    loadingShader->use();
    loadingShader->setUniform("time", 0.0f);
    loadingShader->setTexture("screen", loadingScreen, 0);
    loadingShader->setTexture("screenContinue", loadingContinueScreen, 1);
    fullscreen_quad_loading->render();
}

glm::uvec2 Graphics::getWindowSize() {
    return windowSize;
}

void Graphics::render(double time)
{
    if (!gameStart) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, windowSize.x, windowSize.y);
        loadingShader->use();
        loadingShader->setUniform("time", float(time));
        fullscreen_quad_loading->render();
    }
    else {
        double nextLightUpdate = lastLightUpdate + lightUpdateDelay;
        if (time >= nextLightUpdate)
        {
            updateLights();
            lastLightUpdate = time;
        }

        // At first render shadows
        std::vector<glm::mat4> depthViewProjectionMatrices = std::vector<glm::mat4>(framebuffer_directional.size());
        if (renderShadows) {
            // update priorities
            for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
                shadowRenderQueue.at(i).currentPriority += shadowRenderQueue.at(i).priority;
            }
            // schedule lights with highest priority
            // tuple : Light, currentPriority, slot
            std::vector<std::tuple<std::shared_ptr<Light>, int, int>> renderQueue =
                std::vector<std::tuple<std::shared_ptr<Light>, int, int>>(maxShadowRenderCount);
            for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
                for(unsigned int j = 0; j<renderQueue.size(); j++){
                    if (shadowRenderQueue.at(i).currentPriority > std::get<1>(renderQueue.at(j))){
                        if (renderQueue.at(j) != renderQueue.back()) {
                            renderQueue.at(j+1) = renderQueue.at(j);
                        }
                        renderQueue.at(j) = std::make_tuple(shadowRenderQueue.at(i).light, shadowRenderQueue.at(i).currentPriority, i);
                        break;
                    }
                }
            }
            // reset currentPriority
            for(unsigned int i = 0; i<renderQueue.size(); i++) {
                shadowRenderQueue.at(std::get<2>(renderQueue.at(i))).currentPriority = 0;
            }

            depthCubeShader->use();
            // render depth textures for point lights
            glViewport(0, 0, cube_size, cube_size);
            depthCubeShader->use();
            glm::mat4 depthProjectionMatrix_pointlights = glm::perspective(1.571f, (float)cube_size/(float)cube_size, 0.1f,  50.0f);
            glm::vec3 looking_directions[6] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)};
            glm::vec3 upvectors[6] = {glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)};


            framebuffer_cube->bind();
            for (unsigned int i_pointlight = 0; i_pointlight < renderQueue.size(); i_pointlight++) {
                // check if queue points to a existing light
                if (std::get<0>(renderQueue.at(i_pointlight))) {
                    // render each side of the cube
                    glm::vec3 position = glm::vec3(0.0f);
                    if (std::get<0>(renderQueue.at(i_pointlight))->isFlame()) {
                        position = std::get<0>(renderQueue.at(i_pointlight))->getPosition();
                        position = glm::vec3(position.x - 0.75f*wind.y, position.y, position.z + 0.75f*wind.x);
                    }
                    else {
                        position = std::get<0>(renderQueue.at(i_pointlight))->getPosition();
                    }
                    for (int i_face = 0; i_face<6; i_face++) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face,
                                depth_cubeMaps.at(std::get<2>(renderQueue.at(i_pointlight)))->getObjectName(), 0);
                        glClear(GL_DEPTH_BUFFER_BIT);
                        glm::mat4 viewMatrix = glm::lookAt(position, position + looking_directions[i_face], upvectors[i_face]);
                        glm::mat4 depthViewProjectionMatrix_face = depthProjectionMatrix_pointlights * viewMatrix;
                        std::vector<glm::mat4> viewMatrixVector = std::vector<glm::mat4>(1);
                        viewMatrixVector.at(0) = viewMatrix;
                        level->render(depthCubeShader, false, std::get<0>(renderQueue.at(i_pointlight))->getPosition(), 1, &depthViewProjectionMatrix_face, &viewMatrixVector);
                        if (!framebuffer_cube->isFrameBufferObjectComplete()) {
                            printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
                        }
                    }
                }
            }

            glViewport(0, 0, windowSize.x, windowSize.y);

            // render depth textures for sun
            float sunAngle = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(level->getDirectionalLight()->getPosition()));
            glm::vec3 sunVector = (level->getCameraCenter()->getPosition() + level->getDirectionalLight()->getPosition());

            if (sunAngle > 0.0f) {
                depthShader->use();
                for (unsigned int i = 0; i<framebuffer_directional.size(); i++) {
                    framebuffer_directional.at(i)->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                        float projection_size = 0.0f;
                        switch(i) {
                            case 0:
                                projection_size = 10.0f;
                                break;
                            case 1:
                                projection_size = 20.0f;
                                break;
                            case 2:
                                projection_size = 40.0f;
                                break;
                            case 3:
                                projection_size = 60.0f;
                                break;
                            case 4:
                                projection_size = farPlane/1.5f;
                                break;
                        }
                        depthViewProjectionMatrices.at(i) =  glm::ortho<float>(-projection_size, projection_size, -projection_size, projection_size, -farPlane/1.5f, farPlane/1.5f) *
                            glm::lookAt(sunVector, level->getCameraCenter()->getPosition(), glm::vec3(0,1,0));
                        level->render(depthShader, false, level->getCameraCenter()->getPosition(), -1, &depthViewProjectionMatrices.at(i));
                        if (!framebuffer_directional.at(i)->isFrameBufferObjectComplete()) {
                            printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
                        }
                }
            }
        }
        
        // lighting render pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //wind
        double nextWindUpdate = lastWindUpdate + lightUpdateDelay;
        if (time >= nextWindUpdate)
        {
            const float windTargetEnd = 0.7f;
            windTarget = static_cast<float>(rand()) / static_cast<float>(RAND_MAX/pow(windTargetEnd, 2));
            windTarget = sqrt(windTarget);
            windTarget *= 0.8f*pow(sin(0.1f*time), 2) +0.2f;
            const float windDirectionXEnd = 0.5f;
            float windDirectionX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX/pow(windDirectionXEnd, 2));
            windDirectionX = sqrt(windDirectionX);
            const float windDirectionYEnd = 0.5f;
            float windDirectionY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX/pow(windDirectionYEnd, 2));
            windDirectionY = sqrt(windDirectionY);
            windDirectionTarget = glm::vec2(windDirectionX, windDirectionY);
            lastWindUpdate = time;
        }
    
        const float windApproachSpeed= 0.0005f;

        if (windApproachSpeed*static_cast<float>(time)>1.0f) {
            wind = glm::normalize(windDirection)*windTarget;
            windDirection = windDirectionTarget;
        }
        else {
            windDirection.x = windDirection.x + windApproachSpeed*static_cast<float>(time)*windDirectionTarget.x - windDirection.x;
            windDirection.y = windDirection.y + windApproachSpeed*static_cast<float>(time)*windDirectionTarget.y - windDirection.x;
            wind = wind + (windApproachSpeed*static_cast<float>(time)) * (glm::normalize(windDirection)*windTarget - wind);
        }

        //set view and projection matrix
        glm::mat4 lightingViewProjectionMatrix = glm::perspective(1.571f, (float)windowSize.x/(float)windowSize.y, 0.1f, farPlane) * buildViewMatrix(level);

        //render skydome
        skydomeShader->use();
        // set fog Parameters
        skydomeShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());
        skydomeShader->setUniform("directionalVector", level->getDirectionalLight()->getPosition());
        level->getSkydome()->render(skydomeShader, false, &lightingViewProjectionMatrix);
        
        lightingShader->use();
        
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

        // set fog Parameters
        lightingShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());
        if(level->getDirectionalLight()) {
            lightingShader->setUniform("directionalLightVector",
                level->getDirectionalLight()->getPosition());
        }
        
        // set Material Parameters
        lightingShader->setUniform("camera", level->getPhysics()->getCameraPosition());
        textureMovementPosition += wind/5.0f;
        lightingShader->setUniform("movingTextureOffset", textureMovementPosition);
        lightingShader->setUniform("movement", wind);

        lightingShader->setUniform("time", (float) time);

        if (renderWorld) {
            // render the level
            level->enqueueObjects(this);
            for (unsigned int i = 0; i<Material::getAllTextures()->size(); i++) {
                bool parametersSet = false;
                for(unsigned int j = 0; j<renderQueue.size(); j++) {
                    if(renderQueue.at(j)->at(i).size() != 0) {
                        if (!parametersSet) {
                            parametersSet = true;
                            Material* material = renderQueue.at(j)->at(i).at(0)->getMaterial();
                            if (material->isMoving()) {
                                lightingShader->setUniform("movingTexture", true);
                            }
                            else {
                                lightingShader->setUniform("movingTexture", false);
                            }
                            lightingShader->setUniform("uTexture", material->getTextureUnit());
                            lightingShader->setUniform("ambientFactor", material->getAmbientFactor());
                            lightingShader->setUniform("diffuseFactor", material->getDiffuseFactor());
                            lightingShader->setUniform("specularFactor", material->getSpecularFactor());
                            lightingShader->setUniform("shininess", material->getShininess());
                        }
                        for(unsigned int k = 0; k<renderQueue.at(j)->at(i).size(); k++) {
                        renderQueue.at(j)->at(i).at(k)->render(lightingShader, true, &lightingViewProjectionMatrix, &depthBiasVPs);
                        }
                    }
                }
            }
            // render water plane last for correct transparency
            if (level->getWaterPlane()) {
                Material* material = level->getWaterPlane()->getMaterial();
                if (material->isMoving()) {
                    lightingShader->setUniform("movingTexture", true);
                }
                else {
                    lightingShader->setUniform("movingTexture", false);
                }
                lightingShader->setUniform("uTexture", material->getTextureUnit());
                lightingShader->setUniform("ambientFactor", material->getAmbientFactor());
                lightingShader->setUniform("diffuseFactor", material->getDiffuseFactor());
                lightingShader->setUniform("specularFactor", material->getSpecularFactor());
                lightingShader->setUniform("shininess", material->getShininess());
                level->getWaterPlane()->render(lightingShader, true, &lightingViewProjectionMatrix, &depthBiasVPs);
            }
            renderQueue.clear();
        }

        if (renderDebug) {
            debugDrawer.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
            level->getPhysics()->getWorld()->debugDrawWorld();
            debugDrawer.setDebugMode(btIDebugDraw::DBG_NoDebug);
            unsigned int data_count = debugDrawer.getData()->size();
            float* debugData = new float[data_count];
            for (unsigned int i = 0; i<data_count; i++) {
                debugData[i] = debugDrawer.getData()->at(i);
            }
            debug_ab->setDataElements(data_count/6, debugData);
            debugDrawer.clearData();
            delete[] debugData;
            debugShader->use();
            debugShader->setUniform("viewProjectionMatrix", lightingViewProjectionMatrix);
            debug_vao->render();
        }

        // draw flames on top
        if (renderFlames) {
            flameShader->use();
            // cull faces to get consistent color while using alpha
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            // draw with colors
            for(unsigned int i = 0; i<closestFlames.size(); i++) {
                closestFlames.at(i)->render(flameShader, lightingViewProjectionMatrix, float(time), true, wind);
            }
            glDisable(GL_CULL_FACE);

            framebuffer_light->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer_light->getObjectName());
            glBlitFramebuffer(0, 0, windowSize.x, windowSize.y, 0, 0, windowSize.x, windowSize.y,
                    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            // draw slightly larger only for stencil buffer to blur edges
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_ALWAYS, 1, 0xFF); //Set any stencil to 1
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            glStencilMask(0xFF);//write to stencil buffer
            glClear(GL_STENCIL_BUFFER_BIT);//clear stencil buffer

            for(unsigned int i = 0; i<closestFlames.size(); i++) {
                closestFlames.at(i)->render(flameShader, lightingViewProjectionMatrix, float(time), false, wind);
            }

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
    }
}

void Graphics::updateLights() {
    std::vector<std::shared_ptr<Light>> oldClosestLights = std::vector<std::shared_ptr<Light>>(*closestLights);
    closestLights = level->getClosestLights(maxShadowSampleCount);
    if (closestLights->size() > 0) {
        lightingShader->use();
        lightingShader->setUniform("lightCount", (int) closestLights->size());
        lightingShader->setUniform("maxShadowRenderCount", min((int)closestLights->size(), maxShadowSampleCount));

        // find new closest lights for the shadow render queue
        unsigned int i = 0;
        std::vector<std::shared_ptr<Light>> compareClosestLights = std::vector<std::shared_ptr<Light>>(*closestLights);
        while(i<oldClosestLights.size()) {
            bool found = false;
            for(unsigned int j = 0; j<compareClosestLights.size(); j++) {
                if (oldClosestLights.at(i) == compareClosestLights.at(j)){
                    found = true;
                    compareClosestLights.erase(compareClosestLights.begin() + j);
                    break;
                }
            }
            if (found) {
                oldClosestLights.erase(oldClosestLights.begin() + i);
            }
            else {
                i++;
            }
        }

        assert(oldClosestLights.size() == compareClosestLights.size());

        // replace old lights with the new ones in the shadow render queue
        for(unsigned int i = 0; i<oldClosestLights.size(); i++) {
            for(unsigned int j = 0; j<shadowRenderQueue.size(); j++) {
                if(oldClosestLights.at(i) == shadowRenderQueue.at(j).light) {
                    shadowRenderQueue.at(j).light = compareClosestLights.at(i);
                    // 15000 is larger priority than any light can get during one tick
                    shadowRenderQueue.at(j).currentPriority = 15000;
                }
            }
        }

        // update priority of the shadow render queue
        for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
            float distance = glm::distance(level->getCameraCenter()->getPosition(), shadowRenderQueue.at(i).light->getPosition());
            shadowRenderQueue.at(i).priority = (int) 100*std::exp(5.0f - 0.1f * distance);
        }

        // Build light position array
        glm::vec3 lightSources[shadowRenderQueue.size()];
        for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
            lightSources[i] = shadowRenderQueue.at(i).light->getPosition();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightSources"),
            sizeof(lightSources),  (GLfloat*) lightSources);
        // Build light colour array
        glm::vec3 lightColours[shadowRenderQueue.size()];
        for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
            lightColours[i] = shadowRenderQueue.at(i).light->getColour();
        }
        glUniform3fv(lightingShader->getUniformLocation("lightColors"),
            sizeof(lightColours),  (GLfloat*) lightColours);
        // Build light attenuation array
        float lightIntensities[shadowRenderQueue.size()];
        for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
            lightIntensities[i] = shadowRenderQueue.at(i).light->getIntensity();
        }
        glUniform1fv(lightingShader->getUniformLocation("lightIntensities"),
            sizeof(lightIntensities),  (GLfloat*) lightIntensities);

        bool isFlame[shadowRenderQueue.size()];
        closestFlames = std::vector<Flame*>();
        for(unsigned int i = 0; i<shadowRenderQueue.size(); i++) {
            if (shadowRenderQueue.at(i).light->isFlame()) {
                closestFlames.push_back(shadowRenderQueue.at(i).light->getFlame());
                isFlame[i] = true;
            }
            else {
                isFlame[i] = false;
            }
        }
        glUniform1iv(lightingShader->getUniformLocation("isFlame"), sizeof(isFlame), (GLint*) isFlame);
    }
}

void Graphics::saveWindowSize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
}

void Graphics::resize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
    if (gameStart) {
        for (unsigned int i = 0; i<depth_directionalMaps.size(); i++) {
            depth_directionalMaps.at(i)->resize(glm::vec2(windowSize.x, windowSize.y));
        }
        light_fbo_color_texture->resize(windowSize);
        light_fbo_depth_texture->resize(windowSize);
        flamePostShader->setUniform("windowSizeX", int(windowSize.x));
        flamePostShader->setUniform("windowSizeY", int(windowSize.y));
        bindTextureUnits();
    }
    else {
        float quadData[24];
        if (loadingScreenWidth/loadingScreenHeight < ((float)windowSize.x)/((float)windowSize.y)) {
            float quadTemp[24] ={
            -(((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)),  1.0f,  0.0f, 1.0f,
             (((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)),  1.0f,  1.0f, 1.0f,
             (((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)), -1.0f,  1.0f, 0.0f,

             (((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)), -1.0f,  1.0f, 0.0f,
            -(((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)), -1.0f,  0.0f, 0.0f,
            -(((float)windowSize.y*loadingScreenWidth)/((float)windowSize.x*loadingScreenHeight)),  1.0f,  0.0f, 1.0f
            };
            for(int i = 0; i<24; i++) {
                quadData[i] = quadTemp[i];
            }
        }
        else {
            float quadTemp[24] = {
                -1.0f,  ((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  0.0f, 1.0f,
                 1.0f,  ((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  1.0f, 1.0f,
                 1.0f, -((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  1.0f, 0.0f,

                 1.0f, -((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  1.0f, 0.0f,
                -1.0f, -((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  0.0f, 0.0f,
                -1.0f,  ((float)windowSize.x*loadingScreenHeight)/((float)windowSize.y*loadingScreenWidth),  0.0f, 1.0f
            };
            for(int i = 0; i<24; i++) {
                quadData[i] = quadTemp[i];
            }
        }

        fullscreen_quad_ab_loading->setDataElements(6, quadData);
        fullscreen_quad_loading = SharedVertexArrayObject(new VertexArrayObject);
        fullscreen_quad_loading->attachAllAttributes(fullscreen_quad_ab_loading);
    }
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

void Graphics::startGame() {
    gameStart = true;
}

void Graphics::setRenderShadows(bool state) {
    if(!state) {
        for(unsigned int i = 0; i<framebuffer_directional.size(); i++) {
            framebuffer_directional.at(i)->bind();
            glClear(GL_DEPTH_BUFFER_BIT);
        }
        for(unsigned int i_pointlight = 0; i_pointlight<depth_cubeMaps.size(); i_pointlight++) {
            for(int i_face = 0; i_face<6; i_face++) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, depth_cubeMaps.at(i_pointlight)->getObjectName(), 0);
                glClear(GL_DEPTH_BUFFER_BIT);
            }
        }
    }
    renderShadows = state;
}

void Graphics::setRenderFlames(bool state) {
    renderFlames = state;
}

bool Graphics::getRenderShadows() {
    return renderShadows;
}

bool Graphics::getRenderFlames() {
    return renderFlames;
}


void Graphics::setRenderDebug(bool state) {
    renderDebug = state;
}

bool Graphics::getRenderDebug() {
    return renderDebug;
}

void Graphics::setRenderWorld(bool state) {
    renderWorld = state;
}

bool Graphics::getRenderWorld() {
    return renderWorld;
}

void Graphics::enqueueObjects(std::vector<std::vector<Object*>>* queue){
    renderQueue.push_back(queue);
}

void Graphics::initShadowRenderQueue() {
    closestLights = level->getClosestLights(maxShadowSampleCount);
    int maxLights = min((int)closestLights->size(), maxShadowSampleCount);
    shadowRenderQueue = std::vector<ShadowRenderQueueSlot>(maxLights);
    glViewport(0, 0, cube_size, cube_size);
    glm::mat4 depthProjectionMatrix_pointlights = glm::perspective(1.571f, (float)cube_size/(float)cube_size, 0.1f,  50.0f);
    glm::vec3 looking_directions[6] = {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)};
    glm::vec3 upvectors[6] = {glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, -1.0f, 0.0f),glm::vec3(0.0f, -1.0f, 0.0f)};

    framebuffer_cube->bind();

    for(unsigned int i = 0; i<shadowRenderQueue.size(); i++){
        shadowRenderQueue.at(i).light = closestLights->at(i);
        shadowRenderQueue.at(i).currentPriority = 0;
        // render depth textures for point lights
        depthCubeShader->use();
        // render each side of the cube
        for (int i_face = 0; i_face<6; i_face++) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i_face, depth_cubeMaps.at(i)->getObjectName(), 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glm::mat4 viewMatrix = glm::lookAt(shadowRenderQueue.at(i).light->getPosition(),
                shadowRenderQueue.at(i).light->getPosition() + looking_directions[i_face], upvectors[i_face]);
            glm::mat4 depthViewProjectionMatrix_face = depthProjectionMatrix_pointlights * viewMatrix;
            std::vector<glm::mat4> viewMatrixVector = std::vector<glm::mat4>();
            viewMatrixVector.push_back(viewMatrix);
            level->render(depthCubeShader, false, shadowRenderQueue.at(i).light->getPosition(), 1, &depthViewProjectionMatrix_face, &viewMatrixVector);
            if (!framebuffer_cube->isFrameBufferObjectComplete()) {
                printf("Framebuffer incomplete, unknown error occured during shadow generation!\n");
            }
        }
    }
}
