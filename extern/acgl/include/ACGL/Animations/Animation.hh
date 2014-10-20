/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_ANIMATIONS_ANIMATION_HH
#define ACGL_ANIMATIONS_ANIMATION_HH

#include <ACGL/ACGL.hh>
#include <ACGL/Types.hh>
#include <ACGL/Base/Macros.hh>
#include <ACGL/Animations/EaseFunctions.hh>
#include <ACGL/Animations/Interpolator.hh>

#include <list>
#include <queue>

namespace ACGL{
namespace Animations{

// Animation *************************************************************************************
class Animation
{
public:
    enum RepeatMode {Endless = -1, EndlessNoRepeat = -2};

    Animation() :
        mInited(false),
        mStopped(false) {}
    virtual ~Animation() {}

    virtual void init() = 0;
    virtual long update(uint_t msec) = 0;
    virtual bool finished() = 0;

    inline const bool& isInited() const;
    inline const bool& isStopped() const;

    inline void restart();
    inline void stop();
protected:

    bool mInited;
    bool mStopped;
private:

};

ACGL_SMARTPOINTER_TYPEDEFS(Animation)

typedef std::list<SharedAnimation> AnimationList;
typedef std::queue<SharedAnimation> AnimationQueue;

class AnimationManager
{
public:
    static void push(const SharedAnimation&  _animation);
    static void update(uint_t _msec);

    static void cleanUp();
protected:
private:
    static AnimationList mAnimations;

};

class AnimationEventCallbackFunction
{
public:
    typedef void (*Function)(uint_t);

    AnimationEventCallbackFunction(Function _pFunction,uint_t _id) :
        mpFunction(_pFunction),
        mId(_id) {  }

    inline void call()
    {
        mpFunction(mId);
    }

private:

    Function mpFunction;
    uint_t mId;
};

template <class T>
class AnimationEventCallbackMemberFunction
{
public:
    typedef void (T::*Function)(uint_t);

    AnimationEventCallbackMemberFunction(const T* _pTarget, Function _pFunction,uint_t _id) :
        mpTarget(_pTarget),
        mpFunction(_pFunction),
        mId(_id) { }

    inline void call()
    {
        mpTarget->mpFunction(mId);
    }

private:
    const T* mpTarget;
    Function mpFunction;
    uint_t mId;
};

// AnimationEvent *************************************************************************************

template <class Callback>
class AnimationEvent : public Animation
{
public:
    AnimationEvent(const Callback& _callback) :
        Animation(),
        mCallback(_callback),
        mEventCalled(false) {  }

    virtual void init()
    {
        mEventCalled = false;

        mInited = true;
    }

    virtual long update(uint_t _msec)
    {
        mCallback.call();
        mEventCalled = true;

        return _msec;
    }

    virtual bool finished()
    {
        return mEventCalled;
    }

    Callback& getCallback()
    {
        return mCallback;
    }

private:
    Callback mCallback;

    bool mEventCalled;
};


// AnimationWait *************************************************************************************

class AnimationWait : public Animation
{
public:
    AnimationWait(const uint_t _duration);

    virtual void init();
    virtual long update(uint_t _msec);
    virtual bool finished();

private:
    long mTimeLeft;
    long mDuration;
};

ACGL_SMARTPOINTER_TYPEDEFS(AnimationWait)

// AnimationSequential *************************************************************************************

class AnimationSequential : public Animation
{
public:
    AnimationSequential(const int_t _loops = 0);
    virtual ~AnimationSequential();

    virtual void init();
    virtual long update(uint_t _msec);
    virtual bool finished();

    void push_animation(const SharedAnimation& _animation);

private:
    int_t mLoops;

    AnimationList::iterator mCurrentPosition;
    AnimationList mAnimations;
};

ACGL_SMARTPOINTER_TYPEDEFS(AnimationSequential)

// AnimationParallel *************************************************************************************

class AnimationParallel : public Animation
{
public:
    AnimationParallel(const int_t _loops = 0);
    virtual ~AnimationParallel();

    virtual void init();
    virtual long update(uint_t _msec);
    virtual bool finished();

    void push_animation(const SharedAnimation& animation);

private:
    int_t mLoops;

    AnimationList mAnimations;
    uint_t mRunningAnimations;
};

ACGL_SMARTPOINTER_TYPEDEFS(AnimationParallel)


// AnimationVariable *************************************************************************************
template <class T, class Interpolator>
class AnimationVariable : public Animation
{
public:

    AnimationVariable(T& _data, uint_t _duration, const SharedEaseFunction& _ease_function, const Interpolator& _interpolator = Interpolator()) :
        mpEaseFunction(_ease_function),
        mInterpolator(_interpolator),
        mCurrentTime(0),
        mDuration(_duration),
        mpData(_data) { }

    virtual ~AnimationVariable()
    {
         mBlendInterpolators.clear();
    }

    virtual void init()
    {
        mCurrentTime = 0;
        mInterpolator.init(mpData);

        mInited = true;
    }

    virtual long update(uint_t _msec)
    {
        long time_span;
        if(mDuration-mCurrentTime >= _msec) time_span = _msec;
        else time_span = mDuration-mCurrentTime;

        mCurrentTime += time_span;
        float progress = (float)mCurrentTime/(float)mDuration;

        mpData = mInterpolator.interpolate( mpEaseFunction->value(progress) );

        float sub_progress;

        for(unsigned int i = 0;i < mBlendInterpolators.size();i++)
        {
            sub_progress = (progress - mBlendInterpolators[i].start)/(mBlendInterpolators[i].end - mBlendInterpolators[i].start);
            if(0.0f <= sub_progress && sub_progress <= 1.0f)
            {
                mBlendInterpolators[i].mpInterpolator->blend(&mpData, mBlendInterpolators[i].mpEaseFunction->value(sub_progress) );
            }
        }

        return _msec-time_span;
    }

    virtual bool finished()
    {
        if(mCurrentTime >= mDuration)
            mpData = mInterpolator.finish();

        return mCurrentTime >= mDuration;
    }

    void add_blend_interpolator(const SharedBlendInterpolator& _interpolator, const SharedEaseFunction& _ease_function, float _start = 0.0f, float _end = 1.0f)
    {
        mBlendInterpolators.push_back((blend_interpolator_t){_interpolator, _ease_function, _start, _end, false});
    }

    inline const SharedEaseFunction& get_ease_function(){ return mpEaseFunction;}
    inline void set_ease_function(const SharedEaseFunction& _ease_function){ mpEaseFunction.reset(); mpEaseFunction = _ease_function;}

    inline Interpolator& get_interpolator(){ return mInterpolator; }
    //void set_interpolator(Interpolator* interpolator){ this->interpolator = interpolator; }

protected:
private:
    SharedEaseFunction mpEaseFunction;
    Interpolator mInterpolator;

    uint_t mCurrentTime;
    uint_t mDuration;

    T& mpData;

    struct blend_interpolator_t{
        SharedBlendInterpolator mpInterpolator;
        SharedEaseFunction mpEaseFunction;
        float start, end;
        bool active;
    };
    typedef std::vector<blend_interpolator_t> BlendInterpolators;

    BlendInterpolators mBlendInterpolators;

};

typedef AnimationVariable<float, LinearInterpolatorFloat > AnimationFloatLinear;
ACGL_SMARTPOINTER_TYPEDEFS(AnimationFloatLinear)

typedef AnimationVariable<glm::vec2, LinearInterpolatorVec2 > AnimationVec2Linear;
ACGL_SMARTPOINTER_TYPEDEFS(AnimationVec2Linear)

typedef AnimationVariable<glm::vec3, LinearInterpolatorVec3 > AnimationVec3Linear;
ACGL_SMARTPOINTER_TYPEDEFS(AnimationVec3Linear)

template <class T, class SpeedInterpolator>
class AnimationSpeed : public Animation{
public:
    AnimationSpeed(T& _data, float _targetSpeed, float _acceleration = 1.0f, float _decceleration = 1.0f, float _startSpeed = 0.0f, const SpeedInterpolator& _interpolator = SpeedInterpolator()) :
        mCurrentSpeed(_startSpeed),
        mTargetSpeed(_targetSpeed),
        mAcceleration(_acceleration),
        mDecceleration(_decceleration),
        mpData(_data),
        mInterpolator(_interpolator){ }

    virtual void init()
    {
        mInterpolator.init(mpData);
        mInited = true;
    }

    virtual long update(uint_t _msec)
    {
        //Calculate the distance
        float distance = mCurrentSpeed*((float)_msec)/1000.0f;

        //Move on that distance
        float realDistance = mInterpolator.interpolate(mpData, distance);
        long msecLeft = (uint_t)((distance-realDistance)*1000.0f/mCurrentSpeed);

        //Adjust the speed
        if(mCurrentSpeed > mTargetSpeed)
            mCurrentSpeed -= fmin(mCurrentSpeed-mTargetSpeed, mDecceleration*((float)_msec)/1000.0f);
        else
            mCurrentSpeed += fmin(mTargetSpeed-mCurrentSpeed, mAcceleration*((float)_msec)/1000.0f);

        return msecLeft;
    }

    virtual bool finished()
    {
        if(mInterpolator.finished())
            mInited = false;
        return mInterpolator.finished();
    }

    SpeedInterpolator& getSpeedInterpolator(){return mInterpolator;}

    void setTargetSpeed(float _value)
    {
        mTargetSpeed = _value;
    }

    void setSpeed(float _value)
    {
        mCurrentSpeed = _value;
    }

    void setAcceleration(float _value)
    {
        mAcceleration = _value;
    }

    void setDecceleration(float _value)
    {
        mDecceleration = _value;
    }

private:
    float mCurrentSpeed;
    float mTargetSpeed;
    float mAcceleration;
    float mDecceleration;

    T& mpData;

    SpeedInterpolator mInterpolator;
};

}
}



#endif // ACGL_ANIMATIONS_ANIMATION_HH

#endif
