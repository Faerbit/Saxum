/***********************************************************************
 * Copyright 2011-2012 Computer Graphics Group RWTH Aachen University. *
 * All rights reserved.                                                *
 * Distributed under the terms of the MIT License (see LICENSE.TXT).   *
 **********************************************************************/

#ifdef ACGL_INCLUDE_UNSUPPORTED

#ifndef ACGL_SCENE_COORDINATESYSTEM_HH
#define ACGL_SCENE_COORDINATESYSTEM_HH

/*
 * The coordinate system can be used to draw a simple grid to visualize the
 * worlds coordinate system. Drawing itself is not implemented here!
 */

#include <ACGL/ACGL.hh>
#include <ACGL/Math/Math.hh>

#include <vector>

namespace ACGL{
namespace Scene{

class CoordinateSystem
{
public:
    CoordinateSystem(int_t outerLines = 100, int_t innerRange = 2)
    :   mOuterLines(outerLines),
        mInnerRange(innerRange),
        mInnerLines((innerRange * 2 + 1) * 10),
        mInterpolC(0.0f),
        mOuterLinesCA(mOuterLines * 2),
        mOuterLinesCB(mOuterLines * 2),
        mInnerLinesCA(mInnerLines * 2),
        mInnerLinesCB(mInnerLines * 2)
    {}
    ~CoordinateSystem() {}

    /*! Adjust the grid concerning the given absolut position */
    template<int_t A, int_t B, int_t C>
    void adjust(const glm::vec3& pos, const glm::vec3& forward);

    inline int_t getOuterLines() const { return mOuterLines; }
    inline int_t getInnerLines() const { return mInnerLines; }
    inline float getInterpolC()  const { return mInterpolC; }

    inline const std::vector<glm::vec3>& getOuterLinesCA() const { return mOuterLinesCA; }
    inline const std::vector<glm::vec3>& getOuterLinesCB() const { return mOuterLinesCB; }
    inline const std::vector<glm::vec3>& getInnerLinesCA() const { return mInnerLinesCA; }
    inline const std::vector<glm::vec3>& getInnerLinesCB() const { return mInnerLinesCB; }

    inline float getInnerMetric() { return mInnerMetric; }

private:
    int_t                  mOuterLines;
    int_t                  mInnerRange;
    int_t                  mInnerLines;
    float                  mInterpolC;
    std::vector<glm::vec3> mOuterLinesCA;
    std::vector<glm::vec3> mOuterLinesCB;
    std::vector<glm::vec3> mInnerLinesCA;
    std::vector<glm::vec3> mInnerLinesCB;
    float                  mInnerMetric;
};

} // Scene
} // ACGL

#endif // ACGL_SCENE_COORDINATESYSTEM_HH

#endif
