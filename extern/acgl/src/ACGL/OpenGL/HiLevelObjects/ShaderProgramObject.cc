/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef deprecated

#include <ACGL/OpenGL/HiLevelObjects/ShaderProgramObject.hh>

using namespace ACGL::OpenGL;

void ShaderProgramObject::use(void) const
{
    mpShaderProgram->use();
    updateUniforms();
}

void ShaderProgramObject::updateUniforms(void) const
{
    for(UniformAttachmentVec::size_type i = 0; i < mUniformAttachments.size(); ++i)
        //if(mUniformAttachments[i].location != -1) // this test breaks the SWP template
            mUniformAttachments[i].uniform->apply(mUniformAttachments[i].location);

    for(UniformTextureAttachmentVec::size_type i = 0; i < mUniformTextureAttachments.size(); ++i)
        //if(mUniformTextureAttachments[i].location != -1) // this test breaks the SWP template
            mUniformTextureAttachments[i].uniformTexture->apply(mUniformTextureAttachments[i].location, i);
}

#endif
