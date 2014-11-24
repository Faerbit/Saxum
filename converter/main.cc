#include "lodepng.hh"
#include <string>
#include "converter.hh"
#include <vector>
#include <iostream>

int main( int argc, char *argv[] ){  
    unsigned int level=atoi(argv[1]);
    Converter conv;
    bool idFound[256][256];
    for (int i=0; i<256; i++){
        for (int j=0; j<256; j++){
            idFound[i][j] = false;
        }
    }
    //read the setup png
    char levelChar[2];
    sprintf (levelChar,"%i", level);
    std::string levelString = levelChar;
    std::string filePath = "../Levels/ObjectSetups/Lvl" + levelString + ".png";
    std::vector<unsigned char> image; //the raw pixels
    unsigned int width, height;
    unsigned error = lodepng::decode(image, width, height, filePath);
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
    //iterate over all pixels of the image
    for(unsigned int rowNum = 0; rowNum < height; rowNum++){
        for(unsigned int columnNum = 0; columnNum < width; columnNum++){
            unsigned int pixel = (rowNum*width+columnNum)*4;
            //if there is a composition here, adjust the xml and image
            if(image[pixel]!=0){
                if(image[pixel+1]==0 && image[pixel+2]==0){
                    std::vector<unsigned int> temp;
                    temp = conv.newComposition(image[pixel], rowNum, columnNum);
                    idFound[temp[0]][temp[1]] = true;
                    image[pixel+1] = temp[0];
                    image[pixel+2] = temp[1];
                }else{
                    conv.updateComposition(image[pixel], image[pixel+1], image[pixel+2], rowNum, columnNum);
                    idFound[image[pixel+1]][image[pixel+2]] = true;
                }
            }
        }
    }
    //write ids back to the setup png
    error = lodepng::encode(filePath, image, width, height);
    if(error) {
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }
    //delete compositions that were not in the png anymore
    for (int i=0; i<256; i++){
        for (int j=0; j<256; j++){
            if (idFound[i][j] == false){
                //TODO if (exists)?
                conv.deleteComposition(i,j);
            }
        }
    }
}

