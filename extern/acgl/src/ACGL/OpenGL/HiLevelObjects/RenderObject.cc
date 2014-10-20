/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef deprecated

#include <ACGL/OpenGL/HiLevelObjects/RenderObject.hh>
#if (ACGL_OPENGL_VERSION >= 30)

#include <ACGL/OpenGL/Tools.hh>
#include <ACGL/Utils/StringHelpers.hh>
#include <ACGL/OpenGL/Data/LocationMappings.hh>

#include <iostream>
#include <fstream>

using namespace ACGL::Base;
using namespace ACGL::Utils;
using namespace ACGL::OpenGL;

void RenderObject::updateMappings()
{
    /*
    if (mpFrameBufferObject) {
        // set ShaderProgram out-mappings by the definition of the FBO
        mpShaderProgramObject->getShaderProgram()->setFragmentDataLocations( mpFrameBufferObject->getAttachmentLocations() );
    }

    mpVertexArrayObject->setAttributeLocations( mpShaderProgramObject->getShaderProgram()->getAttributeLocations() );
    */
}

void RenderObject::enableFrameBufferObject() const
{
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &mLastFBO );

    if(mpFrameBufferObject)
    {
        mpFrameBufferObject->bind();
    }
    else
    {
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    }
}

void RenderObject::disableFrameBufferObject() const
{
    glBindFramebuffer( GL_FRAMEBUFFER, mLastFBO );
}


void RenderObject::enableShaderProgramObject() const
{
    glGetIntegerv( GL_CURRENT_PROGRAM, &mLastShaderProgram );
    mpShaderProgramObject->use();
}

void RenderObject::disableShaderProgramObject() const
{
    glUseProgram( mLastShaderProgram );
}

void RenderObject::enableVertexArrayObject() const
{
    // remember old VAO
    glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &mLastVAO );

    mpVertexArrayObject->bind();
}

void RenderObject::disableVertexArrayObject() const
{
    glBindVertexArray( mLastVAO );
}

void RenderObject::enableViewport() const
{
    mLastViewport.setFromGLContext();

    if(mpViewport)
    {
        mpViewport->use();
    } else if (mpFrameBufferObject) {
        glm::uvec3 size = mpFrameBufferObject->getSize();
        glViewport( 0, 0, size.x, size.y );
    }
}

void RenderObject::disableViewport() const
{
    mLastViewport.use();
}

#endif

#endif
