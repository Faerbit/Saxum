#include "debugDraw.hh"

DebugDraw::DebugDraw() : debug_mode(0) {
    data = std::vector<float>();
}

void DebugDraw::setDebugMode(int debugMode) {
    this->debug_mode = debugMode;
}

int DebugDraw::getDebugMode() const {
    return debug_mode;
}

std::vector<float>* DebugDraw::getData() {
    return &data;
}

void DebugDraw::drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    data.push_back(from.getX());
    data.push_back(from.getY());
    data.push_back(from.getZ());
    data.push_back(color.getX());
    data.push_back(color.getY());
    data.push_back(color.getZ());
    data.push_back(to.getX());
    data.push_back(to.getY());
    data.push_back(to.getZ());
    data.push_back(color.getX());
    data.push_back(color.getY());
    data.push_back(color.getZ());
}

void DebugDraw::drawLine (const btVector3 &from, const btVector3 &to, const btVector3 &fromColor,
    const btVector3 &toColor) {
    data.push_back(from.getX());
    data.push_back(from.getY());
    data.push_back(from.getZ());
    data.push_back(fromColor.getX());
    data.push_back(fromColor.getY());
    data.push_back(fromColor.getZ());
    data.push_back(to.getX());
    data.push_back(to.getY());
    data.push_back(to.getZ());
    data.push_back(toColor.getX());
    data.push_back(toColor.getY());
    data.push_back(toColor.getZ());
}


void DebugDraw::drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, 
        btScalar distance, int lifeTime, const btVector3 &color){
}

void DebugDraw::reportErrorWarning (const char *warningString) {
    printf("%s\n", warningString);
}

void DebugDraw::draw3dText (const btVector3 &location, const char *textString) {
}

void DebugDraw::clearData() {
    this->data.clear();
}
