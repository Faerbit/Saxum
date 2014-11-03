#include "terrain.hh"

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    std::ifstream terrain_png(this->filePath);
    unsigned int width, height, rowNum, columnNum;

    terrain_png.seekg(16);
    terrain_png.read((char *)&width, 4);
    terrain_png.read((char *)&height, 4);
    width = ntohl(width);
    height = ntohl(height);

    heightmap = new unsigned int*[height];
    for(rowNum=0; rowNum<height; rowNum++){
	heightmap[columnNum] = new unsigned int[width];
	for(columnNum=0; columnNum<width; columnNum++){
	    terrain_png.read((char *)&heightmap[columnNum][rowNum], 1);
	}
    }
    


    
}

void Terrain::render() {
}
