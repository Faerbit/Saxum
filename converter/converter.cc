#include "converter.hh"
#include <fstream>

using namespace tinyxml2;

Converter::Converter(std::string level){
    xmlFile = "../Levels/ObjectSetups/Level" + level + ".xml";
    
    //Load Compositions
    const char* charCompositions = "../Levels/ObjectSetups/Compositions.xml";
    compositions->LoadFile(charCompositions);
    if (compositions->ErrorID()!=0){
        printf("Could not open Compositions!!!\n");
    }
    
    //Create a backup of the current Level xml file
    std::string backup = "../Levels/ObjectSetups/BackupLevel" + level + ".xml";
    std::ifstream  src(xmlFile, std::ios::binary);
    std::ofstream  dst(backup,  std::ios::binary);
    dst << src.rdbuf();
    
    //Load the Level xml file
    const char* charXmlFile = xmlFile.c_str();
    doc->LoadFile(charXmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open xml, creating new xml.\n");
    }
    
    nextId.push_back(1);
    nextId.push_back(250);  //TODO
}

Converter::Converter(){
}

Converter::~Converter(){
}

void Converter::updateComposition(unsigned int idG, unsigned int idB, unsigned int posX, unsigned int posZ){
    //TODO
}


std::vector<unsigned int> Converter::newComposition(unsigned int type, unsigned int posX, unsigned int posZ){
    XMLNode* thisComposition = compositions->FirstChild();
    for(; thisComposition; thisComposition=thisComposition->NextSiblingElement()){
        int thisType;
        XMLElement* thisCompositionElement = thisComposition->ToElement();
        thisCompositionElement->QueryIntAttribute("typeID", &thisType);
        if(thisType == type){
            //XMLElement* newComposition = doc.NewElement();
            //newComposition = thisComposition;
            //XMLElement* newComposition = thisComposition->Clone();        //TODO write own clone function
            doc->InsertFirstChild(newComposition);
            
            XMLElement* idBlue  = doc->NewElement("<idBlue>"  + itoa(nextId[1]) + "</idBlue>");
            XMLElement* idGreen = doc->NewElement("<idGreen>" + itoa(nextId[0]) + "</idGreen>");
            XMLElement* zPos    = doc->NewElement("<zPos>"    + itoa(posZ)      + "</zPos>");
            XMLElement* yOffset = doc->NewElement("<yOffset>0.0</yOffset>");
            XMLElement* xPos    = doc->NewElement("<xPos>"    + itoa(posX)      + "</xPos>");
            XMLElement* zRot    = doc->NewElement("<zRot>0.0</zRot>");
            XMLElement* yRot    = doc->NewElement("<yRot>0.0</yRot>");
            XMLElement* xRot    = doc->NewElement("<xRot>0.0</xRot>");
            XMLElement* scale   = doc->NewElement("<scale>1.0</scale>");
            
            newComposition->InsertFirstChild(idBlue);
            newComposition->InsertFirstChild(idGreen);
            newComposition->InsertFirstChild(zPos);
            newComposition->InsertFirstChild(yOffset);
            newComposition->InsertFirstChild(xPos);
            newComposition->InsertFirstChild(zRot);
            newComposition->InsertFirstChild(yRot);
            newComposition->InsertFirstChild(xRot);
            newComposition->InsertFirstChild(scale);
        }
    }
    
    std::vector<unsigned int> ret = nextId;
    nextId[1] += 1;
    if (nextId[1] == 256){
        nextId[1] = 1;
        nextId[0] +=1;
    }
    return ret;
}

void Converter::deleteComposition(unsigned int idG, unsigned int idB){
    //TODO
}

void Converter::save(){
    const char* charXmlFile = xmlFile.c_str();
    doc->SaveFile(charXmlFile);
}

std::vector<unsigned int> Converter::getNextId(){
    return nextId;
}


void Converter::newObject(unsigned int type, unsigned int idG, unsigned int idB, unsigned int posX, unsigned int posZ){
    //TODO
}
