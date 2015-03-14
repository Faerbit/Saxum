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

Graphics::Graphics(glm::uvec2 windowSize, float nearPlane, 
    float farPlane, int cube_size,
    unsigned int maxShadowRenderCount,
    std::string screenPath,
    std::string screenContinuePath) {
    this->windowSize = windowSize;
    this->nearPlane = nearPlane;
    this->farPlane = farPlane;
    this->cube_size = cube_size;
    this->maxShadowRenderCount = maxShadowRenderCount;
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

    
    // update lights on creation
    lastLightUpdate = -lightUpdateDelay;

    lastWindUpdate = - windUpdateDelay;
    windTarget = 0.0f;
    wind = glm::vec2(0.0f, 0.0f);
    windDirection = glm::vec2(-1.0f, -1.0f);
    windDirectionTarget = glm::vec2(-1.0f, -1.0f);
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

    bindTextureUnits();

    updateClosestLights();
}

void Graphics::bindTextureUnits(){

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

    loadingShader->use(); 
    loadingShader->setTexture("screen", loadingScreen, 16);
    loadingShader->setTexture("screenContinue", loadingContinueScreen, 17);
}

void Graphics::renderLoadingScreen() {
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &number_of_texture_units);
    printf("Your graphics card supports %d texture units.\n", number_of_texture_units);
    // Exit if we need more texture units
    if (number_of_texture_units < 18) {
        printf("You need at least 18 texture units to run this application. Exiting\n");
        exit(-1);
    }
    loadingScreen = Texture2DFileManager::the()->get(Texture2DCreator(loadingScreenPath));
    loadingScreen->generateMipmaps();
    loadingContinueScreen = Texture2DFileManager::the()->get(Texture2DCreator(loadingScreenContinuePath));
    loadingContinueScreen->generateMipmaps();
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
    loadingShader->setTexture("screen", loadingScreen, 16);
    loadingShader->setTexture("screenContinue", loadingContinueScreen, 17);
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
        loadingShader->use();
        loadingShader->setUniform("time", float(time));
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
           
            
            float sunAngle = glm::dot(glm::vec3(0.0f, 1.0f, 0.0f), glm::normalize(level->getDirectionalLight()->getPosition()));
            glm::vec3 sunVector = (level->getCameraCenter()->getPosition() + level->getDirectionalLight()->getPosition());

            for (unsigned int i = 0; i<framebuffer_directional.size(); i++) {
                framebuffer_directional.at(i)->bind(); 
                glClear(GL_DEPTH_BUFFER_BIT);
                if (sunAngle > -0.6f) {
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
        skydomeShader->setUniform("farPlane", farPlane);
        skydomeShader->setUniform("skydomeSize", level->getSkydomeSize());
        skydomeShader->setUniform("fogColorDay", level->getFogColourDay());
        skydomeShader->setUniform("fogColorRise", level->getFogColourRise());
        skydomeShader->setUniform("fogColorNight", level->getFogColourNight());
        skydomeShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());
        skydomeShader->setUniform("directionalVector", level->getDirectionalLight()->getPosition());
        skydomeShader->setUniform("sunColor", level->getDirectionalLight()->getColour());
        level->getSkydome()->render(skydomeShader, false, true, &lightingViewProjectionMatrix);
        
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

        lightingShader->setUniform("farPlane", farPlane);
        
        // set fog Parameters
        lightingShader->setUniform("fogColorDay", level->getFogColourDay());
        lightingShader->setUniform("fogColorRise", level->getFogColourRise());
        lightingShader->setUniform("fogColorNight", level->getFogColourNight());
        lightingShader->setUniform("cameraCenter", level->getCameraCenter()->getPosition());
        
        // set Material Parameters
        lightingShader->setUniform("ambientColor", level->getAmbientLight());
        lightingShader->setUniform("camera", level->getPhysics()->getCameraPosition());
        textureMovementPosition += wind/5.0f;
        lightingShader->setUniform("movingTextureOffset", textureMovementPosition);
        lightingShader->setUniform("movement", wind);

        lightingShader->setUniform("time", (float) time);

        if (renderWorld) {
            // render the level
            level->render(lightingShader, true, &lightingViewProjectionMatrix, &depthBiasVPs);
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
    if (level->getLights()->size() > 15) {
        closestLights = std::vector<Light>(&closestLights[0],
                &closestLights[15]);
    }
}

void Graphics::updateLights() {
    updateClosestLights();
    if (closestLights.size() > 0) {
        lightingShader->use();
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
    bool isFlame[closestLights.size()];
    closestFlames = std::vector<Flame*>();
    for (unsigned int i = 0; i<closestLights.size(); i++) {
        if (closestLights.at(i).isFlame()) {
            closestFlames.push_back(closestLights.at(i).getFlame());
            isFlame[i] = true;
        }
        else {
            isFlame[i] = false;
        }
    }
    glUniform1iv(lightingShader->getUniformLocation("isFlame"), sizeof(isFlame), (GLint*) isFlame);
}

void Graphics::resize(glm::uvec2 windowSize) {
    this->windowSize = windowSize;
    for (unsigned int i = 0; i<depth_directionalMaps.size(); i++) {
        depth_directionalMaps.at(i)->resize(glm::vec2(windowSize.x, windowSize.y));
    }
    light_fbo_color_texture->resize(windowSize);
    light_fbo_depth_texture->resize(windowSize);
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
