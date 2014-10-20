/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_ANIMATIONS_EASEFUNCTIONS_HH
#define ACGL_ANIMATIONS_EASEFUNCTIONS_HH

#include <ACGL/ACGL.hh>

#include <ACGL/Math/Math.hh>
#include <ACGL/Base/Macros.hh>

namespace ACGL
{
namespace Animations
{

class EaseFunction
{
public:
    virtual ~EaseFunction(){}

    virtual float value(const float _progress) = 0;
};

ACGL_SMARTPOINTER_TYPEDEFS(EaseFunction)

/*
float Linear(float progress, ease_function_parameter& parameter);
float InQuad(float progress, ease_function_parameter& parameter);
float OutQuad(float progress, ease_function_parameter& parameter);
float InOutQuad(float progress, ease_function_parameter& parameter);

float BlendLinear(float progress, ease_function_parameter& parameter);
float BlendSin(float progress, ease_function_parameter& parameter);
float BlendCos(float progress, ease_function_parameter& parameter);
float BlendSinDamped(float progress, ease_function_parameter& parameter);
float BlendCosDamped(float progress, ease_function_parameter& parameter);
*/


namespace EaseFunctions{

class Linear : public EaseFunction
{
public:
    Linear(){ }
    virtual ~Linear(){}

    virtual float value(const float _progress)
    {
        return _progress;
    }
};
class InQuad : public EaseFunction
{
public:
    InQuad(){ }
    virtual ~InQuad(){}

    virtual float value(const float _progress)
    {
        return _progress*_progress;
    }
};
class OutQuad : public EaseFunction
{
public:
    OutQuad(){ }
    virtual ~OutQuad(){}

    virtual float value(const float _progress)
    {
        return 1.0f-_progress*_progress;
    }
};
class InOutQuad : public EaseFunction
{
public:
    InOutQuad(){ }
    virtual ~InOutQuad(){}

    virtual float value(const float _progress)
    {
        if(_progress < 0.5)
            return 2.0f*_progress*_progress;
        else
            return 1.0f-2.0f*pow(_progress-1.0f,2.0f);
    }
};

class Sin : public EaseFunction
{
public:
    Sin(float _frequency = 1.0f) :
        mFrequency(_frequency) { }
    virtual ~Sin(){}

    virtual float value(const float _progress)
    {
        return sin(0.5f*(float)M_PI*mFrequency*_progress);
    }

    float mFrequency;
};

class BlendLinear : public EaseFunction
{
public:
    BlendLinear(){ }
    virtual ~BlendLinear(){}

    virtual float value(const float _progress)
    {
		return 2.0f*Math::Functions::min(_progress, 1.0f-_progress);
    }
};
class BlendSin : public EaseFunction
{
public:
    BlendSin(){ }
    virtual ~BlendSin(){}

    virtual float value(const float _progress)
    {
        return sin(20*(float)M_PI*_progress);
    }
};
class BlendCos : public EaseFunction
{
public:
    BlendCos(){ }
    virtual ~BlendCos(){}

    virtual float value(const float _progress)
    {
        return cos(6*(float)M_PI*_progress)-1.0f;
    }
};
class BlendSinDamped : public EaseFunction
{
public:
    BlendSinDamped(){ }
    virtual ~BlendSinDamped(){}

    virtual float value(const float _progress)
    {
        return 2.0f*Math::Functions::min(_progress, 1.0f-_progress)*sin(10.0f*(float)M_PI*_progress);
    }
};
class BlendCosDamped : public EaseFunction
{
public:
    BlendCosDamped(){ }
    virtual ~BlendCosDamped(){}

    virtual float value(const float _progress)
    {
        return 2.0f*Math::Functions::min(_progress, 1.0f-_progress)*cos(10.0f*(float)M_PI*_progress);
    }
};

}

}
}

#endif // ACGL_ANIMATIONS_EASEFUNCTIONS_HH

#endif
