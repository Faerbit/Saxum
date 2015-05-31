CMAKE_MINIMUM_REQUIRED (VERSION 2.6) 

add_subdirectory(converter)

# project/binary name:
PROJECT(saxum)

# ACGL setup
SET(ACGL_OPENGL_SUPPORT CORE_32)
ADD_DEFINITIONS(-DACGL_OPENGL_VERSION_32)
ADD_DEFINITIONS(-DACGL_OPENGL_PROFILE_CORE)
# create the newest availabe OpenGL context, independent of the ACGL version:
#ADD_DEFINITIONS(-DGLFW_OPENGL_LATEST_VERSION)

###############################################################################
#
# Compiler settings, can be simpler if only one compiler should be used.
#

#Enable c++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #on clang we need to find out the version to set stdlib if needed
  # if clang version is less than 3.3 ( XCode 5.0) you need to set the stdlib
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  MESSAGE("Clangversion ${CLANG_VERSION_STRING}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
endif()

# enable warnings
IF(MSVC)
# for msvc also set multiple processors 
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W3")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP /W3")
ELSE(MSVC)
ADD_DEFINITIONS(-Wall)
ENDIF(MSVC)
ADD_DEFINITIONS(-DNO_SPACE_NAVIGATOR_SUPPORT)

#
#
###############################################################################

###############################################################################
#
# Settings for the app.
#
#
# Lets the binary get written to a shared folder (which can be ignored by git).
# Will also set the run directory for QTCreator:
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -DSAXUM_DEBUG -g")
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -DSAXUM_DEBUG -g")

SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2")

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

# source and header files
FILE(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/game/*.cc")

FILE(GLOB HEADER_FILES "${CMAKE_SOURCE_DIR}/game/*.hh")

# tinyxml

FILE(GLOB HEADER_FILES_TINYXML "${CMAKE_SOURCE_DIR}/extern/tinyxml/*.hh")
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_TINYXML})
FILE(GLOB SOURCE_FILES_TINYXML "${CMAKE_SOURCE_DIR}/extern/tinyxml/*.cc")
SET(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_TINYXML})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/tinyxml)
#
#
###############################################################################

###############################################################################
#
# all we need for ACGL:
#
INCLUDE(${CMAKE_SOURCE_DIR}/extern/acgl/CMakeListsStaticInclude.txt)
#
#
###############################################################################

###############################################################################
#
# GLFW (and some other linker flags)
#

OPTION(GLFW_BUILD_DOCS OFF)
OPTION(GLFW_BUILD_TESTS OFF)
OPTION(GLFW_BUILD_EXAMPLES OFF)
OPTION(GLFW_INSTALL OFF)

FILE(GLOB_RECURSE HEADER_FILES_GLFW "${CMAKE_SOURCE_DIR}/extern/glfw/include/*.h")
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/glfw/include)
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_GLFW})

add_subdirectory(extern/glfw ${CMAKE_SOURCE_DIR}/extern/glfw)
SET(LIBRARIES glfw ${GLFW_LIBRARIES})


#
# MacOS X:
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(LIBRARIES ${LIBRARIES} -Wl,-framework,Cocoa -Wl,-framework,OpenGL,-framework,IOKit)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

#
# Linux:
#
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    SET(LIBRARIES ${LIBRARIES} -lXrandr -lGL -lXi  -pthread -lm -lX11 -lXxf86vm -ldl)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#
#
#
###############################################################################

FILE(GLOB_RECURSE HEADER_FILES_BULLET ${CMAKE_SOURCE_DIR}/extern/bullet/src/*.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/bullet/src)
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_BULLET})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/lib/libBulletDynamics.a)
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/lib/libBulletCollision.a)
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/lib/libLinearMath.a)
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/lib/libBulletSoftBody.a)
    SET (LIBRARIES -static ${LIBRARIES} -static-libgcc -static-libstdc++)
else()
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/BulletDynamics/libBulletDynamics.a)
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/BulletCollision/libBulletCollision.a)
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/LinearMath/libLinearMath.a)
    SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/BulletSoftBody/libBulletSoftBody.a)
endif()

# Include Lua

FILE(GLOB_RECURSE HEADER_FILES_LUA ${CMAKE_SOURCE_DIR}/extern/lua/src/*.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/lua/src)
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_LUA})

FILE(GLOB_RECURSE HEADER_FILES_LUA_BRIDGE ${CMAKE_SOURCE_DIR}/extern/luabridge/*.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/luabridge)
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_LUA_BRIDGE})

SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/lua/src/liblua.a)


ADD_EXECUTABLE(saxum ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES(saxum ${LIBRARIES} ${CMAKE_DL_LIBS})
