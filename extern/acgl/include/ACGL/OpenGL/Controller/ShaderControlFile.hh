/***********************************************************************
 * Copyright 2011-2013 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#pragma once

#include <ACGL/OpenGL/Creator/ShaderCreator.hh>

namespace ACGL{
namespace OpenGL{

typedef ACGL::OpenGL::ShaderCreator ShaderControlFile;
#ifdef _MSC_VER
#pragma message(__FILE__" : warning: ShaderControlFile is deprecated, use ShaderCreator instead (same interface)")
#else
#warning "ShaderControlFile is deprecated, use ShaderCreator instead (same interface)"
#endif

}
}
