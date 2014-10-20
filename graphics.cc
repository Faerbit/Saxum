#include "main.hh"

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Objects.hh>
#include <ACGL/Base/Settings.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

using namespace std;
using namespace ACGL::OpenGL;
using namespace ACGL::Base;
using namespace ACGL::Utils;

SharedVertexArrayObject vaoBunny;
SharedShaderProgram normalsAsColorShader;
SharedTexture2D bunnyTexture;

// gets called after the OpenGL window is prepared:
void initCustomResources()
{
    // define where shaders and textures can be found:
    Settings::the()->setResourcePath("../");
    Settings::the()->setShaderPath("Shader/");
    Settings::the()->setTexturePath("Geometry/");
    Settings::the()->setGeometryPath("Geometry/");

    // load the geometry of the stanford bunny and build a VAO:
    vaoBunny = VertexArrayObjectCreator("Bunny.obj").create();
    vaoBunny->bind();

    // load a texture:
    bunnyTexture = Texture2DFileManager::the()->get( Texture2DCreator("clownfishBunny.png"));
    // alternatively, without the Managers:
    //bunnyTexture = loadTexture2D( "../shared/Geometry/clownfishBunny.png" );

    // look up all shader files starting with 'HelloWorld' and build a ShaderProgram from it:
    normalsAsColorShader = ShaderProgramCreator("HelloWorld").attributeLocations( vaoBunny->getAttributeLocations() ).create();
    normalsAsColorShader->use();

    // set texture uniform and bind texture to the same texture unit:
    normalsAsColorShader->setTexture( "uTexture", bunnyTexture, 0 );

    // just in case: check for errors
    openGLCriticalError();
}

void deleteCustomResources()
{
    // we have memory management via reference counting, so nothing to do here
}

void draw( float runTime )
{
    // reload textures every 100 frames:
    static int frame = 0;
    frame++;
    if (frame % 100 == 0) {
        Texture2DFileManager::the()->updateAll();
    }

    // clear the framebuffer:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view and projection matrix:
    glm::mat4 viewMatrix = glm::translate(glm::vec3(0.0f, -1.0f, -2.0f)) * glm::rotate<float>(1.0472f * runTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale<float>(glm::vec3(0.25f));
    normalsAsColorShader->setUniform( "uViewMatrix", viewMatrix );
    normalsAsColorShader->setUniform( "uProjectionMatrix", buildFrustum(75.0, 0.1, 100.0, (float)g_windowSize.x/(float)g_windowSize.y) );

    // render the bunny:
    vaoBunny->render();
}

void resizeCallback( GLFWwindow *, int newWidth, int newHeight )
{
    // store the new window size and adjust the viewport:
    g_windowSize = glm::uvec2( newWidth, newHeight);
    glViewport( 0, 0, g_windowSize.x, g_windowSize.y );
}
