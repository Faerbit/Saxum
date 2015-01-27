#include "lodepng.hh"
#include <string>
#include "converter.hh"
#include <vector>
#include <iostream>

int main( int argc, char *argv[] ){
    if (argc <= 1){
        std::cout << "Converter needs the level (1,2,...) as input." << std::endl;
        exit(-1);
    }
    std::string levelString = argv[1];
    Converter conv = Converter(levelString);
    bool idFound[256][256];
    for (int i=0; i<256; i++){
        for (int j=0; j<256; j++){
            idFound[i][j] = false;
        }
    }
    
    //read the setup png    
    std::string filePath = "../Levels/ObjectSetups/Level" + levelString + ".png";
    std::vector<unsigned char> image; //the raw pixels
    unsigned int width, height;
    unsigned error = lodepng::decode(image, width, height, filePath);
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        exit(-1);
    }
    
    //iterate over all pixels of the image
    for(unsigned int rowNum = 0; rowNum < height; rowNum++){
        for(unsigned int columnNum = 0; columnNum < width; columnNum++){
            unsigned int pixel = (rowNum*width+columnNum)*4;
            //if there is a composition here, adjust the xml and image
            if(image[pixel]!=0 && image[pixel]!=255){
                if(image[pixel+1]==0 && image[pixel+2]==0){//composition has no ID
                    std::vector<int> newID;
                    newID = conv.newComposition(image[pixel], 0.5+rowNum-0.5*height, 0.5+columnNum-0.5*width);
                    idFound[newID[0]][newID[1]] = true;
                    image[pixel+1] = newID[0];
                    image[pixel+2] = newID[1];
                }else{//composition has an ID
                    conv.updateComposition(image[pixel+1], image[pixel+2], 0.5+rowNum-0.5*height, 0.5+columnNum-0.5*width);
                    idFound[image[pixel+1]][image[pixel+2]] = true;
                }
            }
        }
    }
    
    //write ids back to the setup png
    error = lodepng::encode(filePath, image, width, height);
    if(error) {
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
        exit(-1);
    }
    
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

