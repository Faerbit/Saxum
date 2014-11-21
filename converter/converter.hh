#ifndef CONVERTER_INCLUDED
#define CONVERTER_INCLUDED
#include <vector>

class Converter {
    public:
        Converter();
        ~Converter();
        void updateComposition(unsigned int type, unsigned int idG, unsigned int idB, unsigned int posX, unsigned int posZ);
        std::vector<unsigned int> newComposition(unsigned int type, unsigned int posX, unsigned int posZ);
	    void deleteComposition(unsigned int idG, unsigned int idB);

    private:
        void newObject(unsigned int type, unsigned int idG, unsigned int idB, unsigned int posX, unsigned int posZ);
};
#endif
