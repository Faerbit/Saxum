/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef deprecated

#ifndef ACGL_OPENGL_OBJECTS_SHADERPROGRAMOBJECT_HH
#define ACGL_OPENGL_OBJECTS_SHADERPROGRAMOBJECT_HH

/*
 * A ShaderProgramObject has no direct OpenGL counterpart but it is a combination
 * of a ShaderProgram an a set of Uniforms which are saved on the client side
 * until that ShaderProgram gets used.
 *
 * This way we can have multiple ShaderProgramOpbjects for the same ShaderProgram
 * but with different values for the uniforms (think of 5 objects which use the same
 * material shader but have different ModelviewMatrixes and colors set as uniforms,
 * in that case we only need one ShaderProgram but 5 descriptions of the different
 * uniform values. Thats what this class provides).
 *
 * You might not use this class directly but as a part of a RenderObject!
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/OpenGL/Objects/ShaderProgram.hh>
#include <ACGL/OpenGL/HiLevelObjects/Uniform.hh>


namespace ACGL{
namespace OpenGL{

class ShaderProgramObject
{
    ACGL_NOT_COPYABLE(ShaderProgramObject)

    // ==================================================================================================== \/
    // ============================================================================================ STRUCTS \/
    // ==================================================================================================== \/
public:
    struct UniformAttachment
    {
        GLint location;
        ConstSharedUniform uniform;
    };

    struct UniformTextureAttachment
    {
        GLint location;
        ConstSharedUniformTexture uniformTexture;
    };

    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef std::vector< UniformAttachment >        UniformAttachmentVec;
    typedef std::vector< UniformTextureAttachment > UniformTextureAttachmentVec;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    ShaderProgramObject(const ConstSharedShaderProgram& _shaderProgram)
    :   mpShaderProgram(_shaderProgram)
    {}

    virtual ~ShaderProgramObject(void) {}

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline ConstSharedShaderProgram    getShaderProgram             (void) const { return mpShaderProgram;            }
    inline UniformAttachmentVec        getUniformAttachments        (void) const { return mUniformAttachments;        }
    inline UniformTextureAttachmentVec getUniformTextureAttachments (void) const { return mUniformTextureAttachments; }

    // ===================================================================================================== \/
    // ============================================================================================ WRAPPERS \/
    // ===================================================================================================== \/
public:
    void use (void) const;
    void updateUniforms (void) const;

    inline void attachUniform(
        const std::string& _name,
        const ConstSharedUniform& _uniform)
    {
        UniformAttachment uniformAttachment = { mpShaderProgram->getUniformLocation(_name), _uniform };
        mUniformAttachments.push_back(uniformAttachment);
    }

    inline void attachUniformTexture(
        const std::string& _name,
        const ConstSharedUniformTexture& _uniformTexture)
    {
        UniformTextureAttachment uniformTextureAttachment = { mpShaderProgram->getUniformLocation(_name), _uniformTexture };
        mUniformTextureAttachments.push_back(uniformTextureAttachment);
    }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    ConstSharedShaderProgram    mpShaderProgram;
    UniformAttachmentVec        mUniformAttachments;
    UniformTextureAttachmentVec mUniformTextureAttachments;
};

ACGL_SMARTPOINTER_TYPEDEFS(ShaderProgramObject)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_SHADERPROGRAMOBJECT_HH


#endif
