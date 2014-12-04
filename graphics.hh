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
        void init();
        void render(Level* level);
        // to build the projection matrix:
        glm::mat4 buildFrustum( float phiInDegree, float near, float far, float aspectRatio);
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
        ACGL::OpenGL::SharedTexture2D depthTexture_depth;
        ACGL::OpenGL::SharedTexture2D depthTexture_colour;
        ACGL::OpenGL::SharedFrameBufferObject framebuffer;
};

#endif
