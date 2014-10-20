#ifndef GRAPHICS_HH_INCLUDED
#define GRAPHICS_HH_INCLUDED

#include "main.hh"

#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>
#include <ACGL/OpenGL/Creator/Texture2DCreator.hh>
#include <ACGL/OpenGL/Objects.hh>
#include <ACGL/Base/Settings.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Data/TextureLoadStore.hh>
#include <ACGL/OpenGL/Managers.hh>

// gets called after the OpenGL window is prepared, init of example specific stuff:
void initCustomResources();

// gets called at application shutdown:
void deleteCustomResources();

// gets called ech frame, runTime is in seconds:
void draw( float runTime );

// gets called at window resize:
void resizeCallback( GLFWwindow *, int newWidth, int newHeight );

// to build the projection matrix:
glm::mat4 buildFrustum( float phiInDegree, float near, float far, float aspectRatio);

#endif
