# - Find ACGL
# Find the native ACGL headers
#
#  ACGL_INCLUDE_DIR  - where to find ACGL.h, etc.
#  ACGL_LIBRARIES    - ACGL library
#  ACGL_DEFINES      - ACGL defines
#  ACGL_FOUND        - True if ACGL found.

IF(ACGL_FOUND)
    # Already in cache, be silent
    SET(ACGL_FIND_QUIETLY TRUE)
ENDIF()

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/DefinesACGL.txt)

IF(BUILD_ACGL OR ACGL_GLOBAL_EXTERN_DIR_FOUND_FIRST_TIME OR BUILD_TYPE_CHANGED)
    UNSET(ACGL_INCLUDE_DIR CACHE)
    UNSET(ACGL_LIBRARIES CACHE)
ENDIF()

IF(BUILD_ACGL)
    SET(ACGL_INCLUDE_DIR "${ACGL_LOCAL_EXTERN_DIR}/acgl/include")
    SET(ACGL_LIBRARIES ACGL${COMPILE_POSTFIX})
ELSE()
    # Look for the header file.
    IF(ACGL_GLOBAL_EXTERN_DIR)
        FIND_PATH(ACGL_INCLUDE_DIR NAMES ACGL/ACGL.hh PATHS "${ACGL_GLOBAL_EXTERN_DIR}/acgl/include" NO_DEFAULT_PATH)
    ENDIF()
    IF(NOT ACGL_INCLUDE_DIR)
        FIND_PATH(ACGL_INCLUDE_DIR NAMES ACGL/ACGL.hh)
    ENDIF()

    # Look for the library file.
    IF(ACGL_GLOBAL_EXTERN_DIR)
        FIND_LIBRARY(ACGL_LIBRARIES NAMES ACGL${COMPILE_POSTFIX} PATHS "${ACGL_GLOBAL_EXTERN_DIR}/acgl/lib" NO_DEFAULT_PATH)
    ENDIF()
    IF(NOT ACGL_LIBRARIES)
        FIND_LIBRARY(ACGL_LIBRARIES NAMES ACGL${COMPILE_POSTFIX})
    ENDIF()
ENDIF()

# Copy the results to the output variables.
IF(ACGL_INCLUDE_DIR AND ACGL_LIBRARIES)
    SET(ACGL_FOUND TRUE CACHE INTERNAL "")
ELSE()
    SET(ACGL_FOUND FALSE CACHE INTERNAL "")
ENDIF()

# Report the results.
IF(NOT ACGL_FOUND)
    SET(ACGL_MESSAGE "ACGL was not found. Make sure ACGL_INCLUDE_DIR AND ACGL_LIBRARIES are set correctly.")
    IF(ACGL_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "${ACGL_MESSAGE}")
    ELSEIF(NOT ACGL_FIND_QUIETLY)
        MESSAGE(STATUS "${ACGL_MESSAGE}")
    ENDIF()
ELSEIF(NOT ACGL_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for ACGL - found")
ENDIF()

MESSAGE(STATUS "ACGL_INCLUDE_DIR:${ACGL_INCLUDE_DIR}")
MESSAGE(STATUS "ACGL_LIBRARIES:${ACGL_LIBRARIES}")

