/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifndef ACGL_MATH_CONSTANTS_HH
#define ACGL_MATH_CONSTANTS_HH

/*
 * Some mathmatical constants, for example readable degree to rad conversion.
 *
 * DON'T INCLUDE THIS DIRECTLY! Include <ACGL/Math.hh> instead!
 */

#include <ACGL/ACGL.hh>

#include <cmath>
#include <limits>

#ifndef M_PI
// M_PI is not defined on e.g. VS2010 (isn't part of the standart), so in that case it gets defined here
// outside of the namespace because some code might expect it to be in math.h which is oviously not in our namespace...
#define M_PI 3.14159265358979323846
#endif

namespace ACGL{
namespace Math{
namespace Constants{

//some important constants
const float   INF_FLOAT  = std::numeric_limits<float>::infinity();
const double  INF_DOUBLE = std::numeric_limits<double>::infinity();
const int_t   INF_INT    = std::numeric_limits<int_t>::infinity();
const short_t INF_SHORT  = std::numeric_limits<short_t>::infinity();

template<typename T> inline T INF(void) { return T(); }
template<> inline float   INF<float>   (void) { return INF_FLOAT; }
template<> inline double  INF<double>  (void) { return INF_DOUBLE; }
template<> inline int_t   INF<int_t>   (void) { return INF_INT; }
template<> inline short_t INF<short_t> (void) { return INF_SHORT; }

//gets used for some floating-point comparisions
const float  EPSILON_FLOAT = 0.0005f;
const double EPSILON_DOUBLE = 0.0005;
const float  SQUARED_EPSILON_FLOAT = 0.00000025f;
const double SQUARED_EPSILON_DOUBLE = 0.00000025;

template<typename T> inline T EPSILON(void) { return T(); }
template<> inline float  EPSILON<float> (void) { return EPSILON_FLOAT; }
template<> inline double EPSILON<double>(void) { return EPSILON_DOUBLE; }

template<typename T> inline T SQUARED_EPSILON(void) { return T(); }
template<> inline float  SQUARED_EPSILON<float> (void) { return SQUARED_EPSILON_FLOAT; }
template<> inline double SQUARED_EPSILON<double>(void) { return SQUARED_EPSILON_DOUBLE; }

//Sine and Cosine stuff
const float  PI_FLOAT  = (float)  M_PI;
const double PI_DOUBLE = (double) M_PI;

template<typename T> inline T PI(void) { return T(); }
template<> inline float  PI<float> (void) { return PI_FLOAT; }
template<> inline double PI<double>(void) { return PI_DOUBLE; }

//constants to change from degree to radians and back
const float  DEG_TO_RAD_FLOAT = PI_FLOAT / 180.0f;
const double DEG_TO_RAD_DOUBLE = PI_DOUBLE / 180.0 ;
const float  RAD_TO_DEG_FLOAT = 180.0f / PI_FLOAT;
const double RAD_TO_DEG_DOUBLE = 180.0  / PI_DOUBLE;

template<typename T> inline T DEG_TO_RAD(void) { return T(); }
template<> inline float  DEG_TO_RAD<float> (void) { return DEG_TO_RAD_FLOAT; }
template<> inline double DEG_TO_RAD<double>(void) { return RAD_TO_DEG_DOUBLE; }

template<typename T> inline T RED_TO_DEG(void) { return T(); }
template<> inline float  RED_TO_DEG<float> (void) { return DEG_TO_RAD_FLOAT; }
template<> inline double RED_TO_DEG<double>(void) { return RAD_TO_DEG_DOUBLE; }

} // Constants
} // Math
} // ACGL

#endif // ACGL_MATH_CONSTANTS_HH
