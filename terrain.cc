#include "terrain.hh"

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    std::ifstream terrain_png(this->filePath);
    unsigned int width, height, rowNum, columnNum;

    terrain_png.seekg(16);			//skip part of the header
    terrain_png.read((char *)&width, 4);	//read width
    terrain_png.read((char *)&height, 4);	//read height
    width = ntohl(width);			//convert from host to network byte order
    height = ntohl(height);			

    heightmap = new unsigned int*[height];	//initialize the heightmap
    for(rowNum=0; rowNum<height; rowNum++){	//read in the heightmap
	heightmap[rowNum] = new unsigned int[width];
	for(columnNum=0; columnNum<width; columnNum++){
	    terrain_png.read((char *)&heightmap[rowNum][columnNum], 1);
	}
    }



    
}

void Terrain::render() {
}
