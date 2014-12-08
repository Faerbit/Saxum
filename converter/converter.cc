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
        printf("Could not open Compositions!!!\n");
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
    dst << src.rdbuf();
    
    //Load the Level xml file
    const char* charXmlFile = xmlFile.c_str();
    doc->LoadFile(charXmlFile);
    nextID.push_back(1);
    nextID.push_back(1);
    if (doc->ErrorID()!=0){
        printf("Could not open xml, creating new xml.\n");
    }else{
        XMLElement* thisComposition = doc->FirstChildElement("composition");
        int idGreen, idBlue;
        XMLError error=XML_NO_ERROR;
        for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
            error = thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen);
            errorCheck(error);
            error = thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue);
            errorCheck(error);
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

std::vector<unsigned int> Converter::newComposition(unsigned int type, float posX, float posZ){
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
    
    std::vector<unsigned int> ret = nextID;
    nextID[1] += 1;
    if (nextID[1] == 255){
        nextID[1] = 0;
        nextID[0] +=1;
    }
    return ret;
}

void Converter::updateComposition(unsigned int idG, unsigned int idB, float posX, float posZ){
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    int idGreen = 0, idBlue = 0;
    bool compositionExists = false;
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        XMLError error=XML_NO_ERROR;
        error = thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen);
        errorCheck(error);
        error = thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue);
        errorCheck(error);
        if(idGreen == idG && idBlue == idB){
            bool manualPos;
            error=thisComposition->FirstChildElement("manualPos")->QueryBoolText(&manualPos);
            errorCheck(error);
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

void Converter::deleteComposition(unsigned int idG, unsigned int idB){
    XMLElement* thisComposition = doc->FirstChildElement("composition");
    int idGreen, idBlue;
    XMLError error=XML_NO_ERROR;
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement("composition")){
        error = thisComposition->FirstChildElement("idGreen")->QueryIntText(&idGreen);
        errorCheck(error);
        error = thisComposition->FirstChildElement("idBlue")->QueryIntText(&idBlue);
        errorCheck(error);
        if(idGreen == idG && idBlue == idB){
            doc->DeleteChild(thisComposition);
        }
    }
}

void Converter::save(){
    const char* charXmlFile = xmlFile.c_str();
    doc->SaveFile(charXmlFile);
}

std::vector<unsigned int> Converter::getNextID(){
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
