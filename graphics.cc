#include "graphics.hh"

#include "model.hh"
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

using namespace std;

ACGL::OpenGL::SharedShaderProgram shader;
Level level;

// gets called after the OpenGL window is prepared:
void initCustomResources()
{
    // define where shaders and textures can be found:
    ACGL::Base::Settings::the()->setResourcePath("../");
    ACGL::Base::Settings::the()->setShaderPath("Shader/");
    ACGL::Base::Settings::the()->setTexturePath("Geometry/");
    ACGL::Base::Settings::the()->setGeometryPath("Geometry/");

    // load Model to give shader correct Attribute locations
    // TODO look up if this is really necessary, since this looks really stupid.
    Model model = Model("Bunny.obj");

    // look up all shader files starting with 'phong' and build a ShaderProgram from it:
    shader = ACGL::OpenGL::ShaderProgramCreator("phong").attributeLocations(
            model.getReference()->getAttributeLocations()).create();
    shader->use();

    // load Level
    level.load(shader);

    // just in case: check for errors
    openGLCriticalError();
}

void deleteCustomResources()
{
    // we have memory management via reference counting, so nothing to do here
}

void draw(float runTime)
{
    // update Level first TODO: move this with the rest of the stuff that doesn't belong here to main
    level.update(runTime);

    // clear the framebuffer:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set view and projection matrix:
    glm::mat4 viewMatrix = glm::translate(glm::vec3(0.0f, -1.0f, -2.0f)) * glm::rotate<float>(1.0472f * runTime, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale<float>(glm::vec3(0.25f));
    shader->setUniform( "uViewMatrix", viewMatrix );
    shader->setUniform( "uProjectionMatrix", buildFrustum(75.0, 0.1, 100.0, (float)g_windowSize.x/(float)g_windowSize.y) );

    //set lighting parameters
    if (level.getLights().size() > 0) {
        shader->setUniform("lightCount", (int) level.getLights().size());

        // TODO look into doing this less often
        // Build light position array
        glm::vec3 lightSources[level.getLights().size()];
        for(unsigned int i = 0; i<level.getLights().size(); i++) {
            lightSources[i] = level.getLights()[i].getPosition();
        }
        glUniform3fv(shader->getUniformLocation("lightSources"),
            sizeof(lightSources),  (GLfloat*) lightSources);
        // Build light colour array
        glm::vec3 lightColours[level.getLights().size()];
        for(unsigned int i = 0; i<level.getLights().size(); i++) {
            lightColours[i] = level.getLights()[i].getColour();
        }
        glUniform3fv(shader->getUniformLocation("lightColors"),
            sizeof(lightColours),  (GLfloat*) lightColours);
        // Build light attenuation array
        float lightIntensities[level.getLights().size()];
        for(unsigned int i = 0; i<level.getLights().size(); i++) {
            lightIntensities[i] = level.getLights()[i].getIntensity();
        }
        glUniform1fv(shader->getUniformLocation("lightIntensities"),
            sizeof(lightIntensities),  (GLfloat*) lightIntensities);
    }

    // set Material Parameters
    shader->setUniform("ambientColor", level.getAmbientLight());
    shader->setUniform("camera", glm::vec3(0.0f, 0.0f, 0.0f));

    // render the level(currently only a bunny):
    level.render();
}

void resizeCallback( GLFWwindow *, int newWidth, int newHeight )
{
    // store the new window size and adjust the viewport:
    g_windowSize = glm::uvec2( newWidth, newHeight);
    glViewport( 0, 0, g_windowSize.x, g_windowSize.y );
}

glm::mat4 buildFrustum( float phiInDegree, float _near, float _far, float aspectRatio) {

    float phiHalfInRadians = 0.5*phiInDegree * (M_PI/180.0);
    float top = _near * tan( phiHalfInRadians );
    float bottom = -top;
    float left  = bottom * aspectRatio;
    float right = -left;

    return glm::frustum(left, right, bottom, top, _near, _far);
}
