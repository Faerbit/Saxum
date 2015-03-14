#include "converter.hh"
#include <fstream>
#include <sys/stat.h>
#include <iostream>

using namespace tinyxml2;

Converter::Converter(std::string levelPath, std::string levelName, std::string compositionsPath){
    xmlFile = levelPath + levelName + ".xml";
    
    //Load Compositions
    std::string stringCompositions = compositionsPath;
    const char* charCompositions = stringCompositions.c_str();
    compositions->LoadFile(charCompositions);
    if (compositions->ErrorID()!=0){
        printf("Could not open Compositions!\n");
        exit(-1);
    }
    
    //Create a backup of the current Level png file, if no backup exists
    std::string pngFile = levelPath + levelName + ".png";
    std::string backupPNG = levelPath  + levelName + "Backup.png";
    struct stat buf;
    if(stat(backupPNG.c_str(), &buf) != 0){
        std::ifstream  src(pngFile, std::ios::binary);
        std::ofstream  dst(backupPNG,  std::ios::binary);
        dst << src.rdbuf();
    }
    
    //Load the Level xml file
    nextID.push_back(0);
    nextID.push_back(1);
    for (int i=0; i<256; i++){
        for (int j=0; j<256; j++){
            idUsed[i][j] = false;
        }
    }
    idUsed[0][0] = true;
    const char* charXmlFile = xmlFile.c_str();
    doc->LoadFile(charXmlFile);
    //check if the xml file did not already exist
    if (doc->ErrorID()!=0){
        std::string answer;
        printf("Could not open xml, do you want to create a new xml? (y/n)\n");
        std::cin >> answer;
        while(answer.compare("y") != 0 && answer.compare("n") != 0){
            printf("Answer with y or n\n");
            std::cin >> answer;
        }
        if(answer.compare("n") == 0){
            exit(-1);
        }
        printf("Creating new xml.\n");
        //Create all global Lightingparameters with Dummy-Values
        std::vector<XMLElement*> lightAttributes;
        lightAttributes.push_back(doc->NewElement("xOffset"));
        lightAttributes.push_back(doc->NewElement("yOffset"));
        lightAttributes.push_back(doc->NewElement("zOffset"));
        lightAttributes.push_back(doc->NewElement("rColour"));
        lightAttributes.push_back(doc->NewElement("gColour"));
        lightAttributes.push_back(doc->NewElement("bColour"));
        lightAttributes.push_back(doc->NewElement("intensity"));
        XMLElement* rColourAmbient = doc->NewElement("rColour");
        XMLElement* gColourAmbient = doc->NewElement("gColour");
        XMLElement* bColourAmbient = doc->NewElement("bColour");
        XMLElement* rColourFogDay = doc->NewElement("rColour");
        XMLElement* gColourFogDay = doc->NewElement("gColour");
        XMLElement* bColourFogDay = doc->NewElement("bColour");
        XMLElement* alphaFogDay = doc->NewElement("alpha");
        XMLElement* rColourFogRise = doc->NewElement("rColour");
        XMLElement* gColourFogRise = doc->NewElement("gColour");
        XMLElement* bColourFogRise = doc->NewElement("bColour");
        XMLElement* alphaFogRise = doc->NewElement("alpha");
        XMLElement* rColourFogNight = doc->NewElement("rColour");
        XMLElement* gColourFogNight = doc->NewElement("gColour");
        XMLElement* bColourFogNight = doc->NewElement("bColour");
        XMLElement* alphaFogNight = doc->NewElement("alpha");
        
        lightAttributes[0]->SetText("-1.0");
        lightAttributes[1]->SetText("1.5");
        lightAttributes[2]->SetText("1.0");
        lightAttributes[3]->SetText("1.0");
        lightAttributes[4]->SetText("1.0");
        lightAttributes[5]->SetText("0.9");
        lightAttributes[6]->SetText("0.2");
        rColourAmbient->SetText("1.0");
        gColourAmbient->SetText("1.0");
        bColourAmbient->SetText("1.0");
        rColourFogDay->SetText("0.57");
        gColourFogDay->SetText("0.80");
        bColourFogDay->SetText("0.98");
        alphaFogDay->SetText("1.0");
        rColourFogRise->SetText("0.88");
        gColourFogRise->SetText("0.38");
        bColourFogRise->SetText("0.38");
        alphaFogRise->SetText("1.0");
        rColourFogNight->SetText("0.09");
        gColourFogNight->SetText("0.1");
        bColourFogNight->SetText("0.24");
        alphaFogNight->SetText("1.0");
        
        XMLElement* ambientLight = doc->NewElement("ambientLight");
        XMLElement* fogColourDay = doc->NewElement("fogColourDay");
        XMLElement* fogColourRise = doc->NewElement("fogColourRise");
        XMLElement* fogColourNight = doc->NewElement("fogColourNight");
        XMLElement* directionalLight = doc->NewElement("directionalLight");
        
        ambientLight->InsertEndChild(rColourAmbient);
        ambientLight->InsertEndChild(gColourAmbient);
        ambientLight->InsertEndChild(bColourAmbient);
        fogColourDay->InsertEndChild(rColourFogDay);
        fogColourDay->InsertEndChild(gColourFogDay);
        fogColourDay->InsertEndChild(bColourFogDay);
        fogColourDay->InsertEndChild(alphaFogDay);
        fogColourRise->InsertEndChild(rColourFogRise);
        fogColourRise->InsertEndChild(gColourFogRise);
        fogColourRise->InsertEndChild(bColourFogRise);
        fogColourRise->InsertEndChild(alphaFogRise);
        fogColourNight->InsertEndChild(rColourFogNight);
        fogColourNight->InsertEndChild(gColourFogNight);
        fogColourNight->InsertEndChild(bColourFogNight);
        fogColourNight->InsertEndChild(alphaFogNight);
        for(int i=0;i<7;i++){
            directionalLight->InsertEndChild(lightAttributes[i]);
        }
        doc->InsertEndChild(ambientLight);
        doc->InsertEndChild(fogColourDay);
        doc->InsertEndChild(fogColourRise);
        doc->InsertEndChild(fogColourNight);
        doc->InsertEndChild(directionalLight);
        
        //Create global terrain Element
        XMLElement* terrain = doc->NewElement("terrain");
        XMLElement* terrainHeightmap = doc->NewElement("heightmap");
        XMLElement* terrainTexture = doc->NewElement("texture");
        XMLElement* terrainAmbientFactor = doc->NewElement("ambientFactor");
        XMLElement* terrainDiffuseFactor = doc->NewElement("diffuseFactor");
        XMLElement* terrainSpecularFactor = doc->NewElement("specularFactor");
        XMLElement* terrainShininess = doc->NewElement("shininess");
        std::string heightmapPath = "heightmap" + levelName + ".png";
        terrainHeightmap->SetText(heightmapPath.c_str());
        terrainTexture->SetText("terrainTexture.png");
        terrainAmbientFactor->SetText("0.1");
        terrainDiffuseFactor->SetText("0.8");
        terrainSpecularFactor->SetText("0.2");
        terrainShininess->SetText("1.0");
        terrain->InsertEndChild(terrainHeightmap);
        terrain->InsertEndChild(terrainTexture);
        terrain->InsertEndChild(terrainAmbientFactor);
        terrain->InsertEndChild(terrainDiffuseFactor);
        terrain->InsertEndChild(terrainSpecularFactor);
        terrain->InsertEndChild(terrainShininess);
        doc->InsertEndChild(terrain);
        
        //Create global skydome Element
        XMLElement* skydome = doc->NewElement("skydome");
        XMLElement* skydomeModel = doc->NewElement("model");
        XMLElement* skydomeTexture = doc->NewElement("texture");
        XMLElement* skydomeTextureNight = doc->NewElement("nightTexture");
        skydomeModel->SetText("skydome.obj");
        skydomeTexture->SetText("skydome.png");
        skydomeTextureNight->SetText("skydomeNight.png");
        skydome->InsertEndChild(skydomeModel);
        skydome->InsertEndChild(skydomeTexture);
        skydome->InsertEndChild(skydomeTextureNight);
        doc->InsertEndChild(skydome);
        
        //Create global physics parameters
        XMLElement* physics = doc->NewElement("physics");
        XMLElement* playerFriction = doc->NewElement("friction");
        XMLElement* playerStrength = doc->NewElement("strength");
        playerFriction->SetText("0.9");
        playerStrength->SetText("300.0");
        physics->InsertEndChild(playerFriction);
        physics->InsertEndChild(playerStrength);
        doc->InsertEndChild(physics);
        
        //Create positionConstraint Dummy
        XMLElement* positionConstraint = doc->NewElement("positionConstraint");
        XMLElement* positionConstraintObjectNum = doc->NewElement("objectNum");
        XMLElement* positionConstraintXPos = doc->NewElement("xPosition");
        XMLElement* positionConstraintYPos = doc->NewElement("yPosition");
        XMLElement* positionConstraintZPos = doc->NewElement("zPosition");
        XMLElement* positionConstraintStrength = doc->NewElement("strength");
        positionConstraintObjectNum->SetText("0");
        positionConstraintXPos->SetText("0.0");
        positionConstraintYPos->SetText("0.0");
        positionConstraintZPos->SetText("0.0");
        positionConstraintStrength->SetText("100.0");
        positionConstraint->InsertEndChild(positionConstraintObjectNum);
        positionConstraint->InsertEndChild(positionConstraintXPos);
        positionConstraint->InsertEndChild(positionConstraintYPos);
        positionConstraint->InsertEndChild(positionConstraintZPos);
        positionConstraint->InsertEndChild(positionConstraintStrength);
        doc->InsertEndChild(positionConstraint);
    }else{
        //Create a backup of the current Level xml file
        std::string backupXML = levelPath + levelName + "Backup.xml";
        std::ifstream  src(xmlFile, std::ios::binary);
        std::ofstream  dst(backupXML,  std::ios::binary);
        dst << src.rdbuf();
        //Check what IDs are already in use
        XMLElement* thisComposition = doc->FirstChildElement("composition");
        for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
            int idGreen = queryInt(thisComposition, "idGreen");
            int idBlue = queryInt(thisComposition, "idBlue");
            idUsed[idGreen][idBlue] = true;
        }
    }
}

Converter::Converter(){
}

Converter::~Converter(){
}

std::vector<int> Converter::newComposition(int type, float posX, float posZ){
    int oldIDGreen, oldIDBlue;
    bool alreadyExists = false;
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        float xPos = queryFloat(thisComposition, "xPos");
        float zPos = queryFloat(thisComposition, "zPos");
        int typeID = queryInt(thisComposition, "typeID");
        if(xPos == posX && zPos == posZ && typeID == type){
            if(alreadyExists){
                std::cout << "At the position " << xPos << "," << zPos << " multiple compositions with the ID " << typeID << " exist in the xml." << std::endl;
                exit(-1);
            }
            oldIDGreen = queryInt(thisComposition, "idGreen");
            oldIDBlue = queryInt(thisComposition, "idBlue");
            alreadyExists = true;
        }
    }
    if (alreadyExists){
        if (! idUsed[oldIDGreen][oldIDBlue]){
            std::cout << "The composition with ID " << oldIDGreen << "," << oldIDBlue << " exists in the xml but the converter thinks it does not." << std::endl;
            exit(-1);
        }
        std::vector<int> oldID;
        oldID.push_back(oldIDGreen);
        oldID.push_back(oldIDBlue);
        return oldID;
    }
    
    bool full = false;
    while(idUsed[nextID[0]][nextID[1]]){
        nextID[1] += 1;
        if (nextID[1] == 256){
            nextID[1] = 0;
            nextID[0] +=1;
            if (nextID[0] == 256){
                if(full){
                    printf("Can not have more than 65535 compositions.\n");
                    exit(-1);
                }
                nextID[0] = 0;
                full = true;
            }
        }
    }
    
    XMLElement* newComposition = doc->NewElement("composition");
    XMLElement* typeID = doc->NewElement("typeID");
    XMLElement* idBlue = doc->NewElement("idBlue");
    XMLElement* idGreen = doc->NewElement("idGreen");
    XMLElement* zPos = doc->NewElement("zPos");
    XMLElement* yOffset = doc->NewElement("yOffset");
    XMLElement* xPos  = doc->NewElement("xPos");
    XMLElement* manualPos  = doc->NewElement("manualPos");
    XMLElement* zRot  = doc->NewElement("zRot");
    XMLElement* yRot  = doc->NewElement("yRot");
    XMLElement* xRot  = doc->NewElement("xRot");
    XMLElement* scale = doc->NewElement("scale");
    
    typeID->SetText(std::to_string(type).c_str());
    idBlue->SetText(std::to_string(nextID[1]).c_str());
    idGreen->SetText(std::to_string(nextID[0]).c_str());
    zPos->SetText(std::to_string(posZ).c_str());
    yOffset->SetText("0.0");
    xPos->SetText(std::to_string(posX).c_str());
    manualPos->SetText("false");
    zRot->SetText("0.0");
    yRot->SetText("0.0");
    xRot->SetText("0.0");
    scale->SetText("1.0");
    
    newComposition->InsertFirstChild(typeID);
    newComposition->InsertFirstChild(idBlue);
    newComposition->InsertFirstChild(idGreen);
    newComposition->InsertFirstChild(zPos);
    newComposition->InsertFirstChild(yOffset);
    newComposition->InsertFirstChild(xPos);
    newComposition->InsertFirstChild(manualPos);
    newComposition->InsertFirstChild(zRot);
    newComposition->InsertFirstChild(yRot);
    newComposition->InsertFirstChild(xRot);
    newComposition->InsertFirstChild(scale);
    
    //Create a Dummy-trigger
    XMLElement* trigger = doc->NewElement("trigger");
    newComposition->InsertEndChild(trigger);
    XMLElement* name = doc->NewElement("name");
    XMLElement* undo = doc->NewElement("undo");
    XMLElement* xPosition = doc->NewElement("xPosition");
    XMLElement* yPosition = doc->NewElement("yPosition");
    XMLElement* zPosition = doc->NewElement("zPosition");
    XMLElement* targetIdGreen = doc->NewElement("targetIdGreen");
    XMLElement* targetIdBlue = doc->NewElement("targetIdBlue");
    XMLElement* distance = doc->NewElement("distance");
    XMLElement* isBiggerThan = doc->NewElement("isBiggerThan");
    XMLElement* objectNum = doc->NewElement("objectNum");
    XMLElement* luaScript = doc->NewElement("luaScript");
    XMLElement* toChangeIdGreen = doc->NewElement("toChangeIdGreen");
    XMLElement* toChangeIdBlue = doc->NewElement("toChangeIdBlue");
    XMLElement* toChangeObjNum = doc->NewElement("toChangeObjNum");
    
    name->SetText("-");
    undo->SetText("false");
    xPosition->SetText("0");
    yPosition->SetText("0");
    zPosition->SetText("0");
    targetIdGreen->SetText("-");
    targetIdBlue->SetText("-");
    distance->SetText("1.0");
    isBiggerThan->SetText("false");
    objectNum->SetText("0");
    luaScript->SetText("-");
    toChangeIdGreen->SetText("0");
    toChangeIdBlue->SetText("0");
    toChangeObjNum->SetText("0");
    
    trigger->InsertEndChild(name);
    trigger->InsertEndChild(undo);
    trigger->InsertEndChild(xPosition);
    trigger->InsertEndChild(yPosition);
    trigger->InsertEndChild(zPosition);
    trigger->InsertEndChild(targetIdGreen);
    trigger->InsertEndChild(targetIdBlue);
    trigger->InsertEndChild(distance);
    trigger->InsertEndChild(isBiggerThan);
    trigger->InsertEndChild(objectNum);
    trigger->InsertEndChild(luaScript);
    trigger->InsertEndChild(toChangeIdGreen);
    trigger->InsertEndChild(toChangeIdBlue);
    trigger->InsertEndChild(toChangeObjNum);
    
    idUsed[nextID[0]][nextID[1]] = true;
    doc->InsertFirstChild(newComposition);
    return nextID;
}

void Converter::updateComposition(int idG, int idB, float posX, float posZ){
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    bool compositionExists = false;
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        int idGreen = queryInt(thisComposition, "idGreen");
        int idBlue = queryInt(thisComposition, "idBlue");
        if(idGreen == idG && idBlue == idB){
            if (compositionExists){
                std::cout << "The ID " << idGreen << "," << idBlue << " is used for multiple compositions in the xml." << std::endl;
                exit(-1);
            }
            bool manualPos = queryBool(thisComposition, "manualPos");
            if(!manualPos){
                thisComposition->FirstChildElement("xPos")->SetText(std::to_string(posX).c_str());
                thisComposition->FirstChildElement("zPos")->SetText(std::to_string(posZ).c_str());
            }
            compositionExists = true;
        }
    }
    if(!compositionExists){
        std::cout << "A composition has the ID " << idG << "," << idB << " and the position " << posX << "," << posZ << " in the png, but does not exist in the xml." << std::endl;
        exit(-1);
    }
}

void Converter::deleteComposition(int idG, int idB){
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        int idGreen = queryInt(thisComposition, "idGreen");
        int idBlue = queryInt(thisComposition, "idBlue");
        if(idGreen == idG && idBlue == idB){
            doc->DeleteChild(thisComposition);
        }
    }
}

void Converter::save(){
    const char* charXmlFile = xmlFile.c_str();
    doc->SaveFile(charXmlFile);
}

float Converter::queryFloat(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    float ret;
    errorCheck(attributeElement->QueryFloatText(&ret));
    return ret;
}

float Converter::queryFloat(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    float ret;
    errorCheck(attributeElement->QueryFloatText(&ret));
    return ret;
}

int Converter::queryInt(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    int ret;
    errorCheck(attributeElement->QueryIntText(&ret));
    return ret;
}

int Converter::queryInt(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    int ret;
    errorCheck(attributeElement->QueryIntText(&ret));
    return ret;
}

bool Converter::queryBool(XMLElement* element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    bool ret;
    errorCheck(attributeElement->QueryBoolText(&ret));
    return ret;
}

bool Converter::queryBool(XMLDocument*& element, const char* attribute){
    XMLElement* attributeElement = element->FirstChildElement(attribute);
    if (attributeElement == NULL){
        std::cout << "XMLError: Attribute " << attribute << " does not exist." << std::endl;
        exit(-1);
    }
    bool ret;
    errorCheck(attributeElement->QueryBoolText(&ret));
    return ret;
}

void Converter::errorCheck(XMLError error){
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
