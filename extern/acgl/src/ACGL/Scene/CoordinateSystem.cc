/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifdef ACGL_INCLUDE_UNSUPPORTED

#include <ACGL/Scene/CoordinateSystem.hh>

using namespace ACGL;
using namespace ACGL::Scene;

template<int_t A, int_t B, int_t C>
void CoordinateSystem::adjust(const glm::vec3& position, const glm::vec3& forward)
{
    const int_t limit = 10;
    float absoluteDistance, outerMetric, tempMetric, centerAlong, centerOrthogonal, leftPos, rightPos, currentPos;
    glm::vec3 intersection, axis;

    absoluteDistance = Math::Functions::abs(position[C] );
    outerMetric = 0.001f;
    for(int_t i = 0; i < limit; ++i)
    {
        if(absoluteDistance > outerMetric)
            outerMetric *= 10.0f;
        else
            break;
    }

    mInnerMetric = outerMetric / 10.0f;
    mInterpolC = (absoluteDistance - mInnerMetric) / (mInnerMetric * 9);

    centerAlong = outerMetric * (int_t)(position[A] / outerMetric);
    centerOrthogonal = outerMetric * (int_t)(position[B] / outerMetric);
    currentPos = centerAlong - (outerMetric / 2.0f) * mOuterLines;
    leftPos  = centerOrthogonal + (outerMetric / 2.0f) * mOuterLines;
    rightPos = centerOrthogonal - (outerMetric / 2.0f) * mOuterLines;
    for(int_t i = 0; i < mOuterLines; ++i)
    {
        mOuterLinesCA[i * 2][A] = ( currentPos);
        mOuterLinesCA[i * 2][B] = ( leftPos);
        mOuterLinesCA[i * 2][C] = ( 0.0f);
        mOuterLinesCA[i * 2 + 1][A] = ( currentPos);
        mOuterLinesCA[i * 2 + 1][B] = ( rightPos);
        mOuterLinesCA[i * 2 + 1][C] = ( 0.0f);
        currentPos += outerMetric;
    }

    centerAlong = outerMetric * (int_t)(position[B] / outerMetric);
    centerOrthogonal = outerMetric * (int_t)(position[A] / outerMetric);
    currentPos = centerAlong - (outerMetric / 2.0f) * mOuterLines;
    leftPos  = centerOrthogonal + (outerMetric / 2.0f) * mOuterLines;
    rightPos = centerOrthogonal - (outerMetric / 2.0f) * mOuterLines;
    for(int_t i = 0; i < mOuterLines; ++i)
    {
        mOuterLinesCB[i * 2][A] = ( leftPos);
        mOuterLinesCB[i * 2][B] = ( currentPos);
        mOuterLinesCB[i * 2][C] = ( 0.0f);
        mOuterLinesCB[i * 2 + 1][A] = ( rightPos);
        mOuterLinesCB[i * 2 + 1][B] = ( currentPos);
        mOuterLinesCB[i * 2 + 1][C] = ( 0.0f);
        currentPos += outerMetric;
    }

    axis[C] = (1.0f);

    //Calculate intersection of view ray with ground plane
    float denominator = glm::dot(axis, forward);
    //Currently, we use the ground plane (distance 0.0f), but perhaps
    //we want to make it more dynamic in the future
    float planeDistance = 0.0f;

    if(Math::Functions::abs(denominator) < Math::Constants::EPSILON_FLOAT)
    {
        //Plane and Ray are parallel
        intersection = glm::vec3( position[A], 0.0f, position[B] );
    }
    else
    {
        float distance = -(glm::dot(axis, position) - planeDistance) / denominator;
        intersection = position + (distance * forward);
    }

    centerAlong = outerMetric * (int_t)(intersection[A] / outerMetric);
    centerOrthogonal = outerMetric * (int_t)(intersection[B] / outerMetric);
    currentPos = centerAlong - Math::Functions::sig(intersection[A]) * outerMetric * mInnerRange;
    tempMetric = Math::Functions::sig(intersection[A]) * mInnerMetric;
    leftPos  = centerOrthogonal + Math::Functions::sig(intersection[B]) * outerMetric * (mInnerRange + 1);
    rightPos = centerOrthogonal - Math::Functions::sig(intersection[B]) * outerMetric *  mInnerRange;
    for(int_t i = 0; i < mInnerLines; ++i)
    {
        mInnerLinesCA[i * 2][A] = ( currentPos);
        mInnerLinesCA[i * 2][B] = ( leftPos);
        mInnerLinesCA[i * 2][C] = ( 0.0f);
        mInnerLinesCA[i * 2 + 1][A] = ( currentPos);
        mInnerLinesCA[i * 2 + 1][B] = ( rightPos);
        mInnerLinesCA[i * 2 + 1][C] = ( 0.0f);
        currentPos += tempMetric;
    }

    centerAlong = outerMetric * (int_t)(intersection[B] / outerMetric);
    centerOrthogonal = outerMetric * (int_t)(intersection[A] / outerMetric);
    currentPos = centerAlong - Math::Functions::sig(intersection[B]) * outerMetric * mInnerRange;
    tempMetric = Math::Functions::sig(intersection[B]) * mInnerMetric;
    leftPos  = centerOrthogonal + Math::Functions::sig(intersection[A]) * outerMetric * (mInnerRange + 1);
    rightPos = centerOrthogonal - Math::Functions::sig(intersection[A]) * outerMetric *  mInnerRange;
    for(int_t i = 0; i < mInnerLines; ++i)
    {
        mInnerLinesCB[i * 2][A] = ( leftPos);
        mInnerLinesCB[i * 2][B] = ( currentPos);
        mInnerLinesCB[i * 2][C] = ( 0.0f);
        mInnerLinesCB[i * 2 + 1][A] = ( rightPos);
        mInnerLinesCB[i * 2 + 1][B] = ( currentPos);
        mInnerLinesCB[i * 2 + 1][C] = ( 0.0f);
        currentPos += tempMetric;
    }
}

template void CoordinateSystem::adjust<0,2,1>(const glm::vec3&, const glm::vec3&);
template void CoordinateSystem::adjust<1,0,2>(const glm::vec3&, const glm::vec3&);
template void CoordinateSystem::adjust<1,2,0>(const glm::vec3&, const glm::vec3&);

#endif
