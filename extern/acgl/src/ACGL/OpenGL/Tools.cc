/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Utils/FileHelpers.hh>
#include <ACGL/OpenGL/glloaders/extensions.hh>
#include <ACGL/OpenGL/Debug.hh>
#include <sstream>
using namespace std;

namespace ACGL{
namespace OpenGL{

GLfloat ACGL_MAX_TEXTURE_MAX_ANISOTROPY = -1.0f;
void initRuntimeDependentLimits()
{
    if ( ACGL_EXT_texture_filter_anisotrophic() ) {
        glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ACGL_MAX_TEXTURE_MAX_ANISOTROPY );
    } else {
        ACGL_MAX_TEXTURE_MAX_ANISOTROPY = 0.0f;
    }
}

//
// This is a "private" function that should not be called from outside of this file.
//
// glGetIntegerv(GL_MAJOR_VERSION... and glGetIntegerv(GL_MINOR_VERSION... are great, but
// require OpenGL 3.0 and are not supported on ES 2 :-( so the VERSION string has to get parsed...
//
// OpenGL spec:
// The VERSION ... strings are laid out as follows:
// <version number><space><vendor-specific information>
//
// OpenGL ES spec:
// The VERSION string is laid out as follows:
// "OpenGL ES N.M vendor-specific information"
//
// both specs:
// The version number is either of the form major_number.minor_number or
// major_number.minor_number.release_number, where the numbers all have one or more digits.
//
uint32_t privateGetOpenGLVersion( int _type )
{
    static uint32_t OGLminor   = 0;
    static uint32_t OGLmajor   = 0;
    static uint32_t OGLversion = 0;

    if (OGLversion == 0) {
        // calculate the version numbers once:
        // NOTE: similar to GLEW we assume here, that the minor and major numbers
        //       only have one digit. We also ignore release numbers. This will fail e.g. for OpenGL 10.0
        const GLubyte* versionString;
        versionString = glGetString(GL_VERSION);

        int positionOfFirstDot = 0;
        while ((versionString[positionOfFirstDot] != '\0') && (versionString[positionOfFirstDot] != '.')) ++positionOfFirstDot;

        OGLmajor = versionString[positionOfFirstDot-1] - '0';
        OGLminor = versionString[positionOfFirstDot+1] - '0';

        if (OGLmajor > 9) OGLmajor = 0;
        if (OGLminor > 9) OGLminor = 0;

        OGLversion = OGLmajor*10 + OGLminor;
    }
    switch (_type) {
        case 0: return OGLminor;
        case 1: return OGLmajor;
        default: return OGLversion;
    };
}

uint32_t getOpenGLMinorVersionNumber()
{
    return privateGetOpenGLVersion( 0 );
}

uint32_t getOpenGLMajorVersionNumber()
{
    return privateGetOpenGLVersion( 1 );
}

uint32_t getOpenGLVersionNumber()
{
    return privateGetOpenGLVersion( 2 );
}

bool doesSupportGeometryShader()
{
#ifdef ACGL_OPENGL_ES
    return false;
#else
    return (ACGL_EXT_geometry_shader4() || ACGL_ARB_geometry_shader4() || (getOpenGLVersionNumber() >= 32));
#endif
}

bool doesSupportTessellationShader()
{
#ifdef ACGL_OPENGL_ES
    return false;
#else
    return ( ACGL_ARB_tessellation_shader() || (getOpenGLVersionNumber() >= 40));
#endif
}

bool doesSupportComputeShader()
{
#ifdef ACGL_OPENGL_ES
    return false;
#else
    return ( ACGL_ARB_compute_shader() || (getOpenGLVersionNumber() >= 43));
#endif
}


GLenum getShaderTypeByFileEnding( const std::string _fileName, bool _ignoreUnsupportedShaderTypes )
{
    std::string fileEnding = ACGL::Utils::StringHelpers::getFileEnding( _fileName );
    if( fileEnding.size() == 0 ) return GL_INVALID_ENUM;

    GLenum foundType = GL_INVALID_ENUM;

    // guess the shader type:
    for (unsigned int ending = 0; ending < sizeof(sShaderEndings) / sizeof(ShaderEndings); ++ending)
    {
        if ( fileEnding == sShaderEndings[ending].ending )
        {
            foundType = sShaderEndings[ending].type;
            break;
        }
    }

    if (_ignoreUnsupportedShaderTypes) {
        if (foundType == GL_GEOMETRY_SHADER        && !doesSupportGeometryShader()    ) return GL_INVALID_ENUM;
        if (foundType == GL_TESS_CONTROL_SHADER    && !doesSupportTessellationShader()) return GL_INVALID_ENUM;
        if (foundType == GL_TESS_EVALUATION_SHADER && !doesSupportTessellationShader()) return GL_INVALID_ENUM;
        if (foundType == GL_COMPUTE_SHADER         && !doesSupportComputeShader()     ) return GL_INVALID_ENUM;
    }

    return foundType;
}

const GLubyte* acglShaderTypeString( GLenum _shaderType )
{
    if      (_shaderType == GL_VERTEX_SHADER)          { return (GLubyte*) "vertex shader"; }
    else if (_shaderType == GL_TESS_CONTROL_SHADER)    { return (GLubyte*) "tessellation control shader"; }
    else if (_shaderType == GL_TESS_EVALUATION_SHADER) { return (GLubyte*) "tessellation evaluation shader"; }
    else if (_shaderType == GL_GEOMETRY_SHADER)        { return (GLubyte*) "geometry shader"; }
    else if (_shaderType == GL_FRAGMENT_SHADER)        { return (GLubyte*) "fragment shader"; }
    else if (_shaderType == GL_COMPUTE_SHADER)         { return (GLubyte*) "compute shader"; }
    else {
        return (GLubyte*) "unknown shader type";
    }
}

const GLubyte* acglErrorString( GLenum _errorCode )
{
    // no gluErrorString on iOS, problems on visual studio...
    // Only 3.2+ Core and ES 2.0+ errors belong here:
    if      (_errorCode == GL_INVALID_ENUM)                  { return (GLubyte*) "GL_INVALID_ENUM"; }
    else if (_errorCode == GL_INVALID_VALUE)                 { return (GLubyte*) "GL_INVALID_VALUE"; }
    else if (_errorCode == GL_INVALID_OPERATION)             { return (GLubyte*) "GL_INVALID_OPERATION"; }
    else if (_errorCode == GL_INVALID_FRAMEBUFFER_OPERATION) { return (GLubyte*) "GL_INVALID_FRAMEBUFFER_OPERATION"; }
    else if (_errorCode == GL_OUT_OF_MEMORY)                 { return (GLubyte*) "GL_OUT_OF_MEMORY"; }
    else if (_errorCode == GL_NO_ERROR)                      { return (GLubyte*) "GL_NO_ERROR"; }
    else {
        return (GLubyte*) "unknown error";
    }
}



GLenum openGLError_( const char *_fileName, const unsigned long _lineNumber )
{
    GLenum currentError = glGetError();
    GLenum lastError    = currentError;

    // OpenGL does not forbit the implementation to stack up more than one error code
    // so we have to check those in a loop:
    while ( currentError != GL_NO_ERROR ) {
        ACGL::Utils::error() << "GL error in file " << _fileName << ":" << _lineNumber << " - " << acglErrorString( currentError ) << std::endl;

        //stringstream sstream (stringstream::in | stringstream::out);
        //sstream << acglErrorString( currentError ) << " in file " << _fileName << ":" << _lineNumber;
        //acglDebugMessageInsert( GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, currentError, GL_DEBUG_SEVERITY_HIGH, -1, sstream.str().c_str() );

        lastError    = currentError;
        currentError = glGetError();
    }

    return lastError; // returns the last real error (in case there was at least one!)
}


} // OpenGL
} // ACGL

