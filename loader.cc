#include "loader.hh"
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
using namespace tinyxml2;

Loader::Loader() {
}

void Loader::loadConfig(Application* application) {
    XMLDocument* config = new XMLDocument();
    const char* xmlFile = "../data/config.xml";
    config->LoadFile(xmlFile);
    if (config->ErrorID()!=0){
        printf("Could not open config.xml!\n");
        exit(-1);
    }
    XMLElement* resolution = config->FirstChildElement("resolution");

    application->setWindowWidth(queryInt(resolution, "width"));
    application->setWindowHeight(queryInt(resolution, "height"));
    application->setShadowCubeSize(queryInt(config, "shadowCubeSize"));
    application->setFarPlane(queryFloat(config, "farPlane"));
    application->setMaxShadowRenderCount(queryInt(config, "maxShadowRenderCount"));
    application->setCompositionsPath(queryString(config, "compositionsPath"));
    application->setShaderPath(queryString(config, "shaderPath"));
    application->setGeometryPath(queryString(config, "geometryPath"));
    std::string texturePath = queryString(config, "texturePath");
    application->setTexturePath(texturePath);
    application->setScriptPath(queryString(config, "scriptPath"));
    application->setHeightmapPath(queryString(config, "heightmapPath"));
    application->setLevelXmlPath(queryString(config, "levelXmlPath"));
    XMLElement* loadingScreen = config->FirstChildElement("loadingScreen");
    if (loadingScreen != NULL) {
        std::string screenPath = queryString(loadingScreen, "screenPath");
        std::string screenContinuePath = queryString(loadingScreen, "screenContinuePath");
        std::string screenCheckPath = "../" + texturePath + screenPath;
        std::string screenContinueCheckPath = "../" + texturePath + screenPath;
        struct stat buf;
        if(stat(screenCheckPath.c_str(), &buf) != 0){
            std::cout << "The texture file " << screenCheckPath << " does not exist." << std::endl;
            exit(-1);
        }
        if(stat(screenContinueCheckPath.c_str(), &buf) != 0){
            std::cout << "The texture file " << screenContinueCheckPath << " does not exist." << std::endl;
            exit(-1);
        }
        application->setLoadingScreenPath(screenPath);
        application->setLoadingScreenContinuePath(screenContinuePath);
    }
    else {
        printf("Could not find loading screen settings in config.xml. Exiting.\n");
        exit(-1);
    }
}

void Loader::load(std::string filePath, Level* level, std::string compositionsPath, std::string scriptPath, std::string globalGeometryPath, std::string globalTexturePath) {
    //Loading from xml:
    XMLDocument* doc = new XMLDocument();
    const char* xmlFile = filePath.c_str();
    doc->LoadFile(xmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open ObjectSetupXml!\n");
        exit(-1);
    }
    //load global physic parameter
    XMLElement* physicsElement = doc->FirstChildElement("physics");
    float strength = queryFloat(physicsElement, "strength");
    float friction = queryFloat(physicsElement, "friction");
    level->setStrength(strength);
    
    // load the terrain
    level->getTerrain()->load();
    Model terrainModel = Model(level->getTerrain()->getModel());
    XMLElement* terrainElement = doc->FirstChildElement("terrain");
    std::string terrainTexture = queryString(terrainElement, "texture");
    float terrainAmbientFactor = queryFloat(terrainElement, "ambientFactor");
    float terrainDiffuseFactor = queryFloat(terrainElement, "diffuseFactor");
    float terrainSpecularFactor = queryFloat(terrainElement, "specularFactor");
    float terrainShininess = queryFloat(terrainElement, "shininess");
    struct stat buf;
    std::string terrainTexturePath = "../" + globalTexturePath + terrainTexture;
    if(stat(terrainTexturePath.c_str(), &buf) != 0){
        std::cout << "The texture file " << terrainTexturePath << " does not exist." << std::endl;
        exit(-1);
    }
    Material terrainMaterial = Material(terrainTexture, terrainAmbientFactor, terrainDiffuseFactor, terrainSpecularFactor, terrainShininess);
    Object* terrainObject = new Object(terrainModel, terrainMaterial,
    glm::vec3(-0.5*((float)level->getTerrain()->getHeightmapHeight()-1), 0.0f, -0.5f*((float)level->getTerrain()->getHeightmapWidth()-1)),
        glm::vec3(0.0f, 0.0f, 0.0f), true);
    level->addObject(terrainObject);
    level->getPhysics()->addTerrain(level->getTerrain()->getHeightmapWidth(), level->getTerrain()->getHeightmapHeight(), level->getTerrain()->getHeightmap());
    
    //load the skydome
    XMLElement* skydomeElement = doc->FirstChildElement("skydome");
    std::string skydomeModelFileName = queryString(skydomeElement, "model");
    std::string skydomePath = "../" + globalGeometryPath + skydomeModelFileName;
    if(stat(skydomePath.c_str(), &buf) != 0){
        std::cout << "The object file " << skydomePath << " does not exist." << std::endl;
        exit(-1);
    }
    Model skydomeModel = Model(skydomeModelFileName, level->getSkydomeSize());
    std::string skydomeTexture = queryString(skydomeElement, "texture");
    std::string skydomeTexturePath = "../" + globalTexturePath + skydomeTexture;
    if(stat(skydomeTexturePath.c_str(), &buf) != 0){
        std::cout << "The texture file " << skydomeTexturePath << " does not exist." << std::endl;
        exit(-1);
    }
    Material skydomeMaterial = Material(skydomeTexture, 1.0f, 0.0f, 0.0f, 0.0f);
    std::string nightTexture = queryString(skydomeElement, "nightTexture");
    std::string nightTexturePath = "../" + globalTexturePath + nightTexture;
    if(stat(nightTexturePath.c_str(), &buf) != 0){
        std::cout << "The texture file " << nightTexturePath << " does not exist." << std::endl;
        exit(-1);
    }
    Material nightMaterial = Material(nightTexture, 1.0f, 0.0f, 0.0f, 0.0f);
    Skydome skydomeObject = Skydome(skydomeModel, skydomeMaterial, nightMaterial);
    level->setSkydomeObject(skydomeObject);
    
    //load the waterPlane
    XMLElement* waterElement = doc->FirstChildElement("waterPlane");
    if (waterElement != NULL){
        float waterHeight = queryFloat(waterElement, "yPosition");
        std::string waterTexture = queryString(waterElement, "texture");
        std::string waterTexturePath = "../" + globalTexturePath + waterTexture;
        if(stat(waterTexturePath.c_str(), &buf) != 0){
            std::cout << "The texture file " << waterTexturePath << " does not exist." << std::endl;
            exit(-1);
        }
        float heightmapHeight = level->getTerrain()->getHeightmapHeight();
        float heightmapWidth = level->getTerrain()->getHeightmapWidth();
        float planeData[] = {
            -heightmapWidth/2.0f, waterHeight, -heightmapHeight/2.0f, 0.0f, heightmapHeight,   0.0f, 1.0f, 0.0f,
            -heightmapWidth/2.0f, waterHeight, heightmapHeight/2.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
            heightmapWidth/2.0f, waterHeight, -heightmapHeight/2.0f, heightmapWidth, heightmapHeight,   0.0f, 1.0f, 0.0f, 
            
            heightmapWidth/2.0f, waterHeight, heightmapHeight/2.0f, heightmapWidth, 0.0f,  0.0f, 1.0f, 0.0f,
            -heightmapWidth/2.0f, waterHeight, heightmapHeight/2.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
            heightmapWidth/2.0f, waterHeight, -heightmapHeight/2.0f, heightmapWidth, heightmapHeight,  0.0f, 1.0f, 0.0f
        };
        ACGL::OpenGL::SharedArrayBuffer water_ab = ACGL::OpenGL::SharedArrayBuffer(new ACGL::OpenGL::ArrayBuffer());
        water_ab->defineAttribute("aPosition", GL_FLOAT, 3);
        water_ab->defineAttribute("aTexCoord", GL_FLOAT, 2);
        water_ab->defineAttribute("aNormal", GL_FLOAT, 3);
        water_ab->setDataElements(6, planeData);
        ACGL::OpenGL::SharedVertexArrayObject water_vao = ACGL::OpenGL::SharedVertexArrayObject(new ACGL::OpenGL::VertexArrayObject());
        water_vao->bind();
        water_vao->setMode(GL_TRIANGLES);
        water_vao->attachAllAttributes(water_ab);
        Material water_material = Material(waterTexture, 0.1f, 0.2f, 0.8f, 5.0f, true);
        Object* water_object = new Object(water_vao, water_material, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), true);
        level->addObject(water_object);
    }
    
    //load lighting parameters
    float rColour, gColour, bColour, alpha, xOffset, yOffset, zOffset, intensity;
    XMLElement* ambientElement = doc->FirstChildElement("ambientLight");
    rColour = queryFloat(ambientElement, "rColour");
    gColour = queryFloat(ambientElement, "gColour");
    bColour = queryFloat(ambientElement, "bColour");
    level->setAmbientLight(glm::vec3(rColour,gColour,bColour));
    
    XMLElement* fogElement = doc->FirstChildElement("fogColour");
    rColour = queryFloat(fogElement, "rColour");
    gColour = queryFloat(fogElement, "gColour");
    bColour = queryFloat(fogElement, "bColour");
    alpha = queryFloat(fogElement, "alpha");
    level->setFogColour(glm::vec4(rColour,gColour,bColour, alpha));
    
    XMLElement* directionalElement = doc->FirstChildElement("directionalLight");
    xOffset = queryFloat(directionalElement, "xOffset");
    yOffset = queryFloat(directionalElement, "yOffset");
    zOffset = queryFloat(directionalElement, "zOffset");
    rColour = queryFloat(directionalElement, "rColour");
    gColour = queryFloat(directionalElement, "gColour");
    bColour = queryFloat(directionalElement, "bColour");
    intensity = queryFloat(directionalElement, "intensity");
    level->setDirectionalLight(Light(glm::vec3(xOffset,yOffset,zOffset), glm::vec3(rColour,gColour,bColour), intensity));
    
    //load Objects
    std::vector<std::vector<int>> objectIdentifiers = std::vector<std::vector<int>>();  //The first entry is the index in objects, the second one the index in physicObjects, the others are idGreen, idBlue and objectNum.  
    XMLDocument* compositions = new XMLDocument();
    const char* compositionsFile = compositionsPath.c_str();
    compositions->LoadFile(compositionsFile);
    if (compositions->ErrorID()!=0){
        printf("Could not open Compositions!\n");
        exit(-1);
    }
    //iterate over all compositions in Level.xml
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        int thisType = queryInt(thisComposition, "typeID");
        //iterate over all compositions in Compositions.xml to find the one corresponding to the current composition
        XMLElement* composition = compositions->FirstChildElement("composition");
        bool typeExists = false;
        for(; composition; composition=composition->NextSiblingElement("composition")){
            int compositionType = queryInt(composition, "typeID");
            //corect composition found
            if(thisType == compositionType){
                typeExists = true;
                //iterate over all objects of the composition
                XMLElement* xmlObject = composition->FirstChildElement("object");
                int objectNum = 0;
                for(; xmlObject; xmlObject=xmlObject->NextSiblingElement("object")){
                    std::string modelPath = queryString(xmlObject, "modelPath");
                    float objectScale = queryFloat(xmlObject, "scale");
                    float compScale = queryFloat(thisComposition, "scale");
                    //find the objectData for the current object
                    XMLElement* objectData = compositions->FirstChildElement("objectData");
                    for(; objectData; objectData=objectData->NextSiblingElement("objectData")){
                        std::string dataModelPath = queryString(objectData, "modelPath");
                        //objectData found
                        if(dataModelPath.compare(modelPath) == 0){
                            bool renderable = queryBool(objectData, "renderable");
                            //create the object
                            Material material;
                            Model model;
                            if (renderable) {
                                float ambientFactor = queryFloat(objectData, "ambientFactor");
                                float diffuseFactor = queryFloat(objectData, "diffuseFactor");
                                float specularFactor = queryFloat(objectData, "specularFactor");
                                float shininess = queryFloat(objectData, "shininess");
                                std::string texturePath = queryString(objectData, "texturePath");
                                std::string entireTexturePath = "../" + globalTexturePath + texturePath;
                                if(stat(entireTexturePath.c_str(), &buf) != 0){
                                    std::cout << "The texture file " << entireTexturePath << " does not exist." << std::endl;
                                    exit(-1);
                                }
                                material = Material(texturePath, ambientFactor, diffuseFactor, specularFactor, shininess);
                                std::string entireModelPath = "../" + globalGeometryPath + modelPath;
                                if(stat(entireModelPath.c_str(), &buf) != 0){
                                    std::cout << "The object file " << entireModelPath << " does not exist." << std::endl;
                                    exit(-1);
                                }
                                model = Model(modelPath, objectScale * compScale);
                            }
                            float compXPos = queryFloat(thisComposition, "xPos");
                            float compYOffset = queryFloat(thisComposition, "yOffset");
                            float compZPos = queryFloat(thisComposition, "zPos");
                            glm::vec3 objectOffset, compRot;
                            objectOffset[0] = queryFloat(xmlObject, "xOffset");
                            objectOffset[1] = queryFloat(xmlObject, "yOffset");
                            objectOffset[2] = queryFloat(xmlObject, "zOffset");
                            compRot[0] = queryFloat(thisComposition, "xRot");
                            compRot[1] = queryFloat(thisComposition, "yRot");
                            compRot[2] = queryFloat(thisComposition, "zRot");
                            compRot *= 0.0174532925;    //transform degrees to radians
                            bool ignoreHeightmap = queryBool(composition, "ignoreHeightmap");
                            if (!ignoreHeightmap){
                                compYOffset = compYOffset+level->getTerrain()->getHeightmap()[int(compXPos-0.5+0.5*level->getTerrain()->getHeightmapHeight())]
                                                                                [int(compZPos-0.5+0.5*level->getTerrain()->getHeightmapWidth())];
                            }
                            glm::vec3 compPos = glm::vec3(compXPos, compYOffset, compZPos);
                            objectOffset = objectOffset * compScale;
                            glm::vec4 rotatedObjectOffset = glm::rotate(compRot.x, glm::vec3(1.0f, 0.0f, 0.0f))
                                                            * glm::rotate(compRot.y, glm::vec3(0.0f, 1.0f, 0.0f))
                                                            * glm::rotate(compRot.z, glm::vec3(0.0f, 0.0f, 1.0f))
                                                            * glm::vec4(objectOffset, 0);
                            glm::vec3 objectPosition = compPos + glm::vec3(rotatedObjectOffset.x,rotatedObjectOffset.y,rotatedObjectOffset.z);
                            glm::vec3 objectRot;
                            objectRot[0] = queryFloat(xmlObject, "xRot");
                            objectRot[1] = queryFloat(xmlObject, "yRot");
                            objectRot[2] = queryFloat(xmlObject, "zRot");
                            objectRot *= 0.0174532925;    //transform degrees to radians
                            Object* object = new Object(model, material, objectPosition, compRot+objectRot, renderable);
                            level->addObject(object);
                            
                            //add object to physics
                            std::string physicType = queryString(objectData, "physicType");
                            float mass = queryFloat(xmlObject, "mass");
                            XMLElement* constraint = thisComposition->FirstChildElement("positionConstraint");
                            bool rotate = (constraint == NULL);
                            float dampningL = 0.0f, dampningA = 0.0f;
                            if (physicType.compare("None") != 0){
                                dampningL = queryFloat(objectData, "dampningL");
                                dampningA = queryFloat(objectData, "dampningA");
                            }
                            if (physicType.compare("Player") == 0){
                                float radius = queryFloat(objectData, "radius");
                                radius *= objectScale*compScale;
                                level->addPhysicsObject(object);
                                level->getPhysics()->addPlayer(friction, radius, *object, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize());
                            }else if (physicType.compare("Box") == 0){
                                float width = queryFloat(objectData, "width");
                                float height = queryFloat(objectData, "height");
                                float length = queryFloat(objectData, "length");
                                width  *= objectScale*compScale;
                                height *= objectScale*compScale;
                                length *= objectScale*compScale;
                                level->addPhysicsObject(object);
                                level->getPhysics()->addBox(width, height, length, *object, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize(), rotate);
                            }else if (physicType.compare("Button") == 0){
                                float width = queryFloat(objectData, "width");
                                float height = queryFloat(objectData, "height");
                                float length = queryFloat(objectData, "length");
                                width  *= objectScale*compScale;
                                height *= objectScale*compScale;
                                length *= objectScale*compScale;
                                level->addPhysicsObject(object);
                                level->getPhysics()->addButton(width, height, length, *object, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize(), rotate);
                            }else if (physicType.compare("TriangleMesh") == 0){
                                level->addPhysicsObject(object);
                                level->getPhysics()->addTriangleMeshBody(*object, modelPath, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize(), objectScale*compScale, rotate);
                            }else if (physicType.compare("None") == 0){
                            
                            } else{
                                printf("XMLError: Not a valid physicType.\n");
                                exit(-1);
                            }
                            
                            //create an identifier for this object
                            std::vector<int> objectIdentifier = std::vector<int>(5);
                            objectIdentifier[0] = level->getObjectsVectorSize()-1;
                            if (physicType.compare("None") == 0){
                                objectIdentifier[1] = 0;
                            }else{
                                objectIdentifier[1] = level->getPhysicsObjectsVectorSize()-1;
                            }
                            int idGreen = queryInt(thisComposition, "idGreen");
                            int idBlue = queryInt(thisComposition, "idBlue");
                            objectIdentifier[2] = idGreen;
                            objectIdentifier[3] = idBlue;
                            objectIdentifier[4] = objectNum;
                            objectIdentifiers.push_back(objectIdentifier);
                            
                            if(compositionType == 20){
                                level->setCameraCenter(object);
                                level->setPlayerIndex(objectIdentifier[1]);
                            }
                        }//objectData found
                    }//finding the objectData
                    
                    objectNum = objectNum + 1;
                }//iterating over all objects of the composition
                
                //iterate over all lights of the composition
                XMLElement* xmlLight = composition->FirstChildElement("light");
                for(; xmlLight; xmlLight=xmlLight->NextSiblingElement("light")){
                    glm::vec3 compRot, lightOffset, lightColour;
                    compRot[0] = queryFloat(thisComposition, "xRot");
                    compRot[1] = queryFloat(thisComposition, "yRot");
                    compRot[2] = queryFloat(thisComposition, "zRot");
                    lightOffset[0] = queryFloat(xmlLight, "xOffset");
                    lightOffset[1] = queryFloat(xmlLight, "yOffset");
                    lightOffset[2] = queryFloat(xmlLight, "zOffset");
                    lightColour[0] = queryFloat(xmlLight, "rColour");
                    lightColour[1] = queryFloat(xmlLight, "gColour");
                    lightColour[2] = queryFloat(xmlLight, "bColour");
                    float compScale = queryFloat(thisComposition, "scale");
                    float compXPos = queryFloat(thisComposition, "xPos");
                    float compYOffset = queryFloat(thisComposition, "yOffset");
                    float compZPos = queryFloat(thisComposition, "zPos");
                    float lightIntensity = queryFloat(xmlLight, "intensity");
                    glm::vec3 compPos = glm::vec3(compXPos,
                                        compYOffset+level->getTerrain()->getHeightmap()[int(compXPos-0.5+0.5*level->getTerrain()->getHeightmapHeight())]
                                                                          [int(compZPos-0.5+0.5*level->getTerrain()->getHeightmapWidth())],
                                        compZPos);
                    lightOffset = lightOffset * compScale;
                    glm::vec4 rotatedLightOffset = glm::rotate(compRot.x, glm::vec3(1.0f, 0.0f, 0.0f))
                                                    * glm::rotate(compRot.y, glm::vec3(0.0f, 1.0f, 0.0f))
                                                    * glm::rotate(compRot.z, glm::vec3(0.0f, 0.0f, 1.0f))
                                                    * glm::vec4(lightOffset, 0);
                    glm::vec3 lightPosition = compPos + glm::vec3(rotatedLightOffset.x,rotatedLightOffset.y,rotatedLightOffset.z);
                    XMLElement* flameOffset = NULL;
                    flameOffset = xmlLight->FirstChildElement("flameOffset");
                    if (flameOffset != NULL){
                        float offset = 0;
                        errorCheck(flameOffset->QueryFloatText(&offset));
                        float flameHeight = queryFloat(xmlLight, "flameHeight");
                        printf("loading: flameHeight: %2.2f\n", flameHeight);
                        float flameWidth = queryFloat(xmlLight, "flameWidth");
                        printf("loading: flameWidth: %2.2f\n", flameWidth);
                        Light light = Light(lightPosition, lightColour, lightIntensity, offset, flameHeight, flameWidth);
                        level->addLight(light);
                    }
                    else {
                        Light light = Light(lightPosition, lightColour, lightIntensity);
                        level->addLight(light);
                    }
                }//iterating over all lights of the composition
            }//corect composition found
        }//iterating over all compositions in Compositions.xml
        if (!typeExists){
            std::cout << "The typeID " << thisType << " exists in the level xml but is not a Compositions." << std::endl;
        }
    }//iterating over all compositions in Level.xml
    
    //load triggers

    // call init.lua to initialize the module load path in Lua
    std::string initLuaPath = scriptPath + "init.lua";
    luaL_dofile(level->getLuaState(), initLuaPath.c_str());
    XMLElement* composition = doc->FirstChildElement("composition");
    for(; composition; composition=composition->NextSiblingElement("composition")){
        XMLElement* xmlTrigger = composition->FirstChildElement("trigger");
        for(; xmlTrigger; xmlTrigger=xmlTrigger->NextSiblingElement("trigger")){
            std::string name = queryString(xmlTrigger, "name");
            if (name.compare("-") != 0){
                float xPos = queryFloat(xmlTrigger, "xPosition");
                float yPos = queryFloat(xmlTrigger, "yPosition");
                float zPos = queryFloat(xmlTrigger, "zPosition");
                bool undo = queryBool(xmlTrigger, "undo");
                glm::vec3 position = glm::vec3(xPos, yPos, zPos);
                std::string stringTarget = queryString(xmlTrigger, "targetIdGreen");
                if (stringTarget.compare("-") != 0){
                    int targetIdGreen = queryInt(xmlTrigger, "targetIdGreen");
                    int targetIdBlue = queryInt(xmlTrigger, "targetIdBlue");
                    XMLElement* thisComposition = doc->FirstChildElement("composition");
                    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
                        int thisIdGreen = queryInt(thisComposition, "idGreen");
                        int thisIdBlue = queryInt(thisComposition, "idBlue");
                        if (targetIdGreen == thisIdGreen && targetIdBlue == thisIdBlue){
                            glm::vec3 targetPosition; 
                            targetPosition[0] = queryFloat(thisComposition, "xPos");
                            targetPosition[1] = queryFloat(thisComposition, "yOffset");
                            targetPosition[2] = queryFloat(thisComposition, "zPos");
                            targetPosition[1] += level->getTerrain()->getHeightmap()[int(targetPosition[0]-0.5+0.5*level->getTerrain()->getHeightmapHeight())]
                                                                       [int(targetPosition[2]-0.5+0.5*level->getTerrain()->getHeightmapWidth())];
                            position += targetPosition;
                        }
                    }
                }
                float distance = queryFloat(xmlTrigger, "distance");
                bool isBigger = queryBool(xmlTrigger, "isBiggerThan");
                int idGreen = queryInt(composition, "idGreen");
                int idBlue = queryInt(composition, "idBlue");
                int objectNum = queryInt(xmlTrigger, "objectNum");
                Object* object=0;
                bool ok = false;
                for (unsigned int i = 0; i<objectIdentifiers.size(); i++){
                    if (objectIdentifiers[i][2]==idGreen && objectIdentifiers[i][3]==idBlue && objectIdentifiers[i][4]==objectNum){
                        object = level->getPhysicsObjects()->at(objectIdentifiers[i][1]);  //Index in physics objects
                        if(ok){
                            printf("2 objects have the same ID while loading triggers.");
                            exit(-1);
                        }
                        ok = true;
                    }
                }
                if(!ok){
                    printf("No index found for a trigger object while loading triggers.");
                    exit(-1);
                }
                std::string luaScript = queryString(xmlTrigger, "luaScript");
                
                int toChangeIdGreen = queryInt(xmlTrigger, "toChangeIdGreen");
                int toChangeIdBlue = queryInt(xmlTrigger, "toChangeIdBlue");
                int toChangeObjNum = queryInt(xmlTrigger, "toChangeObjNum");
                int objectToChange=-1;
                for (unsigned int i = 0; i<objectIdentifiers.size(); i++){
                    if (objectIdentifiers[i][2]==toChangeIdGreen && objectIdentifiers[i][3]==toChangeIdBlue && objectIdentifiers[i][4]==toChangeObjNum){
                        objectToChange = objectIdentifiers[i][1];     //Index in physic objects
                    }
                }
                if (objectToChange == -1){
                    printf("No Identifier found for an object that was to be changed by a trigger.\n");
                }
                if (object != 0) {
                    Trigger trigger = Trigger(position, distance, isBigger, object, luaScript, level->getLuaState(), objectToChange, scriptPath, undo);
                    level->addTrigger(trigger);
                }
                else {
                    printf("Triggering object not found.\n");
                    exit(-1);
                }
            }
        }
    }//triggers
    
    //load positionConstraints
    composition = doc->FirstChildElement("composition");
    for(; composition; composition=composition->NextSiblingElement("composition")){
        XMLElement* positionConstraint = composition->FirstChildElement("positionConstraint");
        for(; positionConstraint; positionConstraint=positionConstraint->NextSiblingElement("positionConstraint")){
            float xPos = queryFloat(positionConstraint, "xPosition");
            float yPos = queryFloat(positionConstraint, "yPosition");
            float zPos = queryFloat(positionConstraint, "zPosition");
            float strength = queryFloat(positionConstraint, "strength");
            int objectNum = queryInt(positionConstraint, "objectNum");
            int idGreen = queryInt(composition, "idGreen");
            int idBlue = queryInt(composition, "idBlue");
            int objectIndex = 0;
            bool ok = false;
            for (unsigned int i = 0; i<objectIdentifiers.size(); i++){
                if (objectIdentifiers[i][2]==idGreen && objectIdentifiers[i][3]==idBlue && objectIdentifiers[i][4]==objectNum){
                    if(ok){
                        printf("2 objects have the same ID while loading constraints.");
                        exit(-1);
                    }
                    objectIndex = objectIdentifiers[i][1];    //Index in physic objects
                    ok = true;
                }
            }
            if(!ok){
                printf("No index found for a trigger object while loading constraints.");
                exit(-1);
            }
            glm::vec3 position = glm::vec3(xPos, yPos, zPos);
            level->getPhysics()->addPositionConstraint(objectIndex, strength, position);
        }
    }//positionConstraints
}



glm::vec3 Loader::reloadPlayerPosition(std::string filePath, Level* level){
    XMLDocument* doc = new XMLDocument();
    const char* xmlFile = filePath.c_str();
    doc->LoadFile(xmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open ObjectSetupXml!\n");
        exit(-1);
    }
    //iterate over all compositions in Level.xml
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        int thisType = queryInt(thisComposition, "typeID");
        if (thisType == 20){
            float compXPos = queryFloat(thisComposition, "xPos");
            float compYOffset = queryFloat(thisComposition, "yOffset");
            float compZPos = queryFloat(thisComposition, "zPos");
            compYOffset += level->getTerrain()->getHeightmap()[int(compXPos-0.5+0.5*level->getTerrain()->getHeightmapHeight())]
                                                                        [int(compZPos-0.5+0.5*level->getTerrain()->getHeightmapWidth())];
            glm::vec3 position = glm::vec3(compXPos, compYOffset, compZPos);
            return position;
        }
    }
    printf("Level.xml contains no player.");
    exit(-1);
}

float Loader::queryFloat(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    float ret;
    errorCheck(attributeElement->QueryFloatText(&ret));
    return ret;
}

float Loader::queryFloat(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    float ret;
    errorCheck(attributeElement->QueryFloatText(&ret));
    return ret;
}

int Loader::queryInt(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    int ret;
    errorCheck(attributeElement->QueryIntText(&ret));
    return ret;
}

int Loader::queryInt(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    int ret;
    errorCheck(attributeElement->QueryIntText(&ret));
    return ret;
}

bool Loader::queryBool(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    bool ret;
    errorCheck(attributeElement->QueryBoolText(&ret));
    return ret;
}

bool Loader::queryBool(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    bool ret;
    errorCheck(attributeElement->QueryBoolText(&ret));
    return ret;
}

std::string Loader::queryString(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    const char* charRet = attributeElement->GetText();
    if(charRet == NULL){
        std::cout << "XMLError: Attribute " << attribute << " could not be loaded." << std::endl;
        exit(-1);
    }
    std::string ret = charRet;
    return ret;
}

std::string Loader::queryString(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    const char* charRet = attributeElement->GetText();
    if(charRet == NULL){
        std::cout << "XMLError: Attribute " << attribute << " could not be loaded." << std::endl;
        exit(-1);
    }
    std::string ret = charRet;
    return ret;
}

void Loader::errorCheck(XMLError error){
    if (error) {
        printf("XMLError: ");
        if (error == XML_WRONG_ATTRIBUTE_TYPE) {
            printf("Wrong attribute type.\n");
        }
        else if (error == XML_NO_ATTRIBUTE) {
            printf("No attribute.\n");
        }
        else if (error == XML_CAN_NOT_CONVERT_TEXT) {
            printf("Can not convert text.\n");
        }
        else if (error == XML_NO_TEXT_NODE) {
            printf("No text.\n");
        }
        else {
            printf("Unknown error.\n");
        }
        exit(-1);
    }
}
