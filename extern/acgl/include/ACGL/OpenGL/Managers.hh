/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

/*
 * A shortcut to include all manager of OpenGL objects.
 */

#include <ACGL/ACGL.hh>
#include <ACGL/OpenGL/Objects.hh>
#include <ACGL/Resource/NameManager.hh>
#include <ACGL/Resource/FileManager.hh>

namespace ACGL{
namespace OpenGL{

// useful for automatically updating shader programs:
typedef Resource::MultiFileManager<ShaderProgram> ShaderProgramFileManager;

// same for meshes:
typedef Resource::FileManager<VertexArrayObject> VAOFileManager;

// for simple 2D (mip-mapped) textures
typedef Resource::FileManager<Texture2D> Texture2DFileManager;

// used internally:
typedef Resource::FileManager<Shader> ShaderFileManager;

} // OpenGL
} // ACGL

