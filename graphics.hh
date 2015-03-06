#ifndef GRAPHICS_HH_INCLUDED
#define GRAPHICS_HH_INCLUDED
#include <ACGL/OpenGL/GL.hh>

#include <GLFW/glfw3.h>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>

#include "level.hh"

using namespace ACGL::OpenGL;

class Graphics {
    public:
        Graphics(glm::uvec2 windowSize, float nearPlane, float farPlane, int cube_size, unsigned int maxShadowRenderCount);
        Graphics();
        void init(Level* level);
        void render(double time);
        glm::mat4 buildViewMatrix(Level* level);
        glm::uvec2 getWindowSize();
        void resize(glm::uvec2 windowSize);
        float getFarPlane();
    private:
        void updateLights();
        void updateClosestLights();
        bool compareLightDistances(Light a, Light b);
        void saveDepthBufferToDisk(int face, std::string);
        double lastUpdate;
        glm::uvec2 windowSize;
        float nearPlane;
        float farPlane;
        std::vector<Light> closestLights;
        SharedShaderProgram lightingShader;
        SharedShaderProgram skydomeShader;
        SharedShaderProgram depthCubeShader;
        SharedShaderProgram depthShader;
        SharedShaderProgram flameShader;
        SharedShaderProgram flameColorShader;
        SharedShaderProgram flamePostShader;
        SharedShaderProgram mergeShader;
        std::vector<SharedTexture2D> depth_directionalMaps;
        std::vector<SharedFrameBufferObject> framebuffer_directional;
        std::vector<SharedTextureCubeMap> depth_cubeMaps;
        SharedFrameBufferObject framebuffer_cube;
        SharedFrameBufferObject framebuffer_light;
        SharedFrameBufferObject framebuffer_flame;
        SharedTexture2D flame_fbo_color_texture;
        SharedTexture2D light_fbo_color_texture;
        SharedTexture2D light_fbo_depth_texture;
        SharedVertexArrayObject flame_positions;
        SharedArrayBuffer flame_positions_ab;
        SharedVertexArrayObject fullscreen_quad;
        SharedArrayBuffer fullscreen_quad_ab;
        int cube_size;
        unsigned int maxShadowRenderCount;
        Level* level;
        int number_of_texture_units = 0;
};

#endif
