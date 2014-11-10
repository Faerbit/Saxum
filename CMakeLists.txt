CMAKE_MINIMUM_REQUIRED (VERSION 2.6) 

# project/binary name:
PROJECT(MarbleRaceGroupC)

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
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/binaries)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

# source and header files
FILE(GLOB_RECURSE SOURCE_FILES_SHARED "../shared/*.cc")
FILE(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/*.cc")
SET(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_SHARED})

FILE(GLOB_RECURSE HEADER_FILES_SHARED "../shared/*.hh")
FILE(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/*.hh")
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_SHARED})

# shader files
FILE(GLOB_RECURSE SHADER_FILES "${CMAKE_SOURCE_DIR}/Shader/*.*")

# Readme
FILE(GLOB_RECURSE README_FILES "${CMAKE_SOURCE_DIR}/../README.TXT")
#
#
###############################################################################

###############################################################################
#
# all we need for ACGL:
#
ADD_DEFINITIONS(-DACGL_ERROR_LEVEL_EC3)
INCLUDE(${CMAKE_SOURCE_DIR}/extern/acgl/CMakeListsStaticInclude.txt)
#
#
###############################################################################

###############################################################################
#
# GLFW (and some other linker flags)
#

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
    SET(LIBRARIES ${LIBRARIES} -lXrandr -lGL -lXi  -pthread -lm -lX11 -lXxf86vm)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#
#
#
###############################################################################

FILE(GLOB_RECURSE HEADER_FILES_BULLET ${CMAKE_SOURCE_DIR}/extern/bullet/src/*.h)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/extern/bullet/src)
SET(HEADER_FILES ${HEADER_FILES} ${HEADER_FILES_BULLET})

SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/BulletCollision/libBulletCollision.a)
SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/BulletDynamics/libBulletDynamics.a)
SET (LIBRARIES ${LIBRARIES} ${CMAKE_SOURCE_DIR}/extern/bullet/build/src/LinearMath/libLinearMath.a)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${README_FILES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} ${LIBRARIES})
