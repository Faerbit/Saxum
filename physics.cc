#include "physics.hh"


btDynamicsWorld* world; //contains physical attributes of the world.
btDispatcher* dispatcher; //
btCollisionConfiguration* colConfig; //defines the type of collision detection.
btBroadphaseInterface* broadphase; //defines how objects are culled from collision detection.
btConstraintSolver* solver; //solver for forces and impulses.

std::vector<btRigidBody*> bodies; //list of all bodies. bodies are also in world, but save again to ease cleaning up process.
btRigidBody* playerBall;
btRigidBody* terrainBody;
btRigidBody* staticGroundBody;


Physics::Physics() {
}

Physics::~Physics() {
}

void Physics::init()
{
	colConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(colConfig);
	broadphase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,colConfig);
	
	world->setGravity(btVector3(0,-10,-0));	
}

void Physics::takeUpdateStep(float timeDiff)
{
	world->stepSimulation(timeDiff);
}

void Physics::addTerrain(int width, int length, float** heightData)
{
	float* heightfield = new float[width * length];
	      int highest = -999999, j = 0, i = 0;
	      for (i = 0; i < width; i++)
		 for (j = 0; j < length; j++) {
		    heightfield[j*length+i] = heightData[i][j];
		    if (heightData[i][j] > highest)
		       highest = heightData[i][j];
		 }

	btHeightfieldTerrainShape* terrianShape = new btHeightfieldTerrainShape(width,length,heightData,highest,1,true,false);

	btRigidBody* tBody = new btRigidBody(0,new btDefaultMotionState(),terrianShape);

	tBody->getWorldTransform().setOrigin(btVector3(0,highest/2,0));

	//tBody->getWoorldTransform().setRotation(btQuaternion(0,0,0,1));

	terrainBody = tBody;

	world->addRigidBody(terrainBody);

/*
	terrianShape->setLocalScaling(btVector3(1,1,1));
	btCollisionShape* trimeshShape = terrianShape;
         
	float mass = 0.f;
	btTransform   startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0,highest/2,0));//not 100% sure maybe (0,0,0) or (0,-highest/2,0)

	btRigidBody* groundBody = localCreateRigidBody(mass, startTransform,trimeshShape);
	
	world->addRigidBody(terrainBody);
	*/

}

void Physics::addStaticGroundPlane()
{
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
        staticGroundBody = new btRigidBody(groundRigidBodyCI);

        world->addRigidBody(staticGroundBody);
}

void Physics::addPlayer(float rad, float x, float y, float z, float mass, unsigned indice)
{
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0,0,0);
	if(mass != 0.0)
	{
		sphere->calculateLocalInertia((btScalar)mass,inertia);
	}

	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));

	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);

	playerBall = new btRigidBody(info);

	world->addRigidBody(playerBall);

	bodies.push_back(playerBall);

	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

}


void Physics::addSphere(float rad, float x, float y, float z, float mass, unsigned indice)
{
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0,0,0);
	if(mass != 0.0)
	{
		sphere->calculateLocalInertia((btScalar)mass,inertia);
	}

	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(x,y,z)));

	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);

	btRigidBody* body = new btRigidBody(info);

	world->addRigidBody(body);

	bodies.push_back(body);


	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

}

glm::vec3 Physics::getPos(int i)
{
	btVector3 origin = bodies[i]->getCenterOfMassPosition();
	glm::vec3 save(origin.getX(),origin.getY(),origin.getZ());
	return save;
}

glm::mat4 Physics::getRotation(int i)
{
	btQuaternion quat = bodies[i]->getOrientation();

	glm::mat4 matrix = glm::rotate(
	    quat.getAngle(),
	    glm::vec3(quat.getAxis().getX(), quat.getAxis().getY(), quat.getAxis().getZ())
	);
	return matrix;
}

void Physics::rollForward(glm::vec3 camPos)
{
    btVector3 pos(camPos.x,camPos.y,camPos.z);
    pos -= playerBody->getCentreOfMassPosition();
    pos.cross(btVector3(0,1,0);
    playerBall->applyTorque(pos);

/*	glm::vec3 saveVector= glm::vec3(1,0,0) * rotCamera;
	saveVector = glm::cross(glm::vec3(0,1,0),saveVector);
	playerBall->applyTorque(btVector3(saveVector[0],saveVector[1],saveVector[2]));*/
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

