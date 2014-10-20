/***********************************************************************
 * Copyright 2013 Computer Graphics Group RWTH Aachen University.      *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>

namespace ACGL{
namespace OpenGL{

//! converts a KHR debug source enum to a human readable string
const char *debugSourceName( GLenum _source );

//! converts a KHR debug type enum to a human readable string
const char *debugTypeName( GLenum _type );

//! converts a KHR debug severity enum to a human readable string
const char *debugSeverityName( GLenum _type );

//! tries to register the default debug callback:
//! applications can register alternative callbacks with glDebugMessageCallback !
void ACGLRegisterDefaultDebugCallback();

//! default debug callback
void APIENTRY ACGL_KHR_default_debug_callback( GLenum _source, GLenum _type, GLuint _id, GLenum _severity, GLsizei _length, const GLchar *_message, void *_userParam);

} // OpenGL
} // ACGL




