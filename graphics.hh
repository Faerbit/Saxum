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
        void updateShaderLights();
        void updateClosestLights();
        bool compareLightDistances(Light a, Light b);
        void saveDepthBufferToDisk(int face, std::string);
        double lastUpdate;
        glm::uvec2 windowSize;
        float nearPlane;
        float farPlane;
        // pointer to either use the vector from the level or from here
        std::vector<Light>* closestLights;
        std::vector<Light> closestLightsVector; // contains the 32 closest lights
        ACGL::OpenGL::SharedShaderProgram lightingShader;
        ACGL::OpenGL::SharedShaderProgram depthCubeShader;
        ACGL::OpenGL::SharedShaderProgram depthShader;
        ACGL::OpenGL::SharedTexture2D depthTexture;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer;
        std::vector<ACGL::OpenGL::SharedTextureCubeMap> depth_cubeMaps;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer_cube;
        int cube_size;
        unsigned int maxShadowRenderCount;
        Level* level;
        int number_of_texture_units = 0;
};

#endif
