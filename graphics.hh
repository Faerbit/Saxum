#ifndef GRAPHICS_HH_INCLUDED
#define GRAPHICS_HH_INCLUDED
#include <ACGL/OpenGL/GL.hh>

#include <GLFW/glfw3.h>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>

#include "level.hh"

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
        ACGL::OpenGL::SharedShaderProgram lightingShader;
        ACGL::OpenGL::SharedShaderProgram depthCubeShader;
        ACGL::OpenGL::SharedShaderProgram depthShader;
        ACGL::OpenGL::SharedShaderProgram flameShader;
        std::vector<ACGL::OpenGL::SharedTexture2D> depth_directionalMaps;
        std::vector<ACGL::OpenGL::SharedFrameBufferObject> framebuffer_directional;
        std::vector<ACGL::OpenGL::SharedTextureCubeMap> depth_cubeMaps;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer_cube;
        ACGL::OpenGL::SharedVertexArrayObject flame_positions;
        ACGL::OpenGL::SharedArrayBuffer flame_positions_ab;
        int cube_size;
        unsigned int maxShadowRenderCount;
        Level* level;
        int number_of_texture_units = 0;
};

#endif
