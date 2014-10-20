/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_SCENE_NURBSCONTROLPOINT_HH
#define ACGL_SCENE_NURBSCONTROLPOINT_HH

#include <ACGL/ACGL.hh>
#include <ACGL/Scene/NURBSDataVector.hh>

namespace ACGL{
namespace Scene{

template<uint_t DATA_DIMENSION>
class NURBSControlPoint
{
public:
    typedef NURBSDataVector<DATA_DIMENSION> NURBSDataVectorType;

    NURBSControlPoint(void)
    :   data(),
        weight(1.0f)
    {}
    NURBSControlPoint(
        const NURBSDataVectorType& _data,
        float _weight = 1.0f)
    :   data(_data),
        weight(_weight)
    {}
    virtual ~NURBSControlPoint(void) {}


    const NURBSControlPoint& operator+=(const NURBSControlPoint<DATA_DIMENSION>& _c)
    {
        data += _c.data;
        weight += _c.weight;
        return *this;
    }

    const NURBSControlPoint& operator-=(const NURBSControlPoint<DATA_DIMENSION>& _c)
    {
        data -= _c.data;
        weight -= _c.weight;
        return *this;
    }

    const NURBSControlPoint& operator*=(const NURBSControlPoint<DATA_DIMENSION>& _c)
    {
        data *= _c.data;
        weight *= _c.weight;
        return *this;
    }

    const NURBSControlPoint& operator/=(const NURBSControlPoint<DATA_DIMENSION>& _c)
    {
        data /= _c.data;
        weight /= _c.weight;
        return *this;
    }

    const NURBSControlPoint& operator/=(float _value)
    {
        data /= _value;
        weight /= _value;
        return *this;
    }

    const NURBSControlPoint& operator*=(float _value)
    {
        data *= _value;
        weight *= _value;
        return *this;
    }

    NURBSDataVectorType data;
    float weight;
};

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator+(const NURBSControlPoint<DATA_DIMENSION>& _lhs, const NURBSControlPoint<DATA_DIMENSION>& _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs.data + _rhs.data;
    c.weight = _lhs.weight + _rhs.weight;
    return c;
}

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator-(const NURBSControlPoint<DATA_DIMENSION>& _lhs, const NURBSControlPoint<DATA_DIMENSION>& _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs.data - _rhs.data;
    c.weight = _lhs.weight - _rhs.weight;
    return c;
}

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator*(const NURBSControlPoint<DATA_DIMENSION>& _lhs, const NURBSControlPoint<DATA_DIMENSION>& _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs.data * _rhs.data;
    c.weight = _lhs.weight * _rhs.weight;
    return c;
}

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator/(const NURBSControlPoint<DATA_DIMENSION>& _lhs, const NURBSControlPoint<DATA_DIMENSION>& _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs.data / _rhs.data;
    c.weight = _lhs.weight / _rhs.weight;
    return c;
}

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator*(const NURBSControlPoint<DATA_DIMENSION>& _lhs, float _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs.data * _rhs;
    c.weight = _lhs.weight * _rhs;
    return c;
}

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator*(float _lhs, const NURBSControlPoint<DATA_DIMENSION>& _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs * _rhs.data;
    c.weight = _lhs * _rhs.weight;
    return c;
}

template<uint_t DATA_DIMENSION>
NURBSControlPoint<DATA_DIMENSION> operator/(const NURBSControlPoint<DATA_DIMENSION>& _lhs, float _rhs)
{
    NURBSControlPoint<DATA_DIMENSION> c;
    c.data = _lhs.data / _rhs;
    c.weight = _lhs.weight / _rhs;
    return c;
}

template<uint_t DATA_DIMENSION>
std::ostream& operator<<(std::ostream& _os, const NURBSControlPoint<DATA_DIMENSION>& _controlPoint)
{
    _os << "(Data: " << _controlPoint.data << ", Weight: " << _controlPoint.weight << ")";
    return _os;
}

} // Scene
} // ACGL

#endif // ACGL_SCENE_NURBSCONTROLPOINT_HH

#endif
