#include <lodepng/lodepng.h>
#include <string>
#include "converter.hh"
#include <vector>
#include <iostream>
#include "tinyxml2.hh"

using namespace tinyxml2;

int main( int argc, char *argv[] ){
    if (argc <= 1){
        std::cout << "Converter needs the levels name (Level1) as input." << std::endl;
        exit(-1);
    }
    printf("Initializing.\n");
    std::string levelString = argv[1];
    
    
    bool idFound[256][256];
    for (int i=0; i<256; i++){
        for (int j=0; j<256; j++){
            idFound[i][j] = false;
        }
    }
    
    XMLDocument* config = new XMLDocument();
    const char* xmlFile = "../data/config.xml";
    config->LoadFile(xmlFile);
    if (config->ErrorID()!=0){
        printf("Could not open config.xml!\n");
        exit(-1);
    }
    
    XMLElement* xmlLevelPath = config->FirstChildElement("levelXmlPath");
    if (xmlLevelPath == NULL){
        std::cout << "XMLError: Attribute levelXmlPath does not exist." << std::endl;
        exit(-1);
    }
    const char* charLevelPath = xmlLevelPath->GetText();
    if(charLevelPath == NULL){
        std::cout << "XMLError: Attribute levelXmlPath could not be loaded." << std::endl;
        exit(-1);
    }
    std::string levelPath = charLevelPath;
    
    XMLElement* xmlCompositionsPath = config->FirstChildElement("compositionsPath");
    if (xmlCompositionsPath == NULL){
        std::cout << "XMLError: Attribute compositionsPath does not exist." << std::endl;
        exit(-1);
    }
    const char* charCompositionsPath = xmlCompositionsPath->GetText();
    if(charCompositionsPath == NULL){
        std::cout << "XMLError: Attribute compositionsPath could not be loaded." << std::endl;
        exit(-1);
    }
    std::string compositionsPath = charCompositionsPath;
    Converter conv = Converter("../" + levelPath, levelString, "../" + compositionsPath);
    
    //read the setup png
    printf("Loading the png.\n");
    std::string filePath = "../" + levelPath + levelString + ".png";
    std::vector<unsigned char> image; //the raw pixels
    unsigned int width, height;
    unsigned error = lodepng::decode(image, width, height, filePath);
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        std::cout << "Converter needs the levels name (Level1) as input." << std::endl;
        exit(-1);
    }
    printf("Iterating over the png.\n");
    //iterate over all pixels of the image
    for(unsigned int columnNum = 0; columnNum < width; columnNum++){
        for(unsigned int rowNum = 0; rowNum < height; rowNum++){
            unsigned int pixel = (rowNum*width+columnNum)*4;
            //if there is a composition here, adjust the xml and image
            if(image[pixel]!=0 && image[pixel]!=255){
                if(image[pixel+1]==0 && image[pixel+2]==0){//composition has no ID
                    std::vector<int> newID;
                    newID = conv.newComposition(image[pixel], 0.5+columnNum-0.5*width, 0.5+rowNum-0.5*height);
                    idFound[newID[0]][newID[1]] = true;
                    image[pixel+1] = newID[0];
                    image[pixel+2] = newID[1];
                }else{//composition has an ID
                    conv.updateComposition(image[pixel+1], image[pixel+2], 0.5+columnNum-0.5*width, 0.5+rowNum-0.5*height);
                    idFound[image[pixel+1]][image[pixel+2]] = true;
                }
            }
        }
    }
    printf("Writing IDs back to the png.\n");
    //write ids back to the setup png
    error = lodepng::encode(filePath, image, width, height);
    if(error) {
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
        exit(-1);
    }
    printf("Deleting obsolete Compositions.\n");
    //delete compositions that were not in the png anymore
    for (int i=0; i<256; i++){
        for (int j=0; j<256; j++){
            if (! idFound[i][j]){
                conv.deleteComposition(i,j);
            }
        }
    }
    
    //save the xml
    conv.save();
}
