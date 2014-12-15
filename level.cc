#include "level.hh"
#include <string>
using namespace tinyxml2;

//dowadiddydiddydumdiddydo
Level::Level(std::string levelNum){
    this->levelNum = levelNum;
    this->terrain = Terrain(levelNum);
    skydomeSize = 50.0f;
}

Level::Level() {
}
//there she was just walking down the street singing
Level::~Level() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        delete(objects.at(i));
    }
}

//dowadiddydiddydumdiddydoo!
void Level::errorCheck(XMLError error){
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
    }
}

void Level::load() {
    this->physics = Physics();
    this->physics.init();
    
    // currently hard coded should later read this stuff out of a file
    this->camera = Camera(glm::vec2(-0.8f, 0.0f), 3.0f);
    
    // load terrain
    this->terrain.load();
    Model terrainModel = Model(this->terrain.getModel());
    // load a texture:
    Material terrainMaterial = Material("seamlessTerrain.png", 0.1f, 0.8f, 0.2f, 3.0f);
    //Create object
    Object* terrainObject = new Object(terrainModel, terrainMaterial,
	glm::vec3(-0.5f*(float)this->terrain.getHeightmapHeight(), 0.0f, -0.5f*(float)this->terrain.getHeightmapWidth()),
        glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(terrainObject);
    
    //addTerrainPhysic
	physics.addTerrain(terrain.getHeightmapWidth(), terrain.getHeightmapHeight(), terrain.getHeightmap());

    //Loading from xml:
    XMLDocument* doc = new XMLDocument();
    const char* xmlFile = ("../Levels/ObjectSetups/Level" + levelNum + ".xml").c_str();
    doc->LoadFile(xmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open ObjectSetupXml!\n");
        exit(-1);
    }
    //load the skydome
    XMLElement* skydomeElement = doc->FirstChildElement("skydome");
    const char* charSkydomeTexture = skydomeElement->FirstChildElement("texture")->GetText();
    if(charSkydomeTexture == NULL){
        printf("XMLError: No skydomeTexture found.\n");
    }
    std::string skydomeTexture = charSkydomeTexture;
    Model skydomeModel = Model("skydome.obj", skydomeSize);
    Material skydomeMaterial = Material(skydomeTexture, 0.7f, 0.0f, 0.0f, 0.0f);
    Object* skydomeObject = new Object(skydomeModel, skydomeMaterial, glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(skydomeObject);
    skydome = skydomeObject;

    //load lighting parameters
    float rColour, gColour, bColour, alpha, xOffset, yOffset, zOffset, intensity;
    XMLElement* ambientElement = doc->FirstChildElement("ambientLight");
    errorCheck(ambientElement->FirstChildElement("rColour")->QueryFloatText(&rColour));
    errorCheck(ambientElement->FirstChildElement("gColour")->QueryFloatText(&gColour));
    errorCheck(ambientElement->FirstChildElement("bColour")->QueryFloatText(&bColour));
    ambientLight = glm::vec3(rColour,gColour,bColour);
    XMLElement* fogElement = doc->FirstChildElement("fogColour");
    errorCheck(fogElement->FirstChildElement("rColour")->QueryFloatText(&rColour));
    errorCheck(fogElement->FirstChildElement("gColour")->QueryFloatText(&gColour));
    errorCheck(fogElement->FirstChildElement("bColour")->QueryFloatText(&bColour));
    errorCheck(fogElement->FirstChildElement("alpha")->QueryFloatText(&alpha));
    fogColour = glm::vec4(rColour,gColour,bColour, alpha);
    XMLElement* directionalElement = doc->FirstChildElement("directionalLight");
    errorCheck(directionalElement->FirstChildElement("xOffset")->QueryFloatText(&xOffset));
    errorCheck(directionalElement->FirstChildElement("yOffset")->QueryFloatText(&yOffset));
    errorCheck(directionalElement->FirstChildElement("zOffset")->QueryFloatText(&zOffset));
    errorCheck(directionalElement->FirstChildElement("rColour")->QueryFloatText(&rColour));
    errorCheck(directionalElement->FirstChildElement("gColour")->QueryFloatText(&gColour));
    errorCheck(directionalElement->FirstChildElement("bColour")->QueryFloatText(&bColour));
    errorCheck(directionalElement->FirstChildElement("intensity")->QueryFloatText(&intensity));
    directionalLight = Light(glm::vec3(xOffset,yOffset,zOffset), glm::vec3(rColour,gColour,bColour), intensity);

    //load Objects
    std::vector<std::vector<int>> objectIdentifiers = std::vector<std::vector<int>>();  //The first entry is the index in objects, the others are idGreen, idBlue and objectNum.  
    XMLDocument* compositions = new XMLDocument();
    const char* compositionsFile = "../Levels/ObjectSetups/Compositions.xml";
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
                    }
                    std::string modelPath = charModelPath;
                    float objectScale, compScale;
                    errorCheck(xmlObject->FirstChildElement("scale")->QueryFloatText(&objectScale));
                    errorCheck(thisComposition->FirstChildElement("scale")->QueryFloatText(&compScale));
                    Model model = Model(modelPath, objectScale * compScale);
                    //find the objectData for the current object
                    XMLElement* objectData = compositions->FirstChildElement("objectData");
                    for(; objectData; objectData=objectData->NextSiblingElement("objectData")){
                        const char* charDataModelPath = objectData->FirstChildElement("modelPath")->GetText();
                        if(charDataModelPath == NULL){
                            printf("XMLError: No modelPath found in objectData.\n");
                        }
                        std::string dataModelPath = charDataModelPath;
                        //objectData found
                        if(dataModelPath.compare(modelPath) == 0){
                            //create the object
                            float ambientFactor, diffuseFactor, specularFactor, shininess;
                            errorCheck(objectData->FirstChildElement("ambientFactor")->QueryFloatText(&ambientFactor));
                            errorCheck(objectData->FirstChildElement("diffuseFactor")->QueryFloatText(&diffuseFactor));
                            errorCheck(objectData->FirstChildElement("specularFactor")->QueryFloatText(&specularFactor));
                            errorCheck(objectData->FirstChildElement("shininess")->QueryFloatText(&shininess));
                            const char* charTexturePath = objectData->FirstChildElement("texturePath")->GetText();
                            if(charTexturePath == NULL){
                                printf("XMLError: No texturePath found in objectData.\n");
                            }
                            std::string texturePath = charTexturePath;
                            Material material = Material(texturePath, ambientFactor, diffuseFactor, specularFactor, shininess);
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
                            glm::vec3 compPos = glm::vec3(compXPos,
                                                          compYOffset+terrain.getHeightmap()[int(compXPos-0.5+0.5*terrain.getHeightmapHeight())]
                                                                                            [int(compZPos-0.5+0.5*terrain.getHeightmapWidth())],
                                                          compZPos);
                            objectOffset = objectOffset * compScale;
                            glm::vec4 rotatedObjectOffset = glm::rotate(compRot.x, glm::vec3(1.0f, 0.0f, 0.0f))
                                                            * glm::rotate(compRot.y, glm::vec3(0.0f, 1.0f, 0.0f))
                                                            * glm::rotate(compRot.z, glm::vec3(0.0f, 0.0f, 1.0f))
                                                            * glm::vec4(objectOffset, 0);
                            glm::vec3 objectPosition = compPos + glm::vec3(rotatedObjectOffset.x,rotatedObjectOffset.y,rotatedObjectOffset.z);
                            Object* object = new Object(model, material, objectPosition, compRot);
                            objects.push_back(object);
                            //create an identifier for this object
                            std::vector<int> objectIdentifier = std::vector<int>(4);
                            objectIdentifier[0] = objects.size()-1;
                            int idGreen, idBlue;
                            errorCheck(thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
                            errorCheck(thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
                            objectIdentifier[1] = idGreen;
                            objectIdentifier[2] = idBlue;
                            objectIdentifier[3] = objectNum;
                            objectIdentifiers.push_back(objectIdentifier);

                            physicObjects.push_back(object);
                            const char* charPhysicType = objectData->FirstChildElement("physicType")->GetText();
                            if(charPhysicType == NULL){
                                printf("XMLError: No physicType found.\n");
                            }
                            std::string physicType = charPhysicType;
                            //add object to physics
                            float mass;
                            errorCheck(xmlObject->FirstChildElement("mass")->QueryFloatText(&mass));
                            if (physicType.compare("Player") == 0){
                                float radius;
                                errorCheck(objectData->FirstChildElement("radius")->QueryFloatText(&radius));
                                this->physics.addPlayer(radius, *object, mass, physicObjects.size());
                            }else if (physicType.compare("Box") == 0){
                                float width, height, length;
                                errorCheck(objectData->FirstChildElement("width")->QueryFloatText(&width));
                                errorCheck(objectData->FirstChildElement("height")->QueryFloatText(&height));
                                errorCheck(objectData->FirstChildElement("length")->QueryFloatText(&length));
                                this->physics.addBox(width, height, length, *object, mass, physicObjects.size());
                            }else if (physicType.compare("TriangleMesh") == 0){
                                float dampningL, dampningA;
                                errorCheck(objectData->FirstChildElement("dampningL")->QueryFloatText(&dampningL));
                                errorCheck(objectData->FirstChildElement("dampningA")->QueryFloatText(&dampningA));
                                this->physics.addTriangleMeshBody(*object, modelPath, mass, dampningL, dampningA, physicObjects.size());
                            } else{
                                printf("XMLError: Not a valid physicType.\n");
                            }

                            if(compositionType == 20){
                                cameraCenter = object;
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
                                        compYOffset+terrain.getHeightmap()[int(compXPos-0.5+0.5*terrain.getHeightmapHeight())]
                                                                          [int(compZPos-0.5+0.5*terrain.getHeightmapWidth())],
                                        compZPos);
                    lightOffset = lightOffset * compScale;
                    glm::vec4 rotatedLightOffset = glm::rotate(compRot.x, glm::vec3(1.0f, 0.0f, 0.0f))
                                                    * glm::rotate(compRot.y, glm::vec3(0.0f, 1.0f, 0.0f))
                                                    * glm::rotate(compRot.z, glm::vec3(0.0f, 0.0f, 1.0f))
                                                    * glm::vec4(lightOffset, 0);
                    glm::vec3 lightPosition = compPos + glm::vec3(rotatedLightOffset.x,rotatedLightOffset.y,rotatedLightOffset.z);
                    Light light = Light(lightPosition, lightColour, lightIntensity);
                    lights.push_back(light);
                }//iterating over all lights of the composition
            }//corect composition found
        }//iterating over all compositions in Compositions.xml
    }//iterating over all compositions in Level.xml
    
    //load triggers
    XMLElement* xmlTrigger = doc->FirstChildElement("trigger");
    for(; xmlTrigger; xmlTrigger=xmlTrigger->NextSiblingElement("trigger")){
        const char* charName = xmlTrigger->FirstChildElement("name")->GetText();
        if(charName == NULL){
            printf("XMLError: No name found for a trigger.\n");
        }
        std::string name = charName;
        if (name.compare("-") != 0){
            float xPos, yPos, zPos, distance;
            glm::vec3 position;
            bool isBigger;
            int idGreen, idBlue, objectNum, functionPointer_int;
            errorCheck(xmlTrigger->FirstChildElement("xPosition")->QueryFloatText(&xPos));
            errorCheck(xmlTrigger->FirstChildElement("yPosition")->QueryFloatText(&yPos));
            errorCheck(xmlTrigger->FirstChildElement("zPosition")->QueryFloatText(&zPos));
            errorCheck(xmlTrigger->FirstChildElement("distance")->QueryFloatText(&distance));
            position = glm::vec3(xPos, yPos, zPos);
            errorCheck(xmlTrigger->FirstChildElement("isBiggerThan")->QueryBoolText(&isBigger));
            errorCheck(xmlTrigger->FirstChildElement("idGreen")->QueryIntText(&idGreen));
            errorCheck(xmlTrigger->FirstChildElement("idBlue")->QueryIntText(&idBlue));
            errorCheck(xmlTrigger->FirstChildElement("objectNum")->QueryIntText(&objectNum));
            Object* object=0;
            for (unsigned int i = 0; i<objectIdentifiers.size(); i++){
                                printf("objectIdentifiers: %d, %d, %d, %d\n", objectIdentifiers[i][0], objectIdentifiers[i][1], objectIdentifiers[i][2], objectIdentifiers[i][3]);
                if (objectIdentifiers[i][1]==idGreen && objectIdentifiers[i][2]==idBlue && objectIdentifiers[i][3]==objectNum){
                    object = objects[objectIdentifiers[i][0]];
                }
            }
            errorCheck(xmlTrigger->FirstChildElement("functionPointer")->QueryIntText(&functionPointer_int));
            void (*functionPointer)() = NULL;
            switch(functionPointer_int) {
                case 0:
                    functionPointer =  &trigger_function_0;
                    break;
                case 1:
                    functionPointer =  &trigger_function_1;
                    break;
                case 2:
                    functionPointer =  &trigger_function_2;
                    break;
                case 3:
                    functionPointer =  &trigger_function_3;
                    break;
                case 4:
                    functionPointer =  &trigger_function_4;
                    break;
                default:
                    printf("Trigger function could not be found.");
            }
            if (object != 0) {
                Trigger trigger = Trigger(position, distance, isBigger, object, functionPointer);
                triggers.push_back(trigger);
            }
            else {
                printf("Trigger object not found.\n");
            }
        }
    }
}

void Level::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass,
        glm::mat4* viewProjectionMatrix, std::vector<glm::mat4>* shadowVPs) {
    for(unsigned int i = 0; i<objects.size(); i++) {
        // do not project shadow of skydome 
        if(lightingPass || objects.at(i) != skydome) {
            objects.at(i)->render(shader, lightingPass, viewProjectionMatrix, shadowVPs);
        }
    }
}

void Level::update(float runTime, glm::vec2 mouseDelta, bool wPressed, bool aPressed, bool sPressed, bool dPressed) {
    // Ignore first two mouse updates, because they are incorrect
    // DON'T try to move this functionallity elsewhere
    static int i = 0;
    if (i <2) {
        i++;
    }
    else {
        mouseDelta.x = -mouseDelta.x;
        camera.updateRotation(mouseDelta/100.0f);
    }    
    
    float str = 30;
    
    if(wPressed){
        physics.rollForward(camera.getVector(),str);
    }
    if(aPressed) {
        physics.rollLeft(camera.getVector(),str);
    }
    if(sPressed) {
        physics.rollBack(camera.getVector(),str);
    }
    if(dPressed){
        physics.rollRight(camera.getVector(),str);
    }
    
    physics.takeUpdateStep(runTime);
    
    cameraCenter->setPosition(physics.getPos(0));
    cameraCenter->setRotation(physics.getRotation(0));
    
    for(unsigned i = 0; i < physicObjects.size();i++)
    {
        physicObjects[i]->setPosition(physics.getPos(i));
        physicObjects[i]->setRotation(physics.getRotation(i));
    }
    
    skydome->setPosition(glm::vec3(cameraCenter->getPosition().x, 
        0.0f, cameraCenter->getPosition().z));
        
    for(unsigned int i = 0; i<triggers.size(); i++) {
        triggers.at(i).triggerUpdate();
    }
}

glm::vec3 Level::getAmbientLight() {
    return ambientLight;
}

std::vector<Light>* Level::getLights() {
    return &lights;
}

Camera* Level::getCamera() {
    return &camera;
}

Object* Level::getCameraCenter() {
    return cameraCenter;
}

Light* Level::getDirectionalLight() {
    return &directionalLight;
}

glm::vec4 Level::getFogColour() {
    return fogColour;
}

glm::vec3 Level::getCameraPosition() {
   return cameraCenter->getPosition() + camera.getVector();
}

void Level::setSkydomeSize(float size) {
    skydomeSize = size;
}

void Level::trigger_function_0() {
    exit(0);
}

void Level::trigger_function_1() {
}

void Level::trigger_function_2() {
}

void Level::trigger_function_3() {
}

void Level::trigger_function_4() {
}
