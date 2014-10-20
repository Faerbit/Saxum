/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_ANIMATIONS_INTERPOLATOR_HH
#define ACGL_ANIMATIONS_INTERPOLATOR_HH

#include <ACGL/ACGL.hh>

#include <ACGL/Animations/EaseFunctions.hh>
#include <ACGL/Scene/NURBSCurve.hh>
#include <ACGL/Types.hh>
#include <ACGL/Base/Macros.hh>

#include <vector>

namespace ACGL
{
namespace Animations
{

class BlendInterpolator
{
public:
    virtual void blend(const void* value, const float progress) = 0;
};

ACGL_SMARTPOINTER_TYPEDEFS(BlendInterpolator)

struct interpolator_blend_t
{
    EaseFunction* ease_function;
    BlendInterpolator* interpolator;
    float start, end;
};

typedef std::vector<interpolator_blend_t>  BlendVector;
// Interpolator *************************************************************************************



template <class T>
class LinearInterpolator : public BlendInterpolator
{
public:
    LinearInterpolator() :
        mStartValue(),
        mEndValue(),
        mDiffValue(),
        mSetStartPoint(true)
    {
    }

    LinearInterpolator(const T& _end_value) :
        mStartValue(),
        mEndValue(_end_value),
        mDiffValue(),
        mSetStartPoint(true)
    {
    }

    LinearInterpolator(const T& _start_value, const T& _end_value) :
        mStartValue(_start_value),
        mEndValue(_end_value),
        mDiffValue(_end_value - _start_value),
        mSetStartPoint(false)
    {
    }

    inline void init(const T& _initValue)
    {
        if(mSetStartPoint)
        {
            mStartValue = _initValue;
            mDiffValue = mEndValue-mStartValue;
        }
    }

    inline const T interpolate(const float _progress)
    {
         return mStartValue + mDiffValue*_progress;
    }

    inline const T& finish()
    {
        return mEndValue;
    }

    virtual void blend(const void* _value, const float _progress)
    {
        *((T*)_value) += mEndValue*_progress;
    }

protected:
private:


    T mStartValue;
    T mEndValue;

    T mDiffValue;

    bool mSetStartPoint;
};

typedef LinearInterpolator<float> LinearInterpolatorFloat;
typedef LinearInterpolator<glm::vec2> LinearInterpolatorVec2;
typedef LinearInterpolator<glm::vec3> LinearInterpolatorVec3;


template <class T>
class LinearPathInterpolator
{

public:
    LinearPathInterpolator()
    {
    }

    inline void init(const T& init_value)
    {
    }

    inline const T interpolate(float _progress)
    {
        //TODO: This es very UGLY, implement a better way to find the current keypoint.
        unsigned int i = 0;
        for(i = 0 ;i < mKeypoints.size();i++)
        {
            if(mKeypoints[i].time >= _progress)
                break;
        }

        if(i == 0)
        {
            return mKeypoints[0].data;
        }else if(i < mKeypoints.size())
        {
            float val = (_progress-mKeypoints[i-1].time)
                 /(mKeypoints[i].time-mKeypoints[i-1].time);

            return mKeypoints[i-1].data*(1.0f-val)+mKeypoints[i].data*val;
        }
    }

    inline const T& finish()
    {
        return mKeypoints[mKeypoints.size()-1].data;
    }

    void insertControlPoint(const T& _data, const float _time)
    {
        unsigned int i = 0;
        while(i < mKeypoints.size() && mKeypoints[i].time < _time) i++;
        key_point_t keyPoint = {_data, _time};
        mKeypoints.insert( mKeypoints.begin()+i, keyPoint );
    }

protected:
private:
    struct key_point_t{T data;float time;};
    std::vector<key_point_t> mKeypoints;
};

typedef LinearPathInterpolator<float> LinearPathInterpolatorFloat;
typedef LinearPathInterpolator<glm::vec2> LinearPathInterpolatorVec2;
typedef LinearPathInterpolator<glm::vec3> LinearPathInterpolatorVec3;


template<class T, ACGL::uint_t DATA_DIMENSION>
class NURBSPathInterpolator
{
public:
    NURBSPathInterpolator(ACGL::Scene::NURBSCurve<DATA_DIMENSION> curve) :
        curve(curve)
    {
    }

    void init(T& init_value)
    {
       // insert_control_point(data, 0.0f);
    }

    const T interpolate(float progress)
    {
        curve.insertKnot(progress);
        // TODO: Interpolation code is missing
        return T();
    }

    const T& finish()
    {
        return T();
    }

    inline ACGL::Scene::NURBSCurve<DATA_DIMENSION>& get_curve()
    {
        return curve;
    }

protected:
private:
    ACGL::Scene::NURBSCurve<DATA_DIMENSION> curve;
};

template <class T, class OffsetType, class DataOperations>
class LinearPathSpeedInterpolator
{

public:
    LinearPathSpeedInterpolator() :
        mKeypoints(),
        mOffset(0.0f, 0.0f),
        mCurrentPosition(1)
    {
    }

    inline void init(const T& init_value)
    {
        mCurrentPosition = 0;
    }

    inline float interpolate(T& _data, const float _distance)
    {
        if(mCurrentPosition == mKeypoints.size())
            return 0.0f;

        //Get the next target point
        T source, target;

        if(mCurrentPosition == 0)
        {
            source = _data;
            target = DataOperations::applyStartOffset(mKeypoints[mCurrentPosition], mKeypoints[mCurrentPosition+1], mOffset);
        }else{
            source = DataOperations::applyStartOffset(mKeypoints[mCurrentPosition-1], mKeypoints[mCurrentPosition], mOffset);
            if(mCurrentPosition == mKeypoints.size()-1)
            {
                target = DataOperations::applyOffset(mKeypoints[mCurrentPosition-1], mKeypoints[mCurrentPosition], mOffset);
            }else{
                target = DataOperations::applyOffset(mKeypoints[mCurrentPosition-1], mKeypoints[mCurrentPosition], mKeypoints[mCurrentPosition+1], mOffset);
            }
        }

        bool next;
        T newPosition = DataOperations::getNewPosition(_data, source, target, _distance, next);

        float realDistance = DataOperations::distance(_data, newPosition);

        _data = newPosition;

        //If we reached the next control point and there are keypoints left
        if(next)
        {
            mCurrentPosition++;

            return realDistance+interpolate(_data, _distance-realDistance);
        }else
        {
            return realDistance;
        }
    }

    inline bool finished()
    {
        return mCurrentPosition == mKeypoints.size();
    }

    inline OffsetType& getOffset()
    {
        return mOffset;
    }

    void appendControlPoint(const T& _data)
    {
        mKeypoints.push_back(_data);
    }

protected:
private:
    std::vector<T> mKeypoints;

    OffsetType mOffset;

    unsigned int mCurrentPosition;
};



}
}

#endif // INTERPOLATOR_HH

#endif
