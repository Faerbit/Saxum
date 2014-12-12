#include "level.hh"
#include <string>
using namespace tinyxml2;


Level::Level(std::string levelNum){
    this->levelNum = levelNum;
    this->terrain = Terrain(levelNum);
    skydomeSize = 50.0f;
}

Level::Level() {
}

Level::~Level() {
    for(unsigned int i = 0; i<objects.size(); i++) {
        delete(objects.at(i));
    }
}


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
    XMLElement* fogElement = doc->FirstChildElement("ambientLight");
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
    XMLDocument* compositions = new XMLDocument();
    const char* compositionsFile = "../Levels/ObjectSetups/Compositions.xml";
    compositions->LoadFile(compositionsFile);
    if (compositions->ErrorID()!=0){
        printf("Could not open Compositions!\n");
        exit(-1);
    }
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        int thisType = 0;
        errorCheck(thisComposition->FirstChildElement("typeID")->QueryIntText(&thisType));
        XMLElement* composition = compositions->FirstChildElement("composition");
        for(; composition; composition=composition->NextSiblingElement("composition")){
            int compositionType = 0;
            errorCheck(composition->FirstChildElement("typeID")->QueryIntText(&compositionType));
            if(thisType == compositionType){
                XMLElement* object = composition->FirstChildElement("object");
                for(; object; object=object->NextSiblingElement("object")){
                    const char* charModelPath = object->FirstChildElement("modelPath")->GetText();
                    if(charModelPath == NULL){
                        printf("XMLError: No modelPath found in object.\n");
                    }
                    std::string modelPath = charModelPath;
                    float objectScale, compScale;
                    errorCheck(object->FirstChildElement("scale")->QueryFloatText(&objectScale));
                    errorCheck(thisComposition->FirstChildElement("scale")->QueryFloatText(&compScale));
                    Model model = Model(modelPath, objectScale * compScale);
                    XMLElement* objectData = compositions->FirstChildElement("objectData");
                    for(; objectData; objectData=objectData->NextSiblingElement("objectData")){
                        const char* charDataModelPath = objectData->FirstChildElement("modelPath")->GetText();
                        if(charDataModelPath == NULL){
                            printf("XMLError: No modelPath found in objectData.\n");
                        }
                        std::string dataModelPath = charDataModelPath;
                        if(dataModelPath == modelPath){
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
                            errorCheck(object->FirstChildElement("xOffset")->QueryFloatText(&objectOffset[0]));
                            errorCheck(object->FirstChildElement("yOffset")->QueryFloatText(&objectOffset[1]));
                            errorCheck(object->FirstChildElement("zOffset")->QueryFloatText(&objectOffset[2]));
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
                            //physicObjects.push_back(object);
                            const char* charPhysicType = objectData->FirstChildElement("physicType")->GetText();
                            if(charPhysicType == NULL){
                                printf("XMLError: No physicType found.\n");
                            }
                            std::string physicType = charPhysicType;
                            //TODO switch (physicType) and add object to physics
                            //if(compositionType == 20){
                            //    cameraCenter = object;
                            //}
                        }
                    }
                }
                XMLElement* light = composition->FirstChildElement("light");
                for(; light; light=light->NextSiblingElement("light")){
                    glm::vec3 compRot, lightOffset, lightColour;
                    float compScale, compXPos, compYOffset, compZPos, lightIntensity;
                    errorCheck(thisComposition->FirstChildElement("scale")->QueryFloatText(&compScale));
                    errorCheck(light->FirstChildElement("xOffset")->QueryFloatText(&lightOffset[0]));
                    errorCheck(light->FirstChildElement("yOffset")->QueryFloatText(&lightOffset[1]));
                    errorCheck(light->FirstChildElement("zOffset")->QueryFloatText(&lightOffset[2]));
                    errorCheck(thisComposition->FirstChildElement("xPos")->QueryFloatText(&compXPos));
                    errorCheck(thisComposition->FirstChildElement("yOffset")->QueryFloatText(&compYOffset));
                    errorCheck(thisComposition->FirstChildElement("zPos")->QueryFloatText(&compZPos));
                    errorCheck(thisComposition->FirstChildElement("xRot")->QueryFloatText(&compRot[0]));
                    errorCheck(thisComposition->FirstChildElement("yRot")->QueryFloatText(&compRot[1]));
                    errorCheck(thisComposition->FirstChildElement("zRot")->QueryFloatText(&compRot[2]));
                    errorCheck(light->FirstChildElement("rColour")->QueryFloatText(&lightColour[0]));
                    errorCheck(light->FirstChildElement("gColour")->QueryFloatText(&lightColour[1]));
                    errorCheck(light->FirstChildElement("bColour")->QueryFloatText(&lightColour[2]));
                    errorCheck(light->FirstChildElement("intensity")->QueryFloatText(&lightIntensity));
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
                }
            }
        }
    }
    
    
    //add player
    Model marbleModel = Model("marbleSmooth.obj", 0.75f);
    Material marbleMaterial = Material("marbleTexture_small.png", 0.1f, 0.5f, 0.5f, 3.0f);
    Object* object = new Object(marbleModel, marbleMaterial, glm::vec3(2.0f, 10.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f));
    objects.push_back(object);    
    physicObjects.push_back(object);
    this->physics.addPlayer(1.25f,*object,8.0f,physicObjects.size());
    cameraCenter = object;
}

void Level::render(ACGL::OpenGL::SharedShaderProgram shader, bool lightingPass) {
    for(unsigned int i = 0; i<objects.size(); i++) {
        // do not project shadow of skydome 
        if(lightingPass || objects.at(i) != skydome) {
            objects.at(i)->render(shader, lightingPass);
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
    
    float str = 20;
    
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
}

glm::vec3 Level::getAmbientLight() {
    return ambientLight;
}

std::vector<Light> Level::getLights() {
    return lights;
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
