#ifndef CONVERTER_INCLUDED
#define CONVERTER_INCLUDED

#include <vector>
#include <string>
#include "tinyxml2.hh"

using namespace tinyxml2;
class Converter {
    public:
        Converter(std::string levelPath, std::string compositionsPath);
        Converter();
        ~Converter();
        void updateComposition(int idG, int idB, float posX, float posZ); //updates the position of a composition
        std::vector<int> newComposition(int type, float posX, float posZ); //creates a new composition and returns its ID
        void deleteComposition(int idG, int idB);
        void save(); //writes the xml to file
    private:
        
        std::vector<int> nextID;
        bool idUsed[256][256];
        std::string xmlFile;
        XMLDocument* doc = new XMLDocument();
        XMLDocument* compositions = new XMLDocument();
        int queryInt(XMLElement* element, const char* attribute);
        int queryInt(XMLDocument*& element, const char* attribute);
        bool queryBool(XMLElement* element, const char* attribute);
        bool queryBool(XMLDocument*& element, const char* attribute);
        void errorCheck(XMLError error);
};

#endif
