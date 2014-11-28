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
	void takeUpdateStep(float timeDiff); //must be used in level.update to proagate the physics
	void rollForward(glm::vec3 camPos, float strength); //self explainitory
	void rollLeft(glm::vec3 camPos, float strength);
	void rollRight(glm::vec3 camPos, float strength);
	void rollBack(glm::vec3 camPos, float strength);
	glm::vec3 getPos(int i);
	glm::mat4 getRotation(int i);
	void addStaticGroundPlane();
	void addTerrain(int width, int length, float** heightData);
	void addPlayer(float rad, Entity entity, float mass, unsigned indice); //use these AFTER physicObjects.push_back(object)! if mass == 0 then the object is unmoveable
	void addSphere(float rad, Entity entity, float mass, unsigned indice); //The Indice should be set to physicObjects.size()
	void addBox(float width, float height, float length, Entity entity, float mass, unsigned indice); //this is used to ensuer that the system is synchronized

    private:
	btRigidBody* playerBall;    //allows for quicker access to the ball
	btRigidBody* terrainBody;   //duh
	std::vector<btRigidBody*> bodies; //list of all bodies. bodies are also in world, but save again to ease cleaning up process.
        btRigidBody* staticGroundBody;


	btDynamicsWorld* world; //contains physical attributes of the world.
	btDispatcher* dispatcher; //
	btCollisionConfiguration* colConfig; //defines the type of collision detection.
	btBroadphaseInterface* broadphase; //defines how objects are culled from collision detection.
	btConstraintSolver* solver; //solver for forces and impulses.
};
#endif
