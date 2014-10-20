/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

// See http://www.graphics.rwth-aachen.de/redmine/projects/acgl/wiki/ACGL_compile_time_settings
// for a comment what unsupported means.
#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_SCENE_NURBSCURVE_HH
#define ACGL_SCENE_NURBSCURVE_HH

#include <ACGL/ACGL.hh>
#include <ACGL/Scene/NURBSControlPoint.hh>
#include <ACGL/Utils/Log.hh>
#include <ACGL/Math/Math.hh>

#include <vector>

namespace ACGL{
namespace Scene{

template<uint_t DATA_DIMENSION>
class NURBSCurve
{
public:
    typedef NURBSControlPoint<DATA_DIMENSION> NURBSControlPointType;

    NURBSCurve(
        const std::vector<NURBSControlPointType>& _controlPoints,
        uint_t _degree = 3)
    :   mDegree(_degree),
        mOrder(_degree + 1),
        mControlPoints(_controlPoints),
        mKnots()
    {
        //First knot with #order multiplicity
        mKnots.resize(mControlPoints.size() + mOrder, 0.0f);
        //Inner knots
        float fCurrentKnot = 1.0f;
        for(uint_t i = mOrder; i < mKnots.size()-mOrder; ++i)
        {
            mKnots[i] = fCurrentKnot;
            fCurrentKnot += 1.0f;
        }
        //Last knot with #order multiplicity
        for(uint_t i = mKnots.size() - mOrder; i < mKnots.size(); ++i)
            mKnots[i] = fCurrentKnot;
        normalizeKnotVector();
    }
    NURBSCurve(
        const std::vector<NURBSControlPointType>& _controlPoints,
        const std::vector<float>& _knots,
        uint_t _degree = 3)
    :   mDegree(_degree),
        mOrder(_degree + 1),
        mControlPoints(_controlPoints),
        mKnots()
    {
        if(mControlPoints.size() == _knots.size() + mOrder)
        {
            mKnots = _knots;
        }
        else
        {
            ACGL::Utils::error() << "Trying to create NURBS curve with a wrong number of knots... automatically generating a valid knot vector instead!" << std::endl;
            //First knot with #order multiplicity
            mKnots.resize(mControlPoints.size() + mOrder, 0.0f);
            //Inner knots
            float fCurrentKnot = 1.0f;
            for(uint_t i = mOrder; i < mKnots.size()-mOrder; ++i)
            {
                mKnots[i] = fCurrentKnot;
                fCurrentKnot += 1.0f;
            }
            //Last knot with #order multiplicity
            for(uint_t i = mKnots.size() - mOrder; i < mKnots.size(); ++i)
                mKnots[i] = fCurrentKnot;
            normalizeKnotVector();
        }
    }
    virtual ~NURBSCurve(void){}

    uint_t getDegree (void) const { return mDegree;   }
    uint_t getOrder  (void) const { return mOrder;    }

    const std::vector<NURBSControlPointType>& getControlPoints (void) const { return mControlPoints; }
    const std::vector<float>&                 getKnots         (void) const { return mKnots;         }

    NURBSControlPointType& getControlPointAt (typename std::vector<NURBSControlPointType>::size_type _i) { return mControlPoints[_i]; }
    float&                 getKnotAt         (typename std::vector<float>::size_type                 _i) { return mKnots[_i];         }

    // http://www.cs.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/NURBS/NURBS-knot-insert.html
    void insertKnot(float _t)
    {
        uint_t interval = getIntervalOf(_t);

        //Apply the weight of all NURBS control points and
        //reverse the order in the span for easier indices
        //in the next step.
        std::vector<NURBSControlPointType> controlPointsInSpan(mOrder);
        for(uint_t i = 0; i < mOrder; ++i)
        {
            controlPointsInSpan[i] = mControlPoints[interval - i];
            controlPointsInSpan[i].data *= controlPointsInSpan[i].weight;
        }

        std::vector<NURBSControlPointType> newControlPoints(mDegree);
        for(uint_t i = 0; i < mDegree; ++i)
        {
            float fRatio = (_t - mKnots[interval - i]) / (mKnots[interval + mDegree - i] - mKnots[interval - i]);
            newControlPoints[i] = controlPointsInSpan[i + 1] * (1.0f - fRatio) + controlPointsInSpan[i] * fRatio;
            newControlPoints[i].data /= newControlPoints[i].weight;
        }

        mKnots.insert(mKnots.begin() + (interval + 1), _t);
        mControlPoints.insert(mControlPoints.begin() + interval, newControlPoints[0]);
        for(uint_t i = 1; i < mDegree; ++i)
            mControlPoints[interval - i] = newControlPoints[i];
    }

    NURBSControlPointType sampleAt(float _t) const
    {
        uint_t interval = getIntervalOf(_t);

        //Apply the weight of all NURBS control points and
        //reverse the order in the span to easier indices
        //in the next step.
        std::vector< std::vector<NURBSControlPointType> > controlPointsInSpan;
        for(uint_t i = 0; i < mOrder; ++i)
            controlPointsInSpan.push_back(std::vector<NURBSControlPointType>());
        for(uint_t i = 0; i < mOrder; ++i)
        {
            NURBSControlPointType controlPoint = mControlPoints[interval - i];
            controlPoint.data *= controlPoint.weight;
            controlPointsInSpan[mDegree].push_back(controlPoint);
        }

        for(uint_t l = 0; l < mDegree; ++l)
        {
            for(uint_t i = 0; i < mDegree - l; ++i)
            {
                float fRatio = (_t - mKnots[interval - i]) / (mKnots[interval + mDegree - l - i] - mKnots[interval - i]);
                //Please note: Due to the reversed order we have:
                //cControlPointsInSpan[i + 1] = m_cControlPoints[interval - i - 1]
                //cControlPointsInSpan[i]     = m_cControlPoints[interval - i]
                controlPointsInSpan[mDegree - l - 1].push_back((1.0f - fRatio) * controlPointsInSpan[mDegree - l][i + 1] + fRatio * controlPointsInSpan[mDegree - l][i]);
            }
        }

        controlPointsInSpan[0][0].data /= controlPointsInSpan[0][0].weight;
        return controlPointsInSpan[0][0];
    }

    std::vector<NURBSControlPointType> sampleAt(float _t, uint_t _derivatives) const
    {
        // The first entry of sampleAndDerivatives will be the sample itself. The
        // others are the derivatives. Therefore, we have _derivatives + 1 result
        // values.
        _derivatives = Math::Functions::min(mDegree, _derivatives);
        uint_t results = _derivatives + 1;
        std::vector<NURBSControlPointType> sampleAndDerivatives(results);

        uint_t interval = getIntervalOf(_t);

        // Apply the weight of all NURBS control points and
        // reverse the order in the span to easier indices
        // in the next step.
        std::vector< std::vector<NURBSControlPointType> > controlPointsInSpan;
        for(uint_t i = 0; i < mOrder; ++i)
            controlPointsInSpan.push_back(std::vector<NURBSControlPointType>());
        for(uint_t i = 0; i < mOrder; ++i)
        {
            NURBSControlPointType controlPoint = mControlPoints[interval - i];
            controlPoint.data *= controlPoint.weight;
            controlPointsInSpan[mDegree].push_back(controlPoint);
        }

        for(uint_t l = 0; l < mDegree; ++l)
        {
            for(uint_t i = 0; i < mDegree - l; ++i)
            {
                float fRatio = (_t - mKnots[interval - i]) / (mKnots[interval + mDegree - l - i] - mKnots[interval - i]);
                //Please note: Due to the reversed order we have:
                //cControlPointsInSpan[i + 1] = m_cControlPoints[interval - i - 1]
                //cControlPointsInSpan[i]     = m_cControlPoints[interval - i]
                controlPointsInSpan[mDegree - l - 1].push_back((1.0f - fRatio) * controlPointsInSpan[mDegree - l][i + 1] + fRatio * controlPointsInSpan[mDegree - l][i]);
            }
        }

        // Add the sample as first return value
        controlPointsInSpan[0][0].data /= controlPointsInSpan[0][0].weight;
        controlPointsInSpan[0][0].weight = 1.0f;
        sampleAndDerivatives[0] = controlPointsInSpan[0][0];

        // Calculate the derivatives
        std::vector<int_t> factors(_derivatives + 1, 1);
        for(uint_t i = 0; i < _derivatives; ++i)
        {
            for(uint_t j = i; j > 0; --j)
            {
                factors[j] = factors[j] + factors[j - 1];
            }
            controlPointsInSpan[i+1][0].data /= controlPointsInSpan[i+1][0].weight;
            controlPointsInSpan[i+1][0].weight = 1.0f;
            sampleAndDerivatives[i+1] = controlPointsInSpan[i+1][0];
            for(uint_t k = 1; k < i+2; ++k)
            {
                controlPointsInSpan[i+1][k].data /= controlPointsInSpan[i+1][k].weight;
                controlPointsInSpan[i+1][k].weight = 1.0f;
                sampleAndDerivatives[i+1] += (k%2==1?-factors[k]:factors[k]) * controlPointsInSpan[i+1][k];
            }
        }

        return sampleAndDerivatives;
    }

    NURBSCurve getSubCurveAt(float _t) const
    {
        uint_t interval = getIntervalOf(_t);

        // Get the subcurve, where _t lies in
        std::vector<NURBSControlPointType> controlPointsInSpan(mOrder);
        for(uint_t i = 0; i < mOrder; ++i)
            controlPointsInSpan[mDegree - i] = mControlPoints[interval - i];
        // Knots = #ControlPoints (here mOrder) + mDegree + 1 = 2 * mOrder
        std::vector<float> knotsInSpan(mOrder * 2);
        for(uint_t i = 0; i < mOrder * 2; ++i)
            knotsInSpan[mDegree + mOrder - i] = mKnots[interval + mOrder - i];

        return NURBSCurve(controlPointsInSpan, knotsInSpan, mDegree);
    }

    //This function takes a knot vector and normalizes
    //it to [0,1] within the region that can be evaluated,
    //namely between the knots (degree) and (#knots - degree - 1)
    void normalizeKnotVector(void)
    {
        float offset = mKnots[mDegree];
        float span = mKnots[mKnots.size() - mOrder] - offset;
        for(std::vector<float>::size_type i = 0; i < mKnots.size(); ++i)
        {
            mKnots[i] -= offset;
            mKnots[i] /= span;
        }
    }

private:
    uint_t getIntervalOf(float _t) const
    {
        //Find the interval that _t lies in
        uint_t interval = mDegree;
        uint_t start = mOrder;
        uint_t end = mKnots.size() - mOrder;

        for(uint_t i = start; i < end; ++i)
        {
            if(mKnots[i] <= _t)
                interval++;
            else
                break;
        }

        return interval;
    }

    uint_t mDegree;
    uint_t mOrder;
    std::vector<NURBSControlPointType> mControlPoints;
    std::vector<float> mKnots;
};

template<uint_t DATA_DIMENSION>
std::ostream& operator<<(std::ostream& _os, const NURBSCurve<DATA_DIMENSION>& _curve)
{
    _os << "(Degree: " << _curve.getDegree() << ", Order: " << _curve.getOrder() << ", ";
    _os << "ControlPoints: (";
    for(typename std::vector<typename NURBSCurve<DATA_DIMENSION>::NURBSControlPointType>::size_type i = 0; i < _curve.getControlPoints().size(); ++i)
    {
        _os << _curve.getControlPoints()[i];
        if(i + 1 < _curve.getControlPoints().size())
            _os << ", ";
    }
    _os << "), ";
    _os << "Knots: (";
    for(std::vector<float>::size_type i = 0; i < _curve.getKnots().size(); ++i)
    {
        _os << _curve.getKnots()[i];
        if(i + 1 < _curve.getKnots().size())
            _os << ", ";
    }
    _os << ")";
    return _os;
}

} // Scene
} // ACGL

#endif // ACGL_SCENE_NURBSCURVE_HH

#endif
