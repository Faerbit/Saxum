#include "converter.hh"
#include <fstream>

using namespace tinyxml2;

Converter::Converter(std::string level){
    xmlFile = "../Levels/ObjectSetups/Level" + level + ".xml";
    
    //Create a backup of the current xml file
    std::string backup = "../Levels/ObjectSetups/BackupLevel" + level + ".xml";
    std::ifstream  src(xmlFile, std::ios::binary);
    std::ofstream  dst(backup,  std::ios::binary);
    dst << src.rdbuf();
    
    //Load the xml file
    const char* charXmlFile = xmlFile.c_str();
    doc->LoadFile(charXmlFile);
    if (doc->ErrorID()!=0){
        printf("Could not open xml, creating new xml.");
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
    //TODO
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
