#include "physics.hh"
#include "extern/bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"


Physics::Physics() {
}

Physics::~Physics() {
}

void Physics::init() //prepares bullet by creating all initial classes
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
	world->stepSimulation(timeDiff); //allows the world to be simmulated correctly indipendant of the timedifferences between frames
	for(unsigned i = 0; i < allPositionConstraints.size();i++) //this handles the spring constraints
	{
	    if(allPositionConstraints[i].position != allPositionConstraints[i].body->getCenterOfMassPosition()) //if constraint != position of the body because otherwise dir = 0
	    {
	        btVector3 dir = allPositionConstraints[i].position - allPositionConstraints[i].body->getCenterOfMassPosition();
	        allPositionConstraints[i].body->applyCentralForce(dir*allPositionConstraints[i].strength); //apply a foce upon the object pushing it towards the constraint position
	    }	
	}
	
	btVector3 position = cameraBody->getCenterOfMassPosition() - playerBall->getCenterOfMassPosition(); //gets a vector from the player to the camera
	position.normalize();
	position *=5;
	position += playerBall->getCenterOfMassPosition(); //is the position 5 units away from the player in the direction of the camera
	 
	 
	btVector3 dir = cameraBody->getCenterOfMassPosition() - position;
	cameraBody->applyCentralForce(dir*cameraBody->getInvMass());//scale the force by camera mass
	cameraBody->applyCentralForce(btVector3(0,1,0)); //applies a force to the camera keeping it in a correct position
	
}

void Physics::removePositionConstraint(int bodyIndice) //remover function for deleting all pos constraints on one body
{
	for(unsigned i = 0; i < allPositionConstraints.size(); i++)
	{
	    if(allPositionConstraints[i].body == bodies[bodyIndice])
	    {
	        allPositionConstraints.erase(allPositionConstraints.begin()+i);
	    }
	}
}

void Physics::addPositionConstraint(int bodyIndice, float strength, glm::vec3 position) //function for adding position constraints
{
    positionConstraint cons;
    cons.body = bodies[bodyIndice];
    cons.strength = strength;
    cons.position = btVector3(position.x,position.y,position.z);
    allPositionConstraints.push_back(cons);
}   

//players and objects
void Physics::addPlayer(float friction, float rad, Entity entity, float mass, float dampningL, float dampningA, unsigned indice)
{
	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" ); //these error are to ensure that level can always communicate with physics without having to worry about synching errors

	btSphereShape* sphere = new btSphereShape(rad); //the first thing we need for a rigid body is the shape
	btVector3 inertia(0,0,0);
	if(mass != 0.0)
	{
		sphere->calculateLocalInertia((btScalar)mass,inertia); //from this shape we can then calculate the innertia, as long as the mass != 0 (otherwise inertia = 0)
	}

	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z))); //next we define the motionstate, wich describes the innital position and rotation

	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia); //next we process all data for the rigid body into info

    info.m_friction = friction*2; //here we modify the friction and restitution (bounciness) of the object
    info.m_restitution = 0.2f;

	playerBall = new btRigidBody(info); //finally we create the rigid body using the info
    
    playerBall->setDamping(dampningL, dampningA); //here we can set the dampning (how much of the motion is lost)
    
	world->addRigidBody(playerBall,COL_OBJECTS,COL_OBJECTS|COL_OBJECTS_NO_TERRAIN|COL_TERRAIN); //then we add the rigid body to the wiorld, allowing it to be simulated

	bodies.push_back(playerBall); //next we add the rigid body to our own list (for cleanup and for synchronitaation with level)
    
    playerBall->setSleepingThresholds(0,0); //in a final step we make sure that the body never is removed from the active rigid bodies
    
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" );  //one last check to make sure level and physics are in synch
    
    addCamera(); //now that the player exists add a camera for the player
}

void Physics::addTerrain(int width, int length, float** heightData) //The terrian adding function
{

	float* heightfield = new float[width * length];//bullet only accepts data in a one dimensional array, so parse data into appropriate format
	      int highest = -999999, j = 0, i = 0;
	      for (i = 0; i < width; i++)
	      {
	    	 for (j = 0; j < length; j++) {
                heightfield[i*length+j] =  heightData[j][i]; //reverse order because they are loaded backwards
		  
                if (heightData[j][i] > highest)
                    highest = heightData[j][i]; //bullet needs to know the highest point of the heightmap
		 }
		 }
		 
		 highest++;

	btHeightfieldTerrainShape* terrianShape = new btHeightfieldTerrainShape(length,width,heightfield,highest,1,true,false); 

	btRigidBody* tBody = new btRigidBody(0,new btDefaultMotionState(),terrianShape);

	tBody->getWorldTransform().setOrigin(btVector3(0,((float)highest)/2,0)); //we have to move the origin of our rigid body down, because bullet sets the origin (0,0,0) at (width/2, height/2, length/2) in the map the x and z are correct in our level, but y needs to be addapted

	terrainBody = tBody;
    
	world->addRigidBody(terrainBody, COL_TERRAIN, COL_TERRAIN | COL_OBJECTS); //COL_XXXX are collision masks, allowing us to ignore collisions between certain object groups (required for buttons)
}

void Physics::addTriangleMeshBody(Entity entity, std::string path, float mass, float dampningL, float dampningA,unsigned indice,float scaling, bool rotate)
{
    
	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" );  
		
    std::vector< unsigned int > vertexIndices; //temp lists for data sets
    std::vector< btVector3 > temp_vertices;
    
    path = "../Levels/Geometry/" + path;
    FILE * file = fopen(path.c_str(), "r");
    if( file == NULL ){
        throw std::invalid_argument( "Impossible to open the file" ); //create correct filepath and report error if cannot open
    }

    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // while not at end do loop
        if ( strcmp( lineHeader, "v" ) == 0 ){ //if a vertex
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(btVector3(vertex.x,vertex.y,vertex.z)); //save vertex in array
        }
        else if ( strcmp( lineHeader, "f" ) == 0 ){ //if face (index for 3 vertexes for a triangle)
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]); //save 3 indexes in array
        }
    }
    //finally start making body        
    btTriangleMesh* triMesh = new btTriangleMesh();
    
    for(unsigned i = 2; i < vertexIndices.size();i+=3)
    {
        triMesh->addTriangle(temp_vertices[vertexIndices[i]],temp_vertices[vertexIndices[i-1]],temp_vertices[vertexIndices[i-2]]); // for every face (3 elements in vertexIndices) create triangle          
    }
    
    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triMesh,true);
    shape->setLocalScaling(btVector3(scaling,scaling,scaling)); //we need to add a scaling here because the objects seem to have diffrent sizes when loaded (no clue why, see composition.xml for exact scaling factors)
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z)));
	
    btVector3 inertia(0,0,0);	
	if(mass != 0.0 && rotate) //&& rotate lets certain objects get inertia (0,0,0) (not rotateable) 
	{
		shape->calculateLocalInertia((btScalar)mass,inertia);
	}
    
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,shape,inertia);
	
	btRigidBody* body = new btRigidBody(info);	
	
	body->setDamping(dampningL,dampningA);    
	
	bodies.push_back(body);
	
	world->addRigidBody(body,COL_OBJECTS, objectsPhysicsCollision);
    
    
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 
}

void Physics::addButton(float width, float height, float length, Entity entity, float mass, float dampningL, float dampningA, unsigned indice,bool rotate)
{
    
	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" );  
		
		btBoxShape* box = new btBoxShape(btVector3(width/2,height/2,length/2));	
		
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z))); 
	
	btVector3 inertia(0,0,0);
		
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,box,inertia);
	
	btRigidBody* body = new btRigidBody(info);	
	
    body->setDamping(dampningL, dampningA);
    
	world->addRigidBody(body,COL_OBJECTS_NO_TERRAIN, specialPhysicsCollision);
	
	bodies.push_back(body);
			
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 	
}

void Physics::addBox(float width, float height, float length, Entity entity, float mass, float dampningL, float dampningA, unsigned indice,bool rotate)
{
	
	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" );  
		
	btBoxShape* box = new btBoxShape(btVector3(width/2,height/2,length/2));	
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z)));
	
	btVector3 inertia(0,0,0);	
	if(mass != 0.0)
	{
		box->calculateLocalInertia((btScalar)mass,inertia);
	}
		

	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,box,inertia);
	
	btRigidBody* body = new btRigidBody(info);	
	
    body->setDamping(dampningL, dampningA);
    
	world->addRigidBody(body,COL_OBJECTS, objectsPhysicsCollision);
	
	bodies.push_back(body);
			
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 
}

void Physics::addSphere(float rad, Entity entity, float mass, float dampningL, float dampningA, unsigned indice,bool rotate)
{
	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

	btSphereShape* sphere = new btSphereShape(rad);
	btVector3 inertia(0,0,0);
	if(mass != 0.0)
	{
		sphere->calculateLocalInertia((btScalar)mass,inertia);
	}

	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z)));
	
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);
	

	btRigidBody* body = new btRigidBody(info);
	
    body->setDamping(dampningL, dampningA);

	world->addRigidBody(body,COL_OBJECTS, objectsPhysicsCollision);

	bodies.push_back(body);
		
    body->setSleepingThresholds(0,0);

	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

}

void Physics::addCamera()
{
    btSphereShape* sphere = new btSphereShape(0.5f);

	btVector3 inertia(0,0,0);
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),playerBall->getCenterOfMassPosition()+btVector3(5,5,5)));
	
	btRigidBody::btRigidBodyConstructionInfo info(1/(playerBall->getInvMass()/100),motion,sphere,inertia);
	
	cameraBody = new btRigidBody(info);
	
    cameraBody->setDamping(1,0.5);

	world->addRigidBody(cameraBody,COL_OBJECTS, objectsPhysicsCollision);
		
    cameraBody->setSleepingThresholds(0,0);
	
	cameraBody->setGravity(btVector3(0,0,0));
    
    /*btVector3 pivotInA(5,0,0);
    btVector3 pivotInB(-5, 0, 0);
    btDistanceConstraint* pdc = new btDistanceConstraint(*cameraBody,*playerBall,pivotInA,pivotInB, distance);
    world->addConstraint(pdc);*/
}


//update functions
glm::vec3 Physics::getCameraPosition()
{
	btVector3 origin = cameraBody->getCenterOfMassPosition();
	glm::vec3 save(origin.getX(),origin.getY(),origin.getZ());
	return save;
}

glm::vec3 Physics::getCameraToPlayer()
{
	btVector3 origin = playerBall->getCenterOfMassPosition() - cameraBody->getCenterOfMassPosition();
	glm::vec3 save(origin.getX(),origin.getY(),origin.getZ());
	return save;
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

void Physics::updateCameraPos(glm::vec2 mouseMovement, float strength)
{
    btVector3 change =  playerBall->getCenterOfMassPosition()-cameraBody->getCenterOfMassPosition();;
    change.setY(0);
    change.normalize();
    change *= mouseMovement.x;
    change = btCross(btVector3(0,1,0),change);
    change.setY(mouseMovement.y);
    change*=strength;
    
    cameraBody->applyCentralForce(change);
        
}

void Physics::rollForward(glm::vec3 camPos,float strength)
{
    btVector3 pos(camPos.x,0,camPos.z);
    pos.normalize();
    pos = btCross(pos,btVector3(0,1,0));
    pos *= strength;
    playerBall->applyTorque(pos);
}

void Physics::rollBack(glm::vec3 camPos,float strength)
{
    btVector3 pos(camPos.x,0,camPos.z);
    pos.normalize();
    pos = btCross(btVector3(0,1,0),pos);
    pos *= strength;
    playerBall->applyTorque(pos);
}

void Physics::rollLeft(glm::vec3 camPos,float strength)
{
    btVector3 pos(camPos.x,0,camPos.z);
    pos.normalize();
    pos *= strength;
    playerBall->applyTorque(pos);
}

void Physics::rollRight(glm::vec3 camPos,float strength)
{
    btVector3 pos(camPos.x,0,camPos.z);
    pos.normalize();
    pos *= strength;
    playerBall->applyTorque(-pos);
}


//not used right now

void Physics::addStaticGroundPlane()
{
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
        staticGroundBody = new btRigidBody(groundRigidBodyCI);

        world->addRigidBody(staticGroundBody);
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

