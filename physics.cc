#include "physics.hh"

#include <vector>


btDynamicsWorld* world; //contains physical attributes of the world.
btDispatcher* dispatcher; //
btCollisionConfiguration* colConfig; //defines the type of collision detection.
btBroadphaseInterface* broadphase; //defines how objects are culled from collision detection.
btConstraintSolver* solver; //solver for forces and impulses.

std::vector<btRigidBody*> bodies; //list of all bodies. bodies are also in world, but save again to ease cleaning up process.

void init()
{
	colConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(colConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,colConfig);
	
	world->setGravity(btVector3(0,-10,-0));

	
}

void takeUpdateStep(float timeDiff)
{
	world->stepSimulation(timeDiff);
}

void addSphere(float rad, float x, float y, float z, float mass)
{
	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0,0,0);
	if(mass == 0.0)
	{
	}
	else
	{
		sphere->calculateLocalInertia((btScalar)mass,inertia);
	}

	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));

	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);

	btRigidBody* body = new btRigidBody(info);

	world->addRigidBody(body);

	bodies.push_back(body);
}

void getPos(int i)
{
	btVector3 origin = bodies[i]->getCenterOfMassPosition();
}

void getRotation(int i)
{
	btQuaternion rotQuantrino = bodies[i]->getOrientation(); //TODO return orientation in gl format
}

void rollForward()
{
	//bodies[k].applyTorque(btVector3);
}

/*
void kill()
{
	//btDynamimcWorld*
	for(int i = 0; i < bodies.size();i++)
	{
		world->removeCollisionObject(bodies[i]); //clarification: go through the list of bodies in wordl for each body b, then remove exactly this body b from world
		btMotionState* motionState = bodies[i]->getMotionState();
		btCollisionShape* shape = bodies[i]->getCollisionShape();
		delete shape;
		delete motionState;
		delete bodies[i];
	}

	delete dispatcher;
	delete colConfig;
	delete solver;
}
	delete broadphase;
	delete world;
}
*/

