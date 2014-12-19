#include "converter.hh"
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <iostream>

using namespace tinyxml2;

Converter::Converter(std::string level){
    xmlFile = "../Levels/ObjectSetups/Level" + level + ".xml";
    
    //Load Compositions
    const char* charCompositions = "../Levels/ObjectSetups/Compositions.xml";
    compositions->LoadFile(charCompositions);
    if (compositions->ErrorID()!=0){
        printf("Could not open Compositions!\n");
    }
    
    //Create a backup of the current Level png file, if no backup exists
    std::string pngFile = "../Levels/ObjectSetups/Level" + level + ".png";
    std::string backupPNG = "../Levels/ObjectSetups/BackupLevel" + level + ".png";
    struct stat buf;
    if(stat(backupPNG.c_str(), &buf) != 0){
        std::ifstream  src(pngFile, std::ios::binary);
        std::ofstream  dst(backupPNG,  std::ios::binary);
        dst << src.rdbuf();
    }
    
    //Create a backup of the current Level xml file
    std::string backupXML = "../Levels/ObjectSetups/BackupLevel" + level + ".xml";
    std::ifstream  src(xmlFile, std::ios::binary);
    std::ofstream  dst(backupXML,  std::ios::binary);
    
    
    //Load the Level xml file
    nextID.push_back(1);
    nextID.push_back(1);
    const char* charXmlFile = xmlFile.c_str();
    doc->LoadFile(charXmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open xml, creating new xml.\n");
        //Create all global Elements with Dummy-Values
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
        XMLElement* rColourFog = doc->NewElement("rColour");
        XMLElement* gColourFog = doc->NewElement("gColour");
        XMLElement* bColourFog = doc->NewElement("bColour");
        XMLElement* alphaFog = doc->NewElement("alpha");
        
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
        rColourFog->SetText("0.10");
        gColourFog->SetText("0.14");
        bColourFog->SetText("0.14");
        alphaFog->SetText("1.0");
        
        XMLElement* ambientLight = doc->NewElement("ambientLight");
        XMLElement* fogColour = doc->NewElement("fogColour");
        XMLElement* directionalLight = doc->NewElement("directionalLight");
        
        ambientLight->InsertEndChild(rColourAmbient);
        ambientLight->InsertEndChild(gColourAmbient);
        ambientLight->InsertEndChild(bColourAmbient);
        fogColour->InsertEndChild(rColourFog);
        fogColour->InsertEndChild(gColourFog);
        fogColour->InsertEndChild(bColourFog);
        fogColour->InsertEndChild(alphaFog);
        for(int i=0;i<7;i++){
            directionalLight->InsertEndChild(lightAttributes[i]);
        }
        
        XMLElement* skydome = doc->NewElement("skydome");
        XMLElement* skydomeTexture = doc->NewElement("texture");
        skydomeTexture->SetText("skydome.png");
        skydome->InsertEndChild(skydomeTexture);
        
        doc->InsertEndChild(ambientLight);
        doc->InsertEndChild(fogColour);
        doc->InsertEndChild(directionalLight);
        doc->InsertEndChild(skydome);
    }else{
        dst << src.rdbuf();
        XMLElement* thisComposition = doc->FirstChildElement("composition");
        int idGreen, idBlue;
        for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
            errorCheck(thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
            errorCheck(thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
            if(idGreen > nextID[0] || (idGreen == nextID[0] && idBlue > nextID[1])){
                nextID[0] = idGreen;
                nextID[1] = idBlue;
            }
        }
    }
}

Converter::Converter(){
}

Converter::~Converter(){
}

std::vector<int> Converter::newComposition(int type, float posX, float posZ){
    XMLElement* newComposition = doc->NewElement("composition");
    doc->InsertFirstChild(newComposition);
    
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
    XMLElement* xPosition = doc->NewElement("xPosition");
    XMLElement* yPosition = doc->NewElement("yPosition");
    XMLElement* zPosition = doc->NewElement("zPosition");
    XMLElement* targetIdGreen = doc->NewElement("targetIdGreen");
    XMLElement* targetIdBlue = doc->NewElement("targetIdBlue");
    XMLElement* distance = doc->NewElement("distance");
    XMLElement* isBiggerThan = doc->NewElement("isBiggerThan");
    XMLElement* objectNum = doc->NewElement("objectNum");
    XMLElement* functionPointer = doc->NewElement("functionPointer");
    
    name->SetText("-");
    xPosition->SetText("0");
    yPosition->SetText("0");
    zPosition->SetText("0");
    targetIdGreen->SetText("-");
    targetIdBlue->SetText("-");
    distance->SetText("1.0");
    isBiggerThan->SetText("false");
    objectNum->SetText("0");
    functionPointer->SetText("-");
    
    trigger->InsertEndChild(name);
    trigger->InsertEndChild(xPosition);
    trigger->InsertEndChild(yPosition);
    trigger->InsertEndChild(zPosition);
    trigger->InsertEndChild(targetIdGreen);
    trigger->InsertEndChild(targetIdBlue);
    trigger->InsertEndChild(distance);
    trigger->InsertEndChild(isBiggerThan);
    trigger->InsertEndChild(objectNum);
    trigger->InsertEndChild(functionPointer);
    
    std::vector<int> ret = nextID;
    nextID[1] += 1;
    if (nextID[1] == 255){
        nextID[1] = 0;
        nextID[0] +=1;
    }
    return ret;
}

void Converter::updateComposition(int idG, int idB, float posX, float posZ){
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    int idGreen = 0, idBlue = 0;
    bool compositionExists = false;
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        errorCheck(thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
        errorCheck(thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
        if(idGreen == idG && idBlue == idB){
            bool manualPos;
            errorCheck(thisComposition->FirstChildElement("manualPos")->QueryBoolText(&manualPos));
            if(!manualPos){
                thisComposition->FirstChildElement("xPos")->SetText(std::to_string(posX).c_str());
                thisComposition->FirstChildElement("zPos")->SetText(std::to_string(posZ).c_str());
            }
            compositionExists = true;
        }
    }
    if(!compositionExists){
        std::cout << "A composition has an ID in the png, but does'nt exist in the xml." << std::endl;
        exit(-1);
    }
}

void Converter::deleteComposition(int idG, int idB){
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    int idGreen, idBlue;
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        errorCheck(thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen));
        errorCheck(thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue));
        if(idGreen == idG && idBlue == idB){
            doc->DeleteChild(thisComposition);
        }
    }
}

void Converter::save(){
    const char* charXmlFile = xmlFile.c_str();
    doc->SaveFile(charXmlFile);
}

std::vector<int> Converter::getNextID(){
    return nextID;
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
    }
}
