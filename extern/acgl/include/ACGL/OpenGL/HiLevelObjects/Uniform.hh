/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef deprecated

#ifndef ACGL_OPENGL_OBJECTS_UNIFORM_HH
#define ACGL_OPENGL_OBJECTS_UNIFORM_HH

/*
 * Uniforms do not map to OpenGL objects directly, to set a uniform value in a
 * ShaderProgram, use the setUniform() / setProgramUniform() methods there with
 * the raw values you want to ste.
 *
 * This class provides the ability to save uniform values on the client side and
 * wait with the uploading until the ShaderProgram gets used. It gets used in
 * combination with the ShaderProgramObject/RenderObject (see there).
 */

#include <ACGL/ACGL.hh>

#include <ACGL/Base/Macros.hh>
#include <ACGL/OpenGL/GL.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/OpenGL/Objects/Texture.hh>
#include <vector>

namespace ACGL{
namespace OpenGL{

// ================================================================================================== \/
// ================================================================================== BASIC INTERFACE \/
// ================================================================================================== \/
class Uniform
{
public:
    //! Apply the uniform to a specified location
    virtual void apply(GLint) const = 0;
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform)

// ================================================================================================= \/
// ===================================================================================== C-SIDE DATA \/
// ================================================================================================= \/
template<typename T>
class UniformData
{
    // ===================================================================================================== \/
    // ============================================================================================ TYPEDEFS \/
    // ===================================================================================================== \/
public:
    typedef T DATA_TYPE;

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    UniformData(void) : mValue() {}
    virtual ~UniformData(void) {}

    // ==================================================================================================== \/
    // ============================================================================================ SETTERS \/
    // ==================================================================================================== \/
public:
    inline void setValue (const T& _value) { mValue = _value; }

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline const T& getValue (void) const { return mValue; }

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    T mValue;
};

// ===================================================================================================== \/
// ===================================================================================== IMPLEMENTATIONS \/
// ===================================================================================================== \/
class Uniform1i : public UniformData<GLint>, public Uniform
{
public:
    Uniform1i(void) : UniformData<GLint>() {}
    virtual ~Uniform1i(void) {}
    void apply(GLint _location) const { glUniform1i(_location, mValue); }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform1i)

//=========================

#if (ACGL_OPENGL_VERSION >= 30)
class Uniform1ui : public UniformData<GLuint>, public Uniform
{
public:
    Uniform1ui(void) : UniformData<GLuint>() {}
    virtual ~Uniform1ui(void) {}
    void apply(GLint _location) const { glUniform1ui(_location, mValue); }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform1ui)
#endif

//=========================

class Uniform1f : public UniformData<GLfloat>, public Uniform
{
public:
    Uniform1f(void) : UniformData<GLfloat>() {}
    virtual ~Uniform1f(void) {}
    void apply(GLint _location) const { glUniform1f(_location, mValue); }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform1f)

//=========================

class Uniform1fv : public UniformData<std::vector<GLfloat> >, public Uniform
{
public:
    virtual ~Uniform1fv(void) {}
    void apply(GLint _location) const
    {
        if(!mValue.empty())
            glUniform1fv(_location, (GLsizei)mValue.size(), (GLfloat*)&mValue[0]);
    }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform1fv)

//=========================

class Uniform2f : public UniformData<glm::vec2>, public Uniform
{
public:
    Uniform2f(void) : UniformData<glm::vec2>() {}
    virtual ~Uniform2f(void) {}
    void apply(GLint _location) const { glUniform2fv(_location, 1, glm::value_ptr(mValue)); }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform2f)


//=========================

class Uniform3f : public UniformData<glm::vec3>, public Uniform
{
public:
    Uniform3f(void) : UniformData<glm::vec3>() {}
    virtual ~Uniform3f(void) {}
    void apply(GLint _location) const { glUniform3fv(_location, 1, glm::value_ptr(mValue)); }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform3f)

//=========================

class Uniform4f : public UniformData<glm::vec4>, public Uniform
{
public:
    Uniform4f(void) : UniformData<glm::vec4>() {}
    virtual ~Uniform4f(void) {}
    void apply(GLint _location) const { glUniform4fv(_location, 1, glm::value_ptr(mValue)); }
};

ACGL_SMARTPOINTER_TYPEDEFS(Uniform4f)

//=========================

class UniformMatrix2f : public UniformData<glm::mat2>, public Uniform
{
public:
    UniformMatrix2f(void) : UniformData<glm::mat2>() {}
    virtual ~UniformMatrix2f(void) {}
    void apply(GLint _location) const { glUniformMatrix2fv(_location, 1, GL_FALSE, glm::value_ptr(mValue)); }
};

ACGL_SMARTPOINTER_TYPEDEFS(UniformMatrix2f)

//=========================

class UniformMatrix3f : public UniformData<glm::mat3>, public Uniform
{
public:
    UniformMatrix3f(void) : UniformData<glm::mat3>() {}
    virtual ~UniformMatrix3f(void) {}
    void apply(GLint _location) const { glUniformMatrix3fv(_location, 1, GL_FALSE, glm::value_ptr(mValue)); }
};

ACGL_SMARTPOINTER_TYPEDEFS(UniformMatrix3f)

//=========================

class UniformMatrix4f : public UniformData<glm::mat4>, public Uniform
{
public:
    UniformMatrix4f(void) : UniformData<glm::mat4>() {}
    virtual ~UniformMatrix4f(void) {}
    void apply(GLint _location) const { glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(mValue)); }
};

ACGL_SMARTPOINTER_TYPEDEFS(UniformMatrix4f)

//=========================
class UniformTexture : public UniformData<ConstSharedTextureBase>
{
public:
    UniformTexture(void) : UniformData<ConstSharedTextureBase>() {}
    virtual ~UniformTexture(void) {}
    void apply(GLint _location, GLenum _unit) const { glUniform1i(_location, _unit); mValue->bind(_unit); }
};

ACGL_SMARTPOINTER_TYPEDEFS(UniformTexture)

} // OpenGL
} // ACGL

#endif // ACGL_OPENGL_OBJECTS_UNIFORM_HH


#endif
