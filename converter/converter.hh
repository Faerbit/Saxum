#ifndef CONVERTER_INCLUDED
#define CONVERTER_INCLUDED

#include <vector>
#include <string>
#include "tinyxml2.hh"

using namespace tinyxml2;
class Converter {
    public:
        Converter(std::string level);
        Converter();
        ~Converter();
        void updateComposition(unsigned int idG, unsigned int idB, float posX, float posZ); //updates the position of a composition
        std::vector<unsigned int> newComposition(unsigned int type, float posX, float posZ);//creates a new composition and returns its ID
	    void deleteComposition(unsigned int idG, unsigned int idB);
	    void save();                            //writes the xml to file
	    std::vector<unsigned int> getNextID();  //returns the next unused ID

    private:
        std::vector<unsigned int> nextID;
        std::string xmlFile;
        XMLDocument* doc = new XMLDocument();
        XMLDocument* compositions = new XMLDocument();
};

#endif
