#ifndef PHYSICS_HH_INCLUDED
#define PHYSICS_HH_INCLUDED

#include <ACGL/Base/Settings.hh>
#include <ACGL/Math/Math.hh>
#include <vector>

#include "entity.hh"

#include "extern/bullet/src/BulletDynamics/Dynamics/btRigidBody.h"
#include "extern/bullet/src/BulletDynamics/Dynamics/btDynamicsWorld.h"
#include "extern/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h"

#include "extern/bullet/src/BulletCollision/CollisionShapes/btSphereShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btBoxShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "extern/bullet/src/BulletCollision/CollisionShapes/btStaticPlaneShape.h"

#include "extern/bullet/src/BulletDynamics/ConstraintSolver/btConstraintSolver.h"
#include "extern/bullet/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"//YAY!

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
	void init();
	void takeUpdateStep(float timeDiff);
	void rollForward(glm::vec3 camPos, float strength);
	void rollLeft(glm::vec3 camPos, float strength);
	void rollRight(glm::vec3 camPos, float strength);
	void rollBack(glm::vec3 camPos, float strength);
	glm::vec3 getPos(int i);
	glm::mat4 getRotation(int i);
	void rollForward(glm::vec3 camPos);
	void addStaticGroundPlane();
	void addTerrain(int width, int length, float** heightData);
	void addPlayer(float rad, float x, float y, float z, float mass, unsigned indice);
	void addSphere(float rad, float x, float y, float z, float mass, unsigned indice);
	void addBox(float width, float height, float length, Entity entity, float mass, unsigned indice);

    private:
	btRigidBody* playerBody;
	btRigidBody* terrainBody;
	std::vector<btRigidBody*> bodies; //list of all bodies. bodies are also in world, but save again to ease cleaning up process.
        btRigidBody* staticGroundBody;


	btDynamicsWorld* world; //contains physical attributes of the world.
	btDispatcher* dispatcher; //
	btCollisionConfiguration* colConfig; //defines the type of collision detection.
	btBroadphaseInterface* broadphase; //defines how objects are culled from collision detection.
	btConstraintSolver* solver; //solver for forces and impulses.
};
#endif
