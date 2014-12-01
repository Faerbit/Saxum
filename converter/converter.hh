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
        void updateComposition(unsigned int idG, unsigned int idB, float posX, float posZ);
        std::vector<unsigned int> newComposition(unsigned int type, float posX, float posZ);
	    void deleteComposition(unsigned int idG, unsigned int idB);
	    void save();
	    std::vector<unsigned int> getNextID();

    private:
        std::vector<unsigned int> nextID;
        std::string xmlFile;
        XMLDocument* doc = new XMLDocument();
        XMLDocument* compositions = new XMLDocument();
};

#endif
