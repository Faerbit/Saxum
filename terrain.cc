#include "terrain.hh"

//Terrain::Terrain(std::string filePath){
//    this->filePath = filePath;
//}

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    std::ifstream terrain_png(this->filePath + "/heightmap.png");			//TODO: filepath organization
    unsigned int rowNum, columnNum, heightmapValue;

    terrain_png.seekg(16);					//skip part of the header
    terrain_png.read((char *)&this->heightmapWidth, 4);		//read width
    terrain_png.read((char *)&this->heightmapHeight, 4);	//read height
    this->heightmapWidth  = ntohl(this->heightmapWidth);	//convert from host to network byte order
    this->heightmapHeight = ntohl(this->heightmapHeight);			

    heightmap = new float*[this->heightmapHeight];		//initialize the heightmap
    for(rowNum = 0; rowNum < this->heightmapHeight; rowNum++){	//read in the heightmap
	    heightmap[rowNum] = new float[this->heightmapWidth];
        for(columnNum = 0; columnNum < this->heightmapWidth; columnNum++){
            terrain_png.read((char *)&heightmapValue, 1);
	    heightmap[rowNum][columnNum] = (float)heightmapValue / 5;
        }
    }

    this->makeTriangleMesh();
    heightmapChanged = false;		//no need to make a TriangleMesh again before rendering

}

void Terrain::makeTriangleMesh(){

    ACGL::OpenGL::SharedArrayBuffer arrayBuffer = ACGL::OpenGL::SharedArrayBuffer();
    arrayBuffer->defineAttribute("pos", GL_FLOAT, 3);	//TODO: ArrayBuffer for the texture coordinates

    unsigned int rowNum=0, columnNum=0;				//initializing:
    bool movingRight = true, isUp = true;
    while(rowNum < this->heightmapHeight){			//traversing the Triangle Strip!
	float newPos[3];
	newPos[0] = (float)rowNum;
	newPos[1] = (float)columnNum;
	newPos[2] = heightmap[rowNum][columnNum];
        arrayBuffer->setDataElements(1, &newPos);
	if (isUp){
	    rowNum = rowNum + 1;
	    isUp = false;
	}else if (movingRight){
	    if (columnNum == this->heightmapWidth - 1){
		arrayBuffer->setDataElements(1, &newPos);
		arrayBuffer->setDataElements(1, &newPos);
		movingRight = false;
		rowNum = rowNum + 1;
	    } else{
		rowNum = rowNum - 1;
		columnNum = columnNum + 1;
		isUp = true;
	    }
	}else{
	    if (columnNum == 0){
		arrayBuffer->setDataElements(1, &newPos);
		arrayBuffer->setDataElements(1, &newPos);
		movingRight = true;
		rowNum = rowNum + 1;
	    }else{
		rowNum = rowNum - 1;
		columnNum = columnNum - 1;
		isUp = true;
	    }
	}
    }

    this->triangleMesh = ACGL::OpenGL::SharedVertexArrayObject();
    this->triangleMesh->bind();
    this->triangleMesh->setMode(GL_TRIANGLE_STRIP);
    this->triangleMesh->attachAllAttributes(arrayBuffer);
    //TODO unbind?
}

void Terrain::render() {
    if (heightmapChanged)
	this->makeTriangleMesh();
    this->triangleMesh->render();
}













