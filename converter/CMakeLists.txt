CMAKE_MINIMUM_REQUIRED (VERSION 2.6) 

PROJECT(Converter)

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

#for lodepng
SET(ACGL_BASE_DIR "${CMAKE_SOURCE_DIR}/extern/acgl")
FILE(GLOB_RECURSE HEADER_FILES_EXTERN   "${ACGL_BASE_DIR}/include/lodepng/*.h")
FILE(GLOB_RECURSE SOURCE_FILES_EXTERN "${ACGL_BASE_DIR}/src/lodepng/*.cpp")
INCLUDE_DIRECTORIES("${ACGL_BASE_DIR}/include")

#INCLUDE(${CMAKE_SOURCE_DIR}/extern/acgl/CMakeListsStaticInclude.txt)

#
#
###############################################################################

FILE(GLOB_RECURSE SOURCE_FILES_CONV "${CMAKE_SOURCE_DIR}/converter/*.cc")
SET(SOURCE_FILES_CONV ${SOURCE_FILES_CONV} ${SOURCE_FILES_EXTERN})

FILE(GLOB_RECURSE HEADER_FILES_CONV "${CMAKE_SOURCE_DIR}/converter/*.hh")
SET(HEADER_FILES_CONV ${HEADER_FILES_CONV} ${HEADER_FILES_EXTERN})
ADD_EXECUTABLE(converter ${SOURCE_FILES_CONV} ${HEADER_FILES_CONV})
