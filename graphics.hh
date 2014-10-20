#ifndef GRAPHICS_HH_INCLUDED
#define GRAPHICS_HH_INCLUDED

#endif

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
