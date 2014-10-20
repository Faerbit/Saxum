/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_SCENE_NURBSDATAVECTOR_HH
#define ACGL_SCENE_NURBSDATAVECTOR_HH

#include <ACGL/ACGL.hh>
#include <ACGL/Math/Math.hh>
#include <ACGL/Utils/Log.hh>

namespace ACGL{
namespace Scene{

template<uint_t DATA_DIMENSION>
class NURBSDataVector
{
public:
    NURBSDataVector(void) {}
    NURBSDataVector(const float* _data)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] = _data[i];
    }
    virtual ~NURBSDataVector(void) {}

    static const uint_t sDataDimension;
    uint_t getDataDimension (void) const { return sDataDimension;}

    const NURBSDataVector& operator+=(const NURBSDataVector<DATA_DIMENSION>& _v)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] += _v.data[i];
        return *this;
    }

    const NURBSDataVector& operator-=(const NURBSDataVector<DATA_DIMENSION>& _v)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] -= _v.data[i];
        return *this;
    }

    const NURBSDataVector& operator*=(const NURBSDataVector<DATA_DIMENSION>& _v)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] *= _v.data[i];
        return *this;
    }

    const NURBSDataVector& operator/=(const NURBSDataVector<DATA_DIMENSION>& _v)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] /= _v.data[i];
        return *this;
    }

    const NURBSDataVector& operator*=(float _value)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] *= _value;
        return *this;
    }

    const NURBSDataVector& operator/=(float _value)
    {
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            data[i] /= _value;
        return *this;
    }

    float getLength (void) const
    {
        double sum = 0.0;
        for(uint_t i = 0; i < DATA_DIMENSION; ++i)
            sum += data[i] * data[i];
        return ACGL::Math::Functions::sqrt(sum);
    }

    float operator[](int_t _id) const
    {
        return data[_id];
    }

    float data[DATA_DIMENSION];
};

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator+(const NURBSDataVector<DATA_DIMENSION>& _lhs, const NURBSDataVector<DATA_DIMENSION>& _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs.data[i] + _rhs.data[i];
    return v;
}

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator-(const NURBSDataVector<DATA_DIMENSION>& _lhs, const NURBSDataVector<DATA_DIMENSION>& _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs.data[i] - _rhs.data[i];
    return v;
}

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator*(const NURBSDataVector<DATA_DIMENSION>& _lhs, const NURBSDataVector<DATA_DIMENSION>& _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs.data[i] * _rhs.data[i];
    return v;
}

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator/(const NURBSDataVector<DATA_DIMENSION>& _lhs, const NURBSDataVector<DATA_DIMENSION>& _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs.data[i] / _rhs.data[i];
    return v;
}

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator*(const NURBSDataVector<DATA_DIMENSION>& _lhs, float _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs.data[i] * _rhs;
    return v;
}

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator*(float _lhs, const NURBSDataVector<DATA_DIMENSION>& _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs * _rhs.data[i];
    return v;
}

template<uint_t DATA_DIMENSION>
NURBSDataVector<DATA_DIMENSION> operator/(const NURBSDataVector<DATA_DIMENSION>& _lhs, float _rhs)
{
    NURBSDataVector<DATA_DIMENSION> v;
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
        v.data[i] = _lhs.data[i] / _rhs;
    return v;
}

template<uint_t DATA_DIMENSION>
const uint_t NURBSDataVector<DATA_DIMENSION>::sDataDimension = DATA_DIMENSION;

template<uint_t DATA_DIMENSION>
std::ostream& operator<<(std::ostream& _os, const NURBSDataVector<DATA_DIMENSION>& _v)
{
    _os << "(";
    for(uint_t i = 0; i < DATA_DIMENSION; ++i)
    {
        _os << _v.data[i];
        if(i + 1 < DATA_DIMENSION)
            _os << ", ";
    }
    _os << ")";
    return _os;
}


} // Scene
} // ACGL

#endif // ACGL_SCENE_NURBSDATAVECTOR_HH

#endif
