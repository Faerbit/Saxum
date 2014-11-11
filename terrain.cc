#include "terrain.hh"

Terrain::Terrain(std::string filePath){
    this->filePath = filePath;
}

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    this->filePath = "../Levels/LevelTest/terrain";	//TODO remove this, its only for testing

    std::ifstream terrain_png(this->filePath + "/heightmap.png");
    unsigned int rowNum, columnNum, heightmapValue;

    terrain_png.seekg(16);						//skip part of the header

    char temp[4];
    terrain_png.read(temp, 4);									//read width
    this->heightmapWidth = (temp[3]<<0) | (temp[2]<<8) | (temp[1]<<16) | (temp[0]<<24);		//convert from network to host byte order
    terrain_png.read(temp, 4);									//read height
    this->heightmapHeight = (temp[3]<<0) | (temp[2]<<8) | (temp[1]<<16) | (temp[0]<<24);	//convert from network to host byte order

    heightmap = new float*[this->heightmapHeight];				//initialize the heightmap
    for(rowNum = 0; rowNum < this->heightmapHeight; rowNum++){			//read in the heightmap
	    heightmap[rowNum] = new float[this->heightmapWidth];
        for(columnNum = 0; columnNum < this->heightmapWidth; columnNum++){
            terrain_png.read((char *)&heightmapValue, 1);
	    heightmap[rowNum][columnNum] = (float)heightmapValue / 256;
        }
    }

    this->makeTriangleMesh();
    heightmapChanged = false;		//no need to make a TriangleMesh again before rendering

}

void Terrain::makeTriangleMesh(){

    ACGL::OpenGL::SharedArrayBuffer ab = std::make_shared<ACGL::OpenGL::ArrayBuffer>();
    ab->defineAttribute("pos", GL_FLOAT, 3);						//TODO: ArrayBuffer for the texture coordinates

    unsigned int rowNum=0, columnNum=0, dataCount=0;					//initializing:
    bool movingRight = true, isUp = true;
    int numVertices = (this->heightmapHeight - 1) * (this->heightmapWidth * 2 + 1) + 1;
    float* abData = new float[numVertices * 3];

    while(rowNum < this->heightmapHeight){						//traversing the Triangle Strip!
	abData[dataCount] = (float)rowNum;
	abData[dataCount+1] = heightmap[rowNum][columnNum];
	abData[dataCount+2] = (float)columnNum;
	dataCount += 3;
	if (isUp){
	    rowNum = rowNum + 1;
	    isUp = false;
	}else if (movingRight){
	    if (columnNum == this->heightmapWidth - 1){
		abData[dataCount] = (float)rowNum;
		abData[dataCount+1] = heightmap[rowNum][columnNum];
		abData[dataCount+2] = (float)columnNum;
		dataCount += 3;
		abData[dataCount] = (float)rowNum;
		abData[dataCount+1] = heightmap[rowNum][columnNum];
		abData[dataCount+2] = (float)columnNum;
		dataCount += 3;
		movingRight = false;
		rowNum = rowNum + 1;
	    } else{
		rowNum = rowNum - 1;
		columnNum = columnNum + 1;
		isUp = true;
	    }
	}else{
	    if (columnNum == 0){
		abData[dataCount] = (float)rowNum;
		abData[dataCount+1] = heightmap[rowNum][columnNum];
		abData[dataCount+2] = (float)columnNum;
		dataCount += 3;
		abData[dataCount] = (float)rowNum;
		abData[dataCount+1] = heightmap[rowNum][columnNum];
		abData[dataCount+2] = (float)columnNum;
		dataCount += 3;
		movingRight = true;
		rowNum = rowNum + 1;
	    }else{
		rowNum = rowNum - 1;
		columnNum = columnNum - 1;
		isUp = true;
	    }
	}
    }

    ab->setDataElements(numVertices, abData);
    this->triangleMesh = std::make_shared<ACGL::OpenGL::VertexArrayObject>();
    this->triangleMesh->bind();
    this->triangleMesh->setMode(GL_TRIANGLE_STRIP);
    this->triangleMesh->attachAllAttributes(ab);
    //TODO unbind?



    //TODO remove this TestCode (that doesnt even work yet...):
/*    ACGL::OpenGL::SharedArrayBuffer tex = std::make_shared<ACGL::OpenGL::ArrayBuffer>();
    tex->defineAttribute("color", GL_FLOAT, 3);
    float* texData = new float[numVertices*3];
    for (int i=0; i<numVertices*3; i++){
	texData[i] = 1.0;
    }
    tex->setDataElements(numVertices, texData);
    this->triangleMesh->attachAllAttributes(tex);
*/


}

void Terrain::render() {
    if (heightmapChanged)
	this->makeTriangleMesh();
    this->triangleMesh->render();
}













