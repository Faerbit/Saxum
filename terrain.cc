#include "terrain.hh"
#include "lodepng.h"

Terrain::Terrain(std::string levelNum){
    this->levelNum = levelNum;
}

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    std::vector<unsigned char> image; //the raw pixels
    unsigned error = lodepng::decode(image, heightmapWidth, heightmapHeight, "../Levels/heightmapLvl" + levelNum + ".png");
    if (error) {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
    }
    this->heightmap = new float*[this->heightmapHeight];					//initialize the heightmap
    for(unsigned int rowNum = 0; rowNum < this->heightmapHeight; rowNum++){				//read in the heightmap
	    this->heightmap[rowNum] = new float[this->heightmapWidth];
        for(unsigned int columnNum = 0; columnNum < this->heightmapWidth; columnNum++){
	        this->heightmap[rowNum][columnNum] = (float)(image[(rowNum*heightmapWidth+columnNum)*4]) / 16;
        }
    }
    
    this->makeTriangleMesh();
    heightmapChanged = false;		//no need to make a TriangleMesh again before rendering

}

void Terrain::makeTriangleMesh(){

    ACGL::OpenGL::SharedArrayBuffer ab = std::make_shared<ACGL::OpenGL::ArrayBuffer>();
    // Do NOT change the order of this!
    ab->defineAttribute("aPosition", GL_FLOAT, 3);
    ab->defineAttribute("aTexCoord", GL_FLOAT, 2);
    ab->defineAttribute("aNormal", GL_FLOAT, 3);

    unsigned int rowNum=0, columnNum=0, dataCount=0, floatsPerVertex=8;				//initializing:
    bool movingRight = true, isUp = true;
    int numVertices = (this->heightmapHeight - 1) * (this->heightmapWidth * 2 + 1) + 1;
    float* abData = new float[numVertices * floatsPerVertex];

    while(rowNum < this->heightmapHeight){							//traversing the Triangle Strip!
	set_abData(abData, dataCount, rowNum, columnNum);
	dataCount += floatsPerVertex;
	if (isUp){
	    rowNum = rowNum + 1;
	    isUp = false;
	}
        else if (movingRight) {
            if (columnNum == this->heightmapWidth - 1) {
                set_abData(abData, dataCount, rowNum, columnNum);
                dataCount += floatsPerVertex;
                set_abData(abData, dataCount, rowNum, columnNum);
                dataCount += floatsPerVertex;
                movingRight = false;
                rowNum = rowNum + 1;
            } 
            else {
                rowNum = rowNum - 1;
                columnNum = columnNum + 1;
                isUp = true;
            }
        }
        else {
            if (columnNum == 0){
                set_abData(abData, dataCount, rowNum, columnNum);
                dataCount += floatsPerVertex;
                set_abData(abData, dataCount, rowNum, columnNum);
                dataCount += floatsPerVertex;
                movingRight = true;
                rowNum = rowNum + 1;
            }
            else {
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
}



void Terrain::set_abData(float* abData, unsigned int dataCount, unsigned int rowNum, unsigned int columnNum){
    //set Position
    abData[dataCount] = (float)rowNum;
    abData[dataCount+1] = heightmap[rowNum][columnNum];
    abData[dataCount+2] = (float)columnNum;

    //set Texture Coordinate
    abData[dataCount+3] = (float)(rowNum % 2);
    abData[dataCount+4] = (float)(columnNum % 2);

    //setNormal
    if (rowNum==0 || rowNum==(this->heightmapHeight-1) || columnNum==0 || columnNum==(this->heightmapWidth-1)){
    	abData[dataCount+5] = 0.0;
    	abData[dataCount+6] = 1.0;
    	abData[dataCount+7] = 0.0;
    }
    else {
        glm::vec3 sumNormals = glm::vec3(0.0f, 0.0f, 0.0f);
        for (int i=-1; i<2; i+=2) {
            for (int j=-1; j<2; j+=2) {
                glm::vec3 vecA, vecB, vecC, normal;
                vecA = glm::normalize(glm::vec3((float)i, (heightmap[rowNum+i][columnNum]   - heightmap[rowNum][columnNum]), 0.0f));
                vecB = glm::normalize(glm::vec3((float)i, (heightmap[rowNum+i][columnNum+j] - heightmap[rowNum][columnNum]), (float)j));
                normal = glm::normalize(glm::cross(vecA, vecB));
                if(i+j!=0){
                    normal = normal*(-1.0f);
                }
                sumNormals += normal;
                
                vecC = glm::normalize(glm::vec3(0.0f,     (heightmap[rowNum][columnNum+j]   - heightmap[rowNum][columnNum]), (float)j));
                normal = glm::normalize(glm::cross(vecB, vecC));
                if(i+j!=0){
                    normal = normal*(-1.0f);
                }
                sumNormals += normal;
            }
        }
        sumNormals = glm::normalize(sumNormals);
        abData[dataCount+5] = sumNormals[0];
        abData[dataCount+6] = sumNormals[1];
        abData[dataCount+7] = sumNormals[2];
    }
}

Model Terrain::getModel(){
    return Model(this->triangleMesh);
}

float** Terrain::getHeightmap(){
    return this->heightmap;
}

unsigned int Terrain::getHeightmapHeight(){
    return this->heightmapHeight;
}

unsigned int Terrain::getHeightmapWidth(){
    return this->heightmapWidth;
}
