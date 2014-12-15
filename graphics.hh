#ifndef GRAPHICS_HH_INCLUDED
#define GRAPHICS_HH_INCLUDED
#include <ACGL/OpenGL/GL.hh>

#include <GLFW/glfw3.h>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>

#include "level.hh"

class Graphics {
    public:
        Graphics(glm::uvec2 windowSize, float nearPlane, float farPlane);
        Graphics();
        void init(Level* level);
        void render();
        glm::mat4 buildViewMatrix(Level* level);
        glm::uvec2 getWindowSize();
        bool createWindow();
        GLFWwindow* getWindow();
        void resize(glm::uvec2 windowSize);
        float getFarPlane();
    private:
        void setGLFWHintsForOpenGLVersion( unsigned int _version );
        glm::uvec2 windowSize;
        float nearPlane;
        float farPlane;
        GLFWwindow* window;
        ACGL::OpenGL::SharedShaderProgram lightingShader;
        ACGL::OpenGL::SharedShaderProgram depthShader;
        ACGL::OpenGL::SharedTexture2D depthTexture_near;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer_near;
        ACGL::OpenGL::SharedTexture2D depthTexture_middle;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer_middle;
        ACGL::OpenGL::SharedTexture2D depthTexture_far;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer_far;
        std::vector<ACGL::OpenGL::SharedTextureCubeMap> depth_cubeMaps;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer_cube;
        static const int cube_size;
        Level* level;
};

#endif
