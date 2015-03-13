#pragma once

#include "LinearMath/btIDebugDraw.h"
#include <ACGL/OpenGL/Objects.hh>

class DebugDraw : public btIDebugDraw {
    int debug_mode;
    public:
        DebugDraw();
        void setDebugMode(int debugMode);
        int getDebugMode() const;
        std::vector<float>* getData();
        void drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color);
        void drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &fromColor,
                const btVector3 &toColor);
        void reportErrorWarning (const char *warningString);
        void clearData();
        // the following functions don't do anything but are defined to 
        // get the class not to be abstract
        void drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, 
            btScalar distance, int lifeTime, const btVector3 &color);
        void draw3dText (const btVector3 &location, const char *textString);
    private:
        std::vector<float> data;
};
