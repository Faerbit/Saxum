#ifndef GRAPHICS_HH_INCLUDED
#define GRAPHICS_HH_INCLUDED
#include <ACGL/OpenGL/GL.hh>

#include <GLFW/glfw3.h>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>

#include "level.hh"
#include "debugDraw.hh"

using namespace ACGL::OpenGL;

class Graphics {
    public:
        Graphics(glm::uvec2 windowSize, float nearPlane, float farPlane, int cube_size, 
                unsigned int maxShadowRenderCount, std::string screenPath,
                std::string screenContinuePath);
        Graphics();
        void init(Level* level);
        void render(double time);
        glm::mat4 buildViewMatrix(Level* level);
        glm::uvec2 getWindowSize();
        void resize(glm::uvec2 windowSize);
        float getFarPlane();
        void startGame();
        void renderLoadingScreen();
        void setRenderShadows(bool state);
        void setRenderFlames(bool state);
        void setRenderDebug(bool state);
        void setRenderWorld(bool state);
        bool getRenderShadows();
        bool getRenderFlames();
        bool getRenderDebug();
        bool getRenderWorld();
        void enqueueObjects(std::vector<std::vector<Object*>>* queue);
    private:
        void bindTextureUnits();
        void updateLights();
        void updateClosestLights();
        bool compareLightDistances(Light a, Light b);
        void saveDepthBufferToDisk(int face, std::string);
        double lastLightUpdate;
        double lastWindUpdate;
        float windTarget;
        glm::vec2 wind;
        glm::vec2 windDirection;
        glm::vec2 windDirectionTarget;
        glm::vec2 textureMovementPosition;
        glm::uvec2 windowSize;
        float nearPlane;
        float farPlane;
        std::string loadingScreenPath;
        std::string loadingScreenContinuePath;
        SharedTexture2D loadingScreen;
        SharedTexture2D loadingContinueScreen;
        std::vector<Light> closestLights;
        std::vector<Flame*> closestFlames;
        SharedShaderProgram loadingShader;
        SharedShaderProgram lightingShader;
        SharedShaderProgram skydomeShader;
        SharedShaderProgram depthCubeShader;
        SharedShaderProgram depthShader;
        SharedShaderProgram flameShader;
        SharedShaderProgram flamePostShader;
        std::vector<SharedTexture2D> depth_directionalMaps;
        std::vector<SharedFrameBufferObject> framebuffer_directional;
        std::vector<SharedTextureCubeMap> depth_cubeMaps;
        SharedFrameBufferObject framebuffer_cube;
        SharedFrameBufferObject framebuffer_light;
        SharedTexture2D light_fbo_color_texture;
        SharedTexture2D light_fbo_depth_texture;
        SharedVertexArrayObject fullscreen_quad;
        SharedArrayBuffer fullscreen_quad_ab;
        SharedVertexArrayObject fullscreen_quad_loading;
        SharedArrayBuffer fullscreen_quad_ab_loading;
        int cube_size;
        unsigned int maxShadowRenderCount;
        Level* level;
        int number_of_texture_units = 0;
        bool gameStart;
        float loadingScreenWidth;
        float loadingScreenHeight;
        bool renderShadows;
        bool renderFlames;
        bool renderDebug;
        bool renderWorld;
        DebugDraw debugDrawer;
        SharedArrayBuffer debug_ab;
        SharedVertexArrayObject debug_vao;
        SharedShaderProgram debugShader;
        std::vector<std::vector<std::vector<Object*>>*> renderQueue;
};

#endif
