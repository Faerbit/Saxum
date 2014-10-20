/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what deprecated means.
#ifdef ACGL_INCLUDE_DEPRECATED

#ifndef ACGL_OPENGL_OBJECTS_RENDEROBJECT_HH
#define ACGL_OPENGL_OBJECTS_RENDEROBJECT_HH

/**
 * A RenderObject combines a FrameBuffer to draw to, a ShaderProgramObject
 * to draw with and a VertexBufferObject to name what to draw.
 *
 * Instead of setting those objects individually and hoping that they match
 * a RenderObject can take care of that.
 *
 * The FBO is optional, if none is set, rendering is performed onscreen.
 * A Viewport is also optional.
 *
 * NOTE: The RenderObject can change the Attributelocation mappings of the VAO
 * as well as the frag data locations of the ShaderProgram.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/GL.hh>
#if (ACGL_OPENGL_VERSION >= 30)

#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include <ACGL/OpenGL/HiLevelObjects/ShaderProgramObject.hh>
#include <ACGL/OpenGL/Objects/FrameBufferObject.hh>
#include <ACGL/OpenGL/HiLevelObjects/Viewport.hh>


namespace ACGL {
namespace OpenGL {

class RenderObject
{
    ACGL_NOT_COPYABLE(RenderObject)

    // ========================================================================================================= \/
    // ============================================================================================ CONSTRUCTORS \/
    // ========================================================================================================= \/
public:
    RenderObject(ConstSharedVertexArrayObject   _vertexArrayObject,
                 ConstSharedShaderProgramObject _shaderProgramObject,
                 ConstSharedFrameBufferObject   _frameBufferObject = ConstSharedFrameBufferObject(),
                 ConstSharedViewport            _viewport = ConstSharedViewport())
    :   mpVertexArrayObject(_vertexArrayObject),
        mpShaderProgramObject(_shaderProgramObject),
        mpFrameBufferObject(_frameBufferObject),
        mpViewport(_viewport)
    {
        updateMappings();
    }

    // ==================================================================================================== \/
    // ============================================================================================ GETTERS \/
    // ==================================================================================================== \/
public:
    inline ConstSharedVertexArrayObject   getVertexArrayObject()   const { return mpVertexArrayObject;   }
    inline ConstSharedFrameBufferObject   getFrameBufferObject()   const { return mpFrameBufferObject;   }
    inline ConstSharedShaderProgramObject getShaderProgramObject() const { return mpShaderProgramObject; }
    inline ConstSharedViewport            getViewport()            const { return mpViewport;            }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
public:
    //! Assigns the attribute locations of the VAO to the locations of the ShaderProgram where there are matching names
    void updateMappings();

    //! setup everything for drawing and store old states
    inline void enable() const
    {
        enableFrameBufferObject();
        enableShaderProgramObject();
        enableVertexArrayObject();
        enableViewport();
    }

    //! restore old states
    inline void disable() const
    {
        disableViewport();
        disableVertexArrayObject();
        disableShaderProgramObject();
        disableFrameBufferObject();
    }

    //! draws the VAO, call enable() first!
    inline void draw() const
    {
        mpVertexArrayObject->draw();
    }

    //! draws the VAO, everything needed for drawing is performed by this call
    inline void render() const
    {
        enable();
        draw();
        disable();
    }


    // ==================================================================================================== \/
    // ==================================================================== TESTING WAYS TO STAY COMPATIBLE \/
    // ==================================================================================================== \/
public:
    //! for testing only: the new enable/disable/render sets store the old GL states, so they can't be const
    //! some old code works with const RenderObjects. don't rely on this function anymore
    inline void enableC() const
    {
        if(mpFrameBufferObject)
        {
            mpFrameBufferObject->bind();
        }
        else
        {
            glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        }
        mpShaderProgramObject->use();
        mpVertexArrayObject->bind();
        if(mpViewport)
        {
            mpViewport->use();
        } else if (mpFrameBufferObject) {
            glm::uvec3 size = mpFrameBufferObject->getSize();
            glViewport( 0, 0, size.x, size.y );
        }
    }

    //! for testing only: the new enable/disable/render sets store the old GL states, so they can't be const
    //! some old code works with const RenderObjects. don't rely on this function anymore
    inline void disableC() const
    {
        glBindVertexArray( 0 );
    }

    //! for testing only: the new enable/disable/render sets store the old GL states, so they can't be const
    //! some old code works with const RenderObjects. don't rely on this function anymore
    inline void renderC() const
    {
        enableC();
        draw();
        disableC();
    }

    // ==================================================================================================== \/
    // ============================================================================================ METHODS \/
    // ==================================================================================================== \/
private:
    void enableFrameBufferObject() const;
    void disableFrameBufferObject() const;

    void enableShaderProgramObject() const;
    void disableShaderProgramObject() const;

    void enableViewport() const;
    void disableViewport() const;

    void enableVertexArrayObject() const;
    void disableVertexArrayObject() const;

    // =================================================================================================== \/
    // ============================================================================================ FIELDS \/
    // =================================================================================================== \/
protected:
    ConstSharedVertexArrayObject   mpVertexArrayObject;   // mandatory
    ConstSharedShaderProgramObject mpShaderProgramObject; // mandatory
    ConstSharedFrameBufferObject   mpFrameBufferObject;   // optional
    ConstSharedViewport            mpViewport;            // optional

    //! old states so render() can be side-effect free
    mutable Viewport mLastViewport;
    mutable GLint    mLastShaderProgram;
    mutable GLint    mLastFBO;
    mutable GLint    mLastVAO;
};

ACGL_SMARTPOINTER_TYPEDEFS(RenderObject)

}
}

#endif

#endif // ACGL_OPENGL_OBJECTS_RENDEROBJECT_HH


#endif
