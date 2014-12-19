#include "physics.hh"
#include "extern/bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h"


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

//TERRAIN SUBSET
void Physics::addTerrainTriangles(int width, int length, float** heightData)
{//not working correctly something with offset wrong?
    btTriangleMesh* trimesh = new btTriangleMesh();
    for(int i = 0; i < width-1;i++)
    {
        for(int j = 0; j < length-1; j++)
        {
            btVector3 v0(i,heightData[j][i],j);
            btVector3 v1(i+1,heightData[j][i+1],j);
            btVector3 v2(i,heightData[j+1][i],j+1);   
            
            trimesh->addTriangle(v0,v1,v2);                             
        }    
    }
    for(int i = 1; i < width;i++)
    {
        for(int j = 1; j < length; j++)
        {
            btVector3 v0(i,heightData[j][i],j);
            btVector3 v1(i-1,heightData[j][i-1],j);
            btVector3 v2(i,heightData[j-1][i],j-1);   
            
            trimesh->addTriangle(v0,v1,v2);                             
        }    
    }
    
    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(trimesh, true);
    btRigidBody* tBody = new btRigidBody(0, new btDefaultMotionState(),shape);
    
	tBody->getWorldTransform().setOrigin(btVector3(-length/2,0,-width/2));
	
	terrainBody = tBody;
	
	world->addRigidBody(tBody);
    
}

void Physics::addTerrain(int width, int length, float** heightData)
{

	float* heightfield = new float[width * length];
	      int highest = -999999, j = 0, i = 0;
	      for (i = 0; i < width; i++)
	      {
		 for (j = 0; j < length; j++) {
		  heightfield[i*length+j] =  heightData[j][i];
		  
		    if (heightData[j][i] > highest)
		       highest = heightData[j][i];
		 }
		 }
		 
		 highest++;

	btHeightfieldTerrainShape* terrianShape = new btHeightfieldTerrainShape(length,width,heightfield,highest,1,true,false);

	btRigidBody* tBody = new btRigidBody(0,new btDefaultMotionState(),terrianShape);

	tBody->getWorldTransform().setOrigin(btVector3(0,((float)highest - 1)/2,0));

	//tBody->getWoorldTransform().setRotation(btQuaternion(0,0,0,1));

	terrainBody = tBody;
    
	world->addRigidBody(terrainBody, COL_TERRAIN, COL_TERRAIN | COL_OBJECTS);
}

void Physics::addStaticGroundPlane()
{
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
        staticGroundBody = new btRigidBody(groundRigidBodyCI);

        world->addRigidBody(staticGroundBody);
}


//players and objects
void Physics::addPlayer(float rad, Entity entity, float mass, unsigned indice)
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

    info.m_friction = 5;

	playerBall = new btRigidBody(info);
    
    playerBall->setDamping(0.1f,0.3f);
    
	world->addRigidBody(playerBall);

	bodies.push_back(playerBall);
    
    playerBall->setSleepingThresholds(0,0);
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

}

void Physics::addTriangleMeshBody(Entity entity, std::string path, float mass, float dampningL, float dampningA,unsigned indice)
{//TODO look at convexHullShapes
    
	if(bodies.size() == indice)
		throw std::invalid_argument( "Bodies out of Sync" );  
		
    std::vector< unsigned int > vertexIndices;
    std::vector< btVector3 > temp_vertices;
    
    path = "../Levels/Geometry/" + path;
    FILE * file = fopen(path.c_str(), "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
    }

    while( 1 ){
 
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop. 
    // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            temp_vertices.push_back(btVector3(vertex.x,vertex.y,vertex.z));
        }
        else if ( strcmp( lineHeader, "f" ) == 0 ){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
        }
    }
    printf("olla");
    //finally start making body        
    btTriangleMesh* triMesh = new btTriangleMesh();
    
    for(unsigned i = 2; i < vertexIndices.size();i+=3)
    {
        triMesh->addTriangle(temp_vertices[vertexIndices[i]],temp_vertices[vertexIndices[i-1]],temp_vertices[vertexIndices[i-2]]);           
    }
    
    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(triMesh,true);
    
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z)));
	
    btVector3 inertia(0,0,0);	
	if(mass != 0.0)
	{
		shape->calculateLocalInertia((btScalar)mass,inertia);
	}
    
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,shape,inertia);
	
	btRigidBody* body = new btRigidBody(info);	
	
	body->setDamping(dampningL,dampningA);    
	
	bodies.push_back(body);
	
	world->addRigidBody(body);
    
    
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 
}

void Physics::addBox(float width, float height, float length, Entity entity, float mass, unsigned indice)
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
	
    body->setDamping(0.8f,0.9f);
    
	world->addRigidBody(body);

	bodies.push_back(body);
			
	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 
}

void Physics::addSphere(float rad, Entity entity, float mass, unsigned indice)
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
	
    body->setDamping(0.2f,0.4f);

	world->addRigidBody(body);

	bodies.push_back(body);
		
    body->setSleepingThresholds(0,0);

	if(bodies.size() != indice)
		throw std::invalid_argument( "Bodies out of Sync" ); 

}
/*
void Physics::addTriangleMeshBody(Entity entity, float mass, float dampningL, float dampningA,unsigned indice)
{
    btTriangleMesh* trimesh = new btTriangleMesh();
     
     
    btVector3 v0(  0, 0, 0 );
    btVector3 v1(  1, 1, 1 );
    btVector3 v2(  2, 2, 2);
     
    trimesh->addTriangle( v0, v1, v2 );    
    	
    btTriangleMeshShape* shape = new btBvhTriangleMeshShape(trimesh,true);  
    btVector3 inertia(0,0,0);
    if(mass != 0.0)
    {
 		shape->calculateLocalInertia((btScalar)mass,inertia);
	}
	
     btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(entity.getPosition().x,entity.getPosition().y,entity.getPosition().z)));
     	
	btRigidBody::btRigidBodyConstructionInfo info(mass,motion,shape,inertia);
	btRigidBody* body = new btRigidBody(info);
	
	body->setDamping(dampningL,dampningA);
	
}*/

void Physics::addCamera(float rad, float distance)
{
    btSphereShape* sphere = new btSphereShape(rad);

	btVector3 inertia(0,0,0);
	btDefaultMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,0,0)));
	
	btRigidBody::btRigidBodyConstructionInfo info(1/(playerBall->getInvMass()/100),motion,sphere,inertia);
	
	cameraBody = new btRigidBody(info);
	
    cameraBody->setDamping(0.9f,1.0f);

	world->addRigidBody(cameraBody);
		
    cameraBody->setSleepingThresholds(0,0);
    
    
    btVector3 pivotInA(5,0,0);
    btVector3 pivotInB(-5, 0, 0);
    btDistanceConstraint* pdc = new btDistanceConstraint(*cameraBody,*playerBall,pivotInA,pivotInB, distance);
    world->addConstraint(pdc);
}


//update functions
glm::vec3 Physics::getCameraPosition()
{
	btVector3 origin = cameraBody->getCenterOfMassPosition();
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

