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
        void updateComposition(unsigned int idG, unsigned int idB, unsigned int posX, unsigned int posZ);
        std::vector<unsigned int> newComposition(unsigned int type, unsigned int posX, unsigned int posZ);
	    void deleteComposition(unsigned int idG, unsigned int idB);
	    void save();
	    std::vector<unsigned int> getNextId();

    private:
        std::vector<unsigned int> nextId;
        std::string xmlFile;
        XMLDocument* doc = new XMLDocument();
        XMLDocument* compositions = new XMLDocument();
        void newObject(unsigned int type, unsigned int idG, unsigned int idB, unsigned int posX, unsigned int posZ);
};

#endif
