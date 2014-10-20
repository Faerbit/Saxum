/**
 *
 * Includes the Oculus Rift LibOVR but tries to suppress as much compiler warnings
 * as possible.
 *
 */

#ifdef ACGL_USE_OCULUS_RIFT

/////////////////////////////////////////////////////////////////////////////////////
// ignore compiler warnings from LibOVR:
//
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning ( disable : 4201 )
#pragma warning ( disable : 4100 )
#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4244 )
#endif

#if (((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) || (__GNUC__ > 4))
#define COMPILER_IS_GCC_4_6_OR_NEWER
#endif

#ifdef __clang__
//   clang/llvm:
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wuninitialized"
#    pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined __GNUC__
#  ifdef COMPILER_IS_GCC_4_6_OR_NEWER
//    gcc >= 4.6:
#     pragma GCC diagnostic push
#     pragma GCC diagnostic ignored "-Wtype-limits"
#     pragma GCC diagnostic ignored "-Wstrict-aliasing"
#     pragma GCC diagnostic ignored "-Wattributes"
#     pragma GCC diagnostic ignored "-Wreorder"
#  endif
// gcc:
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
//
/////////////////////////////////////////////////////////////////////////////////////

#include <OVR.h>
#include <OVRVersion.h>

/////////////////////////////////////////////////////////////////////////////////////
// reactivate compiler warnings:
//
#ifdef __clang__
// clang/llvm:
#  pragma clang diagnostic pop
#elif defined COMPILER_IS_GCC_4_6_OR_NEWER
// gcc >= 4.6:
#  pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif
//
/////////////////////////////////////////////////////////////////////////////////////


#endif // ACGL_USE_OCULUS_RIFT
