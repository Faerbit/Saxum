/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_OPENGL_TOOLS_HH
#define ACGL_OPENGL_TOOLS_HH

/*
 * Some OpenGL related helper functions.
 * All the error checking functions are in here.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Debug.hh>

namespace ACGL{
namespace OpenGL{

//! queries some limits of the runtime which are used in ACGL internally. Client apps should not rely on these values, they are subject to changes. Gets called by ACGL::init()
void initRuntimeDependentLimits();
extern GLfloat ACGL_MAX_TEXTURE_MAX_ANISOTROPY;

//! returns the size in bytes of the common gl types named by there GLenums.
inline GLint getGLTypeSize ( GLenum _type )
{
    switch(_type)
    {
        case GL_BYTE:           return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:  return sizeof(GLubyte);
        case GL_SHORT:          return sizeof(GLshort);
        case GL_UNSIGNED_SHORT: return sizeof(GLushort);
        case GL_INT:            return sizeof(GLint);
        case GL_UNSIGNED_INT:   return sizeof(GLuint);
        case GL_FLOAT:          return sizeof(GLfloat);
#ifndef ACGL_OPENGL_ES
        case GL_DOUBLE:         return sizeof(GLdouble);
#endif

        //case GL_INT64_NV:       return sizeof(GLint64);
        //case GL_UNSIGNED_INT64_NV: return sizeof(GLuint64);
        //case GL_HALF_FLOAT:     return sizeof(GLhalf);
        case GL_FIXED:          return 4; // fixed are packed in a 32bit int (see 2.1.2 of the spec.)
    }
    return 0;
}

inline GLuint getNumberOfChannels( GLenum _format )
{
    // GLES formats:
    if (_format == GL_ALPHA || _format == GL_DEPTH_COMPONENT) return 1;
    if (_format == GL_RGB)  return 3;
    if (_format == GL_RGBA) return 4;
#ifdef ACGL_OPENGL_CORE_PROFILE
    // removed from core:
    if (_format == GL_LUMINANCE)  return 1;
    if (_format == GL_LUMINANCE_ALPHA)  return 2;
#endif

#ifndef ACGL_OPENGL_ES
    // additional desktop formats:
    if ( _format == GL_R8I  || _format == GL_R16I  || _format == GL_R32I
                        || _format == GL_R8UI || _format == GL_R16UI || _format == GL_R32UI
                                              || _format == GL_R16F  || _format == GL_R32F )   return 1;
    if (_format == GL_RED_INTEGER || _format == GL_GREEN_INTEGER || _format == GL_BLUE_INTEGER ) return 1;

    if (_format == GL_RG || _format == GL_RG8I  || _format == GL_RG16I  || _format == GL_RG32I
                         || _format == GL_RG8UI || _format == GL_RG16UI || _format == GL_RG32UI
                                                || _format == GL_RG16F  || _format == GL_RG32F )   return 2;
    if (_format == GL_RG_INTEGER ) return 2;

    if (_format == GL_RGB8I  || _format == GL_RGB16I  || _format == GL_RGB32I
                          || _format == GL_RGB8UI || _format == GL_RGB16UI || _format == GL_RGB32UI
                                                  || _format == GL_RGB16F  || _format == GL_RGB32F )   return 3;
    if (_format == GL_BGR  || _format == GL_RGB_INTEGER  || _format == GL_BGR_INTEGER)  return 3;

    if ( _format == GL_RGBA8I  || _format == GL_RGBA16I  || _format == GL_RGBA32I
                           || _format == GL_RGBA8UI || _format == GL_RGBA16UI || _format == GL_RGBA32UI
                                                    || _format == GL_RGBA16F  || _format == GL_RGBA32F )   return 4;
    if (_format == GL_BGRA || _format == GL_RGBA_INTEGER || _format == GL_BGRA_INTEGER) return 4;
#endif
    
    return 1; // unknown number of channels, assume 1
}

uint32_t getOpenGLMinorVersionNumber();
uint32_t getOpenGLMajorVersionNumber();

// returns the combined version number as 10*major + minor for easy comparing
uint32_t getOpenGLVersionNumber();

// query support for specific shader stages:
bool doesSupportGeometryShader();
bool doesSupportTessellationShader();
bool doesSupportComputeShader();


// Define the shader kinds for older and embedded GL versions so the file extension detection
// can work correctly.
#ifndef GL_GEOMETRY_SHADER
#define GL_GEOMETRY_SHADER 0x8DD9
#endif
#ifndef GL_TESS_CONTROL_SHADER
#define GL_TESS_EVALUATION_SHADER 0x8E87
#define GL_TESS_CONTROL_SHADER 0x8E88
#endif
#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0x91B9
#endif

struct ShaderEndings
{
    const char *ending;
    GLenum      type;
};
const ShaderEndings sShaderEndings[  ] = {
    {"vsh",  GL_VERTEX_SHADER},
    {"vert", GL_VERTEX_SHADER},
    {"tcsh", GL_TESS_CONTROL_SHADER},
    {"tcs",  GL_TESS_CONTROL_SHADER},
    {"tesh", GL_TESS_EVALUATION_SHADER},
    {"tes",  GL_TESS_EVALUATION_SHADER},
    {"gsh",  GL_GEOMETRY_SHADER},
    {"geo",  GL_GEOMETRY_SHADER},
    {"fsh",  GL_FRAGMENT_SHADER},
    {"frag", GL_FRAGMENT_SHADER},
    {"csh",  GL_COMPUTE_SHADER},
    {"cs",   GL_COMPUTE_SHADER}
};

// returns the GL_TESS_CONTROL_SHADER GL_TESS_EVALUATION_SHADER GL_GEOMETRY_SHADER GL_COMPUTE_SHADER
// GL_VERTEX_SHADER GL_FRAGMENT_SHADER or GL_INVALID_VALUE in case the shadertype was not detected
// if _ignoreUnsupportedShaderTypes is true, types unsupported by the current runtime will return
// an GL_INVALID_ENUM also.
GLenum getShaderTypeByFileEnding( const std::string _fileName, bool _ignoreUnsupportedShaderTypes = true );

// looks up the enum and gives a human readable version
const GLubyte* acglShaderTypeString( GLenum _shaderType );

// for every OpenGL error enum this will return a human readable version of it
// similar to gluErrorString, but that function is not available on all plattforms
// (read: iOS)
const GLubyte* acglErrorString( GLenum _errorCode );

/*
 * This function can be used outside of the ACGL framework to check always(!) for
 * OpenGL errors. It will print the errors and return the error code of the last one.
 * Each OpenGL command can only throw one error, errors can only stack up if this
 * function or glGetError was not called often enough (and some OpenGL implementations
 * will forget old errors if new ones occur).
 */
#define openGLError() openGLError_( __FILE__, __LINE__ )

/*
 * This function is used internally in ACGL - but not directly. It gets called from all
 * other rare/common/critical error checks. The __FILE__ __LINE macros have to be used in
 * those to get the correct values from the caller file, if we would use the macro
 * above we could get the file/line from this file, which isn't helping.
 */
GLenum openGLError_( const char *_fileName, const unsigned long _lineNumber );

/*
 * NOTE: Explicit error checks are not needed anymore on desktop systems! Use KHR_debug
 *       callback instead (ACGL registers a default callback automatically).
 *
 *
 *
 * Inside of ACGL we distinguish between rare, common and critical errors. Each kind can be
 * switched off which turns the function into nothing after compiler optimization. If an
 * error check is turned off it will always behave as if there was no error, even if there
 * is one...
 *
 * Per default a debug build enables critical and common errors, a release build only critical
 * ones.
 *
 * We have two functions for each kind:
 * openGL____Error()        will print error messages and return the last error (or GL_NO_ERROR)
 * openGL____ErrorOccured() will print error messages and retrun true if there was an error
 *
 * The definition of rare/common/critical is a bit fuzzy:
 *
 * critical: Errors which can occur even in a bug-free app, like out-of-memory errors.
 *           These checks are rare, stuff like glCreate calls get checked here.
 *           Tests for critical errors should not impact the performance of the app measurably.
 *
 * common:   
 *           Errors which are quite common while developing. Misuse of the library etc.
 *           Setting uniforms which don't exist or shader compile errors are candidates for
 *           common errors. Places where OpenGL resources are calles by a string etc.
 *
 * rare:     Errors which are so uncommon that we don't even check those ina normal debug build.
 *           Switching these on can impact performance as there can be alot of them. If theres a
 *           OpenGL error somewhere in the code, switch these checks on to find the correct spot.
 *
 * OpenGL error checks that are more complicated than just a glGetError call will be wrapped in the
 * same defines, this can be used in application code as well.
 *
 */

inline GLenum openGLErrorDummy()        { return GL_NO_ERROR; }
inline bool   openGLErrorOccuredDummy() { return false; }

#define openGLCheckError()    ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
#define openGLErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)

#ifdef ACGL_CHECK_CRITICAL_GL_ERRORS
# define openGLCriticalError()         ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
# define openGLCriticalErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)
#else
# define openGLCriticalError()         ACGL::OpenGL::openGLErrorDummy()
# define openGLCriticalErrorOccured()  ACGL::OpenGL::openGLErrorOccuredDummy()
#endif

#ifdef ACGL_CHECK_COMMON_GL_ERRORS
# define openGLCommonError()         ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
# define openGLCommonErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)
#else
# define openGLCommonError()         ACGL::OpenGL::openGLErrorDummy()
# define openGLCommonErrorOccured()  ACGL::OpenGL::openGLErrorOccuredDummy()
#endif

#ifdef ACGL_CHECK_RARE_GL_ERRORS
# define openGLRareError()         ACGL::OpenGL::openGLError_( __FILE__, __LINE__ )
# define openGLRareErrorOccured() (ACGL::OpenGL::openGLError_( __FILE__, __LINE__ ) != GL_NO_ERROR)
#else
# define openGLRareError()         ACGL::OpenGL::openGLErrorDummy()
# define openGLRareErrorOccured()  ACGL::OpenGL::openGLErrorOccuredDummy()
#endif



} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_TOOLS_HH

