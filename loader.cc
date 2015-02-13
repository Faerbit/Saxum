#include "loader.hh"
using namespace tinyxml2;

Loader::Loader() {
}

void Loader::loadConfig(Application* application) {
    int windowWidth, windowHeight, shadowCubeSize, maxShadowRenderCount;
    float farPlane;
    std::string compositionsPath, shaderPath, geometryPath, texturePath, scriptPath, heightmapPath, levelXmlPath;
    XMLDocument* config = new XMLDocument();
    const char* xmlFile = "../data/config.xml";
    config->LoadFile(xmlFile);
    if (config->ErrorID()!=0){
        printf("Could not open config.xml!\n");
        exit(-1);
    }
    XMLElement* resolution = config->FirstChildElement("resolution");
    errorCheck(resolution->FirstChildElement("width")->QueryIntText(&windowWidth));
    errorCheck(resolution->FirstChildElement("height")->QueryIntText(&windowHeight));
    errorCheck(config->FirstChildElement("shadowCubeSize")->QueryIntText(&shadowCubeSize));
    errorCheck(config->FirstChildElement("farPlane")->QueryFloatText(&farPlane));
    errorCheck(config->FirstChildElement("maxShadowRenderCount")->QueryIntText(&maxShadowRenderCount));
    
    const char* charCompositionsPath = config->FirstChildElement("compositionsPath")->GetText();
    if(charCompositionsPath == NULL){
        printf("XMLError: No compositionsPath found.\n");
        exit(-1);
    }
    compositionsPath = charCompositionsPath;
    
    const char* charShaderPath = config->FirstChildElement("shaderPath")->GetText();
    if(charShaderPath == NULL){
        printf("XMLError: No shaderPath found.\n");
        exit(-1);
    }
    shaderPath = charShaderPath;
    
    const char* charGeometryPath = config->FirstChildElement("geometryPath")->GetText();
    if(charGeometryPath == NULL){
        printf("XMLError: No geometryPath found.\n");
        exit(-1);
    }
    geometryPath = charGeometryPath;
    
    const char* charTexturePath = config->FirstChildElement("texturePath")->GetText();
    if(charTexturePath == NULL){
        printf("XMLError: No texturePath found.\n");
        exit(-1);
    }
    texturePath = charTexturePath;
    
    const char* charScriptPath = config->FirstChildElement("scriptPath")->GetText();
    if(charScriptPath == NULL){
        printf("XMLError: No scriptPath found.\n");
        exit(-1);
    }
    scriptPath = charScriptPath;

    const char* charHeightmapPath = config->FirstChildElement("heightmapPath")->GetText();
    if(charHeightmapPath == NULL){
        printf("XMLError: No heightmapPath found.\n");
        exit(-1);
    }
    heightmapPath = charHeightmapPath;
    
    const char* charLevelXmlPath = config->FirstChildElement("levelXmlPath")->GetText();
    if(charLevelXmlPath == NULL){
        printf("XMLError: No levelXmlPath found.\n");
        exit(-1);
    }
    levelXmlPath = charLevelXmlPath;
    
    application->setWindowWidth(windowWidth);
    application->setWindowHeight(windowHeight);
    application->setShadowCubeSize(shadowCubeSize);
    application->setFarPlane(farPlane);
    application->setMaxShadowRenderCount(maxShadowRenderCount);
    application->setCompositionsPath(compositionsPath);
    application->setShaderPath(shaderPath);
    application->setGeometryPath(geometryPath);
    application->setTexturePath(texturePath);
    application->setScriptPath(scriptPath);
    application->setHeightmapPath(heightmapPath);
    application->setLevelXmlPath(levelXmlPath);
}

void Loader::load(std::string filePath, Level* level, std::string compositionsPath, std::string scriptPath) {
    //Loading from xml:
    XMLDocument* doc = new XMLDocument();
    const char* xmlFile = filePath.c_str();
    doc->LoadFile(xmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open ObjectSetupXml!\n");
        exit(-1);
    }
    //load global physic parameter
    float friction, strength;
    XMLElement* physicsElement = doc->FirstChildElement("physics");
    errorCheck(physicsElement->FirstChildElement("strength")->QueryFloatText(&strength));
    errorCheck(physicsElement->FirstChildElement("friction")->QueryFloatText(&friction));
    level->setStrength(strength);
    
    // load the terrain
    level->getTerrain()->load();
    Model terrainModel = Model(level->getTerrain()->getModel());
    XMLElement* terrainElement = doc->FirstChildElement("terrain");
    const char* charTerrainTexture = terrainElement->FirstChildElement("texture")->GetText();
    if(charTerrainTexture == NULL){
        printf("XMLError: No terrainTexture found.\n");
        exit(-1);
    }
    std::string terrainTexture = charTerrainTexture;
    float terrainAmbientFactor, terrainDiffuseFactor, terrainSpecularFactor, terrainShininess;
    errorCheck(terrainElement->FirstChildElement("ambientFactor")->QueryFloatText(&terrainAmbientFactor));
    errorCheck(terrainElement->FirstChildElement("diffuseFactor")->QueryFloatText(&terrainDiffuseFactor));
    errorCheck(terrainElement->FirstChildElement("specularFactor")->QueryFloatText(&terrainSpecularFactor));
    errorCheck(terrainElement->FirstChildElement("shininess")->QueryFloatText(&terrainShininess));
    Material terrainMaterial = Material(terrainTexture, terrainAmbientFactor, terrainDiffuseFactor, terrainSpecularFactor, terrainShininess);
    Object* terrainObject = new Object(terrainModel, terrainMaterial,
    glm::vec3(-0.5*(float)level->getTerrain()->getHeightmapHeight(), 0.0f, -0.5f*(float)level->getTerrain()->getHeightmapWidth()),
        glm::vec3(0.0f, 0.0f, 0.0f), true);
    level->addObject(terrainObject);
    level->getPhysics()->addTerrain(level->getTerrain()->getHeightmapWidth(), level->getTerrain()->getHeightmapHeight(), level->getTerrain()->getHeightmap());
    
    //load the skydome
    XMLElement* skydomeElement = doc->FirstChildElement("skydome");
    const char* charSkydomeTexture = skydomeElement->FirstChildElement("texture")->GetText();
    if(charSkydomeTexture == NULL){
        printf("XMLError: No skydomeTexture found.\n");
        exit(-1);
    }
    std::string skydomeTexture = charSkydomeTexture;
    Model skydomeModel = Model("skydome.obj", level->getSkydomeSize());
    Material skydomeMaterial = Material(skydomeTexture, 0.7f, 0.0f, 0.0f, 0.0f);
    Object* skydomeObject = new Object(skydomeModel, skydomeMaterial, glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f), true);
    level->addObject(skydomeObject);
    level->setSkydomeObject(skydomeObject);
    
    //load lighting parameters
    float rColour, gColour, bColour, alpha, xOffset, yOffset, zOffset, intensity;
    XMLElement* ambientElement = doc->FirstChildElement("ambientLight");
    errorCheck(ambientElement->FirstChildElement("rColour")->QueryFloatText(&rColour));
    errorCheck(ambientElement->FirstChildElement("gColour")->QueryFloatText(&gColour));
    errorCheck(ambientElement->FirstChildElement("bColour")->QueryFloatText(&bColour));
    level->setAmbientLight(glm::vec3(rColour,gColour,bColour));
    
    XMLElement* fogElement = doc->FirstChildElement("fogColour");
    errorCheck(fogElement->FirstChildElement("rColour")->QueryFloatText(&rColour));
    errorCheck(fogElement->FirstChildElement("gColour")->QueryFloatText(&gColour));
    errorCheck(fogElement->FirstChildElement("bColour")->QueryFloatText(&bColour));
    errorCheck(fogElement->FirstChildElement("alpha")->QueryFloatText(&alpha));
    level->setFogColour(glm::vec4(rColour,gColour,bColour, alpha));
    
    XMLElement* directionalElement = doc->FirstChildElement("directionalLight");
    errorCheck(directionalElement->FirstChildElement("xOffset")->QueryFloatText(&xOffset));
    errorCheck(directionalElement->FirstChildElement("yOffset")->QueryFloatText(&yOffset));
    errorCheck(directionalElement->FirstChildElement("zOffset")->QueryFloatText(&zOffset));
    errorCheck(directionalElement->FirstChildElement("rColour")->QueryFloatText(&rColour));
    errorCheck(directionalElement->FirstChildElement("gColour")->QueryFloatText(&gColour));
    errorCheck(directionalElement->FirstChildElement("bColour")->QueryFloatText(&bColour));
    errorCheck(directionalElement->FirstChildElement("intensity")->QueryFloatText(&intensity));
    level->setDirectionalLight(Light(glm::vec3(xOffset,yOffset,zOffset), glm::vec3(rColour,gColour,bColour), intensity));
    
    //load Objects
    std::vector<std::vector<int>> objectIdentifiers = std::vector<std::vector<int>>();  //The first entry is the index in objects, the second one the index in physicObjects, the others are idGreen, idBlue and objectNum.  
    XMLDocument* compositions = new XMLDocument();
    //TODO move path to config.xml
    const char* compositionsFile = compositionsPath.c_str();
    compositions->LoadFile(compositionsFile);
    if (compositions->ErrorID()!=0){
        printf("Could not open Compositions!\n");
        exit(-1);
    }
    //iterate over all compositions in Level.xml
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        int thisType = 0;
        errorCheck(thisComposition->FirstChildElement("typeID")->QueryIntText(&thisType));
        //iterate over all compositions in Compositions.xml to find the one corresponding to the current composition
        XMLElement* composition = compositions->FirstChildElement("composition");
        for(; composition; composition=composition->NextSiblingElement("composition")){
            int compositionType = 0;
            errorCheck(composition->FirstChildElement("typeID")->QueryIntText(&compositionType));
            //corect composition found
            if(thisType == compositionType){
                //iterate over all objects of the composition
                XMLElement* xmlObject = composition->FirstChildElement("object");
                int objectNum = 0;
                for(; xmlObject; xmlObject=xmlObject->NextSiblingElement("object")){
                    const char* charModelPath = xmlObject->FirstChildElement("modelPath")->GetText();
                    if(charModelPath == NULL){
                        printf("XMLError: No modelPath found in object.\n");
                        exit(-1);
                    }
                    std::string modelPath = charModelPath;
                    float objectScale, compScale;
                    errorCheck(xmlObject->FirstChildElement("scale")->QueryFloatText(&objectScale));
                    errorCheck(thisComposition->FirstChildElement("scale")->QueryFloatText(&compScale));
                    //find the objectData for the current object
                    XMLElement* objectData = compositions->FirstChildElement("objectData");
                    for(; objectData; objectData=objectData->NextSiblingElement("objectData")){
                        const char* charDataModelPath = objectData->FirstChildElement("modelPath")->GetText();
                        if(charDataModelPath == NULL){
                            printf("XMLError: No modelPath found in objectData.\n");
                            exit(-1);
                        }
                        std::string dataModelPath = charDataModelPath;
                        //objectData found
                        if(dataModelPath.compare(modelPath) == 0){
                            bool renderable;
                            errorCheck(objectData->FirstChildElement("renderable")->QueryBoolText(&renderable));
                            //create the object
                            Material material;
                            Model model;
                            if (renderable) {
                                float ambientFactor, diffuseFactor, specularFactor, shininess;
                                errorCheck(objectData->FirstChildElement("ambientFactor")->QueryFloatText(&ambientFactor));
                                errorCheck(objectData->FirstChildElement("diffuseFactor")->QueryFloatText(&diffuseFactor));
                                errorCheck(objectData->FirstChildElement("specularFactor")->QueryFloatText(&specularFactor));
                                errorCheck(objectData->FirstChildElement("shininess")->QueryFloatText(&shininess));
                                const char* charTexturePath = objectData->FirstChildElement("texturePath")->GetText();
                                if(charTexturePath == NULL){
                                    printf("XMLError: No texturePath found in objectData.\n");
                                    exit(-1);
                                }
                                std::string texturePath = charTexturePath;
                                material = Material(texturePath, ambientFactor, diffuseFactor, specularFactor, shininess);
                                model = Model(modelPath, objectScale * compScale);
                            }
                            float compXPos, compYOffset, compZPos;
                            glm::vec3 objectOffset, compRot;
                            errorCheck(xmlObject->FirstChildElement("xOffset")->QueryFloatText(&objectOffset[0]));
                            errorCheck(xmlObject->FirstChildElement("yOffset")->QueryFloatText(&objectOffset[1]));
                            errorCheck(xmlObject->FirstChildElement("zOffset")->QueryFloatText(&objectOffset[2]));
                            errorCheck(thisComposition->FirstChildElement("xPos")->QueryFloatText(&compXPos));
                            errorCheck(thisComposition->FirstChildElement("yOffset")->QueryFloatText(&compYOffset));
                            errorCheck(thisComposition->FirstChildElement("zPos")->QueryFloatText(&compZPos));
                            errorCheck(thisComposition->FirstChildElement("xRot")->QueryFloatText(&compRot[0]));
                            errorCheck(thisComposition->FirstChildElement("yRot")->QueryFloatText(&compRot[1]));
                            errorCheck(thisComposition->FirstChildElement("zRot")->QueryFloatText(&compRot[2]));
                            compRot *= 0.0174532925;    //transform degrees to radians
                            bool ignoreHeightmap;
                            errorCheck(composition->FirstChildElement("ignoreHeightmap")->QueryBoolText(&ignoreHeightmap));
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
                            errorCheck(xmlObject->FirstChildElement("xRot")->QueryFloatText(&objectRot[0]));
                            errorCheck(xmlObject->FirstChildElement("yRot")->QueryFloatText(&objectRot[1]));
                            errorCheck(xmlObject->FirstChildElement("zRot")->QueryFloatText(&objectRot[2]));
                            objectRot *= 0.0174532925;    //transform degrees to radians
                            Object* object = new Object(model, material, objectPosition, compRot+objectRot, renderable);
                            level->addObject(object);
                            
                            //add object to physics
                            const char* charPhysicType = objectData->FirstChildElement("physicType")->GetText();
                            if(charPhysicType == NULL){
                                printf("XMLError: No physicType found.\n");
                                exit(-1);
                            }
                            std::string physicType = charPhysicType;
                            float mass;
                            errorCheck(xmlObject->FirstChildElement("mass")->QueryFloatText(&mass));
                            XMLElement* constraint = thisComposition->FirstChildElement("positionConstraint");
                            bool rotate = (constraint == NULL);
                            if (physicType.compare("Player") == 0){
                                float radius, dampningL, dampningA;
                                errorCheck(objectData->FirstChildElement("dampningL")->QueryFloatText(&dampningL));
                                errorCheck(objectData->FirstChildElement("dampningA")->QueryFloatText(&dampningA));
                                errorCheck(objectData->FirstChildElement("radius")->QueryFloatText(&radius));
                                radius *= objectScale*compScale;
                                level->addPhysicsObject(object);
                                level->getPhysics()->addPlayer(friction, radius, *object, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize());
                            }else if (physicType.compare("Box") == 0){
                                float width, height, length, dampningL, dampningA;
                                errorCheck(objectData->FirstChildElement("dampningL")->QueryFloatText(&dampningL));
                                errorCheck(objectData->FirstChildElement("dampningA")->QueryFloatText(&dampningA));
                                errorCheck(objectData->FirstChildElement("width")->QueryFloatText(&width));
                                errorCheck(objectData->FirstChildElement("height")->QueryFloatText(&height));
                                errorCheck(objectData->FirstChildElement("length")->QueryFloatText(&length));
                                width  *= objectScale*compScale;
                                height *= objectScale*compScale;
                                length *= objectScale*compScale;
                                level->addPhysicsObject(object);
                                level->getPhysics()->addBox(width, height, length, *object, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize(), rotate);
                            }else if (physicType.compare("Button") == 0){
                                float width, height, length, dampningL, dampningA;
                                errorCheck(objectData->FirstChildElement("dampningL")->QueryFloatText(&dampningL));
                                errorCheck(objectData->FirstChildElement("dampningA")->QueryFloatText(&dampningA));
                                errorCheck(objectData->FirstChildElement("width")->QueryFloatText(&width));
                                errorCheck(objectData->FirstChildElement("height")->QueryFloatText(&height));
                                errorCheck(objectData->FirstChildElement("length")->QueryFloatText(&length));
                                width  *= objectScale*compScale;
                                height *= objectScale*compScale;
                                length *= objectScale*compScale;
                                level->addPhysicsObject(object);
                                level->getPhysics()->addButton(width, height, length, *object, mass, dampningL, dampningA, level->getPhysicsObjectsVectorSize(), rotate);
                            }else if (physicType.compare("TriangleMesh") == 0){
                                float dampningL, dampningA;
                                errorCheck(objectData->FirstChildElement("dampningL")->QueryFloatText(&dampningL));
                                errorCheck(objectData->FirstChildElement("dampningA")->QueryFloatText(&dampningA));
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
                            int idGreen, idBlue;
                            errorCheck(thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
                            errorCheck(thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
                            objectIdentifier[2] = idGreen;
                            objectIdentifier[3] = idBlue;
                            objectIdentifier[4] = objectNum;
                            objectIdentifiers.push_back(objectIdentifier);
                            
                            if(compositionType == 20){
                                level->setCameraCenter(object);
                            }
                        }//objectData found
                    }//finding the objectData
                    
                    objectNum = objectNum + 1;
                }//iterating over all objects of the composition
                
                //iterate over all lights of the composition
                XMLElement* xmlLight = composition->FirstChildElement("light");
                for(; xmlLight; xmlLight=xmlLight->NextSiblingElement("light")){
                    glm::vec3 compRot, lightOffset, lightColour;
                    float compScale, compXPos, compYOffset, compZPos, lightIntensity;
                    errorCheck(thisComposition->FirstChildElement("scale")->QueryFloatText(&compScale));
                    errorCheck(xmlLight->FirstChildElement("xOffset")->QueryFloatText(&lightOffset[0]));
                    errorCheck(xmlLight->FirstChildElement("yOffset")->QueryFloatText(&lightOffset[1]));
                    errorCheck(xmlLight->FirstChildElement("zOffset")->QueryFloatText(&lightOffset[2]));
                    errorCheck(thisComposition->FirstChildElement("xPos")->QueryFloatText(&compXPos));
                    errorCheck(thisComposition->FirstChildElement("yOffset")->QueryFloatText(&compYOffset));
                    errorCheck(thisComposition->FirstChildElement("zPos")->QueryFloatText(&compZPos));
                    errorCheck(thisComposition->FirstChildElement("xRot")->QueryFloatText(&compRot[0]));
                    errorCheck(thisComposition->FirstChildElement("yRot")->QueryFloatText(&compRot[1]));
                    errorCheck(thisComposition->FirstChildElement("zRot")->QueryFloatText(&compRot[2]));
                    errorCheck(xmlLight->FirstChildElement("rColour")->QueryFloatText(&lightColour[0]));
                    errorCheck(xmlLight->FirstChildElement("gColour")->QueryFloatText(&lightColour[1]));
                    errorCheck(xmlLight->FirstChildElement("bColour")->QueryFloatText(&lightColour[2]));
                    errorCheck(xmlLight->FirstChildElement("intensity")->QueryFloatText(&lightIntensity));
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
                    Light light = Light(lightPosition, lightColour, lightIntensity);
                    level->addLight(light);
                }//iterating over all lights of the composition
            }//corect composition found
        }//iterating over all compositions in Compositions.xml
    }//iterating over all compositions in Level.xml
    
    //load triggers
    XMLElement* composition = doc->FirstChildElement("composition");
    for(; composition; composition=composition->NextSiblingElement("composition")){
        XMLElement* xmlTrigger = composition->FirstChildElement("trigger");
        for(; xmlTrigger; xmlTrigger=xmlTrigger->NextSiblingElement("trigger")){
            const char* charName = xmlTrigger->FirstChildElement("name")->GetText();
            if(charName == NULL){
                printf("XMLError: No name found for a trigger.\n");
                exit(-1);
            }
            std::string name = charName;
            if (name.compare("-") != 0){
                float xPos, yPos, zPos, distance;
                bool isBigger;
                int idGreen, idBlue, objectNum;
                
                errorCheck(xmlTrigger->FirstChildElement("xPosition")->QueryFloatText(&xPos));
                errorCheck(xmlTrigger->FirstChildElement("yPosition")->QueryFloatText(&yPos));
                errorCheck(xmlTrigger->FirstChildElement("zPosition")->QueryFloatText(&zPos));
                glm::vec3 position = glm::vec3(xPos, yPos, zPos);
                const char* charTarget = xmlTrigger->FirstChildElement("targetIdGreen")->GetText();
                if(charTarget == NULL){
                    printf("XMLError: No targetIdGreen found for a trigger.\n");
                    exit(-1);
                }
                std::string stringTarget = charTarget;
                if (stringTarget.compare("-") != 0){
                    int targetIdGreen = 0, targetIdBlue = 0;
                    errorCheck(xmlTrigger->FirstChildElement("targetIdGreen")->QueryIntText(&targetIdGreen));
                    errorCheck(xmlTrigger->FirstChildElement("targetIdBlue")->QueryIntText(&targetIdBlue));
                    XMLElement* thisComposition = doc->FirstChildElement("composition");
                    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
                        int thisIdGreen, thisIdBlue;
                        errorCheck(thisComposition->FirstChildElement("idGreen")->QueryIntText(&thisIdGreen));
                        errorCheck(thisComposition->FirstChildElement("idBlue")->QueryIntText(&thisIdBlue));
                        if (targetIdGreen == thisIdGreen && targetIdBlue == thisIdBlue){
                            glm::vec3 targetPosition; 
                            errorCheck(thisComposition->FirstChildElement("xPos")->QueryFloatText(&targetPosition[0]));
                            errorCheck(thisComposition->FirstChildElement("yOffset")->QueryFloatText(&targetPosition[1]));
                            errorCheck(thisComposition->FirstChildElement("zPos")->QueryFloatText(&targetPosition[2]));
                            targetPosition[1] += level->getTerrain()->getHeightmap()[int(targetPosition[0]-0.5+0.5*level->getTerrain()->getHeightmapHeight())]
                                                                       [int(targetPosition[2]-0.5+0.5*level->getTerrain()->getHeightmapWidth())];
                            position += targetPosition;
                        }
                    }
                }
                errorCheck(xmlTrigger->FirstChildElement("distance")->QueryFloatText(&distance));
                errorCheck(xmlTrigger->FirstChildElement("isBiggerThan")->QueryBoolText(&isBigger));
                errorCheck(composition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
                errorCheck(composition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
                errorCheck(xmlTrigger->FirstChildElement("objectNum")->QueryIntText(&objectNum));
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
                const char* charLuaScript = xmlTrigger->FirstChildElement("luaScript")->GetText();
                if(charLuaScript == NULL){
                    printf("XMLError: No Lua script found for a trigger.\n");
                    exit(-1);
                }
                std::string luaScript = charLuaScript;
                
                int toChangeIdGreen, toChangeIdBlue, toChangeObjNum, objectToChange=-1;
                errorCheck(xmlTrigger->FirstChildElement("toChangeIdGreen")->QueryIntText(&toChangeIdGreen));
                errorCheck(xmlTrigger->FirstChildElement("toChangeIdBlue")->QueryIntText(&toChangeIdBlue));
                errorCheck(xmlTrigger->FirstChildElement("toChangeObjNum")->QueryIntText(&toChangeObjNum));
                for (unsigned int i = 0; i<objectIdentifiers.size(); i++){
                    if (objectIdentifiers[i][2]==toChangeIdGreen && objectIdentifiers[i][3]==toChangeIdBlue && objectIdentifiers[i][4]==toChangeObjNum){
                        objectToChange = objectIdentifiers[i][1];     //Index in physic objects
                    }
                }
                if (object != 0) {
                    Trigger trigger = Trigger(position, distance, isBigger, object, luaScript, level->getLuaState(), objectToChange, scriptPath);
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
            float xPos, yPos, zPos, strength;
            int objectNum=0, idGreen=0, idBlue=0, objectIndex=0;
            errorCheck(positionConstraint->FirstChildElement("xPosition")->QueryFloatText(&xPos));
            errorCheck(positionConstraint->FirstChildElement("yPosition")->QueryFloatText(&yPos));
            errorCheck(positionConstraint->FirstChildElement("zPosition")->QueryFloatText(&zPos));
            errorCheck(positionConstraint->FirstChildElement("strength")->QueryFloatText(&strength));
            errorCheck(positionConstraint->FirstChildElement("objectNum")->QueryIntText(&objectNum));
            errorCheck(composition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
            errorCheck(composition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
            bool ok = false;
            for (unsigned int i = 0; i<objectIdentifiers.size(); i++){
                if (objectIdentifiers[i][2]==idGreen && objectIdentifiers[i][3]==idBlue && objectIdentifiers[i][4]==objectNum){
                    objectIndex = objectIdentifiers[i][1];    //Index in physic objects
                    if(ok){
                        printf("2 objects have the same ID while loading constraints.");
                        exit(-1);
                    }
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
