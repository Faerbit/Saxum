#ifndef PHYSICS_HH_INCLUDED
#define PHYSICS_HH_INCLUDED

#include <ACGL/Base/Settings.hh>
#include <ACGL/Math/Math.hh>
#include <vector>
#include <string>
#include <stdio.h>

extern "C" {
#include "extern/lua/src/lua.h"
#include "extern/lua/src/lualib.h"
#include "extern/lua/src/lauxlib.h"
}
#include "extern/luabridge/LuaBridge.h"

#include "entity.hh"

#include "extern/bullet/src/BulletDynamics/Dynamics/btRigidBody.h"
#include "extern/bullet/src/BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "extern/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"

#include "extern/bullet/src/BulletCollision/CollisionShapes/btSphereShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btBoxShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btStaticPlaneShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btTriangleMesh.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btCollisionShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h"

#include "extern/bullet/src/BulletDynamics/ConstraintSolver/btConstraintSolver.h"
#include "extern/bullet/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"//YAY!
#include "extern/bullet/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"

#include "extern/bullet/src/BulletCollision/CollisionDispatch/btCollisionConfiguration.h"
#include "extern/bullet/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h"

#include "extern/bullet/src/BulletCollision/BroadphaseCollision/btBroadphaseInterface.h"
#include "extern/bullet/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"
#include "extern/bullet/src/BulletCollision/BroadphaseCollision/btDispatcher.h"

#include "extern/bullet/src/LinearMath/btScalar.h"
#include "extern/bullet/src/LinearMath/btMotionState.h"
#include "extern/bullet/src/LinearMath/btDefaultMotionState.h"
#include "extern/bullet/src/LinearMath/btQuaternion.h"
#include "extern/bullet/src/LinearMath/btVector3.h"
#include "extern/bullet/src/LinearMath/btMatrix3x3.h"

class Physics {
    public:
    Physics();
	~Physics();
	void init(lua_State* L);
	void takeUpdateStep(float timeDiff); //must be used in level.update to proagate the physics
	void rollForward(glm::vec3 camPos, float strength); //self explainitory
	void rollLeft(glm::vec3 camPos, float strength);
	void rollRight(glm::vec3 camPos, float strength);
	void rollBack(glm::vec3 camPos, float strength);
	glm::vec3 getPos(int i);
	glm::mat4 getRotation(int i);
	void addStaticGroundPlane();
	void addCamera(float rad, float distance); //Do NOT impliment before Player has been created;
	glm::vec3 getCameraPosition();
	void addRigidBodyFromFile(Entity entity, float mass, float dampningL, float dampningA, std::string modelLocation, unsigned indice);
    void addTriangleMeshBody(Entity entity, std::string path, float mass, float dampningL, float dampningA, unsigned indice);
	void addTerrain(int width, int length, float** heightData);
    void addTerrainTriangles(int width, int length, float** heightData); //add the terrain as a trimesh instead of a heightmap
	void addPlayer(float friction, float rad, Entity entity, float mass, float dampningL, float dampningA, unsigned indice); //use these AFTER physicObjects.push_back(object)! if mass == 0 then the object is unmoveable
	void addSphere(float rad, Entity entity, float mass, float dampningL, float dampningA, unsigned indice); //The Indice should be set to physicObjects.size()
	void addBox(float width, float height, float length, Entity entity, float mass, float dampningL, float dampningA, unsigned indice); //this is used to ensuer that the system is synchronized
	void addPositionConstraint(int bodyIndice, float strength, glm::vec3 position);
	void removePositionConstraint(int bodyIndice);

    struct positionConstraint{btRigidBody* body; float strength; btVector3 position;};

    private:
	btRigidBody* playerBall;    //allows for easier access to the ball
	btRigidBody* terrainBody;   //duh
	btRigidBody* cameraBody;
	std::vector<btRigidBody*> bodies; //list of all bodies. bodies are also in world, but save again to ease cleaning up process.
        btRigidBody* staticGroundBody;
    std::vector<positionConstraint> allPositionConstraints;
    
	btDynamicsWorld* world; //contains physical attributes of the world.
	btDispatcher* dispatcher; //
	btCollisionConfiguration* colConfig; //defines the type of collision detection.
	btBroadphaseInterface* broadphase; //defines how objects are culled from collision detection.
	btConstraintSolver* solver; //solver for forces and impulses.
};

enum collisionTypes{
    COL_NOTHING = 0,
    COL_TERRAIN = 1,
    COL_OBJECTS = 2,
    COL_OBJECTS_NO_TERRAIN = 4
};

class btDistanceConstraint : public btPoint2PointConstraint
{
protected:
   btScalar   m_distance;
public:
   btDistanceConstraint(btRigidBody& rbA,btRigidBody& rbB, const btVector3& pivotInA,const btVector3& pivotInB, btScalar dist)
      : btPoint2PointConstraint(rbA, rbB, pivotInA, pivotInB)
   {
      m_distance = dist;
   }
   virtual void getInfo1 (btConstraintInfo1* info)
   {
      info->m_numConstraintRows = 1;
      info->nub = 5;
   }
   virtual void getInfo2 (btConstraintInfo2* info)
   {
      btVector3 relA = m_rbA.getCenterOfMassTransform().getBasis() * getPivotInA();
      btVector3 relB = m_rbB.getCenterOfMassTransform().getBasis() * getPivotInB();
      btVector3 posA = m_rbA.getCenterOfMassTransform().getOrigin() + relA;
      btVector3 posB = m_rbB.getCenterOfMassTransform().getOrigin() + relB;
      btVector3 del = posB - posA;
      btScalar currDist = btSqrt(del.dot(del));
      btVector3 ortho = del / currDist;
      info->m_J1linearAxis[0] = ortho[0];
      info->m_J1linearAxis[1] = ortho[1];
      info->m_J1linearAxis[2] = ortho[2];
      btVector3 p, q;
      p = relA.cross(ortho);
      q = relB.cross(ortho);
      info->m_J1angularAxis[0] = p[0];
      info->m_J1angularAxis[1] = p[1];
      info->m_J1angularAxis[2] = p[2];
      info->m_J2angularAxis[0] = -q[0];
      info->m_J2angularAxis[1] = -q[1];
      info->m_J2angularAxis[2] = -q[2];
      btScalar rhs = (currDist - m_distance) * info->fps * info->erp;
      info->m_constraintError[0] = rhs;
      info->cfm[0] = btScalar(0.f);
      info->m_lowerLimit[0] = -SIMD_INFINITY;
      info->m_upperLimit[0] = SIMD_INFINITY;
   }
};
#endif
