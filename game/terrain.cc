#include "terrain.hh"
#include <lodepng/lodepng.h>

Terrain::Terrain(std::string heightmapFilePath){
    this->heightmapFilePath = heightmapFilePath;
}

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    std::vector<unsigned char> image; //the raw pixels
    unsigned int heightmapHeightLoading = 0;
    unsigned int heightmapWidthLoading = 0;
    unsigned error = lodepng::decode(image, heightmapWidthLoading, heightmapHeightLoading, heightmapFilePath);
    if (error) {
        std::cout << "Decoder error " << error << " from Terrain::load: " << lodepng_error_text(error) << std::endl;
    }
    this->heightmapHeight = heightmapHeightLoading;
    this->heightmapWidth = heightmapWidthLoading;
    this->heightmap = new float*[this->heightmapWidth]; //initialize the heightmap
    for(int columnNum = 0; columnNum < this->heightmapWidth; columnNum++){ //read in the heightmap
        this->heightmap[columnNum] = new float[this->heightmapHeight];
        for(int rowNum = 0; rowNum < this->heightmapHeight; rowNum++){
            this->heightmap[columnNum][rowNum] = (float)(image[(rowNum*heightmapWidth+columnNum)*4]) / 6; //<--heightmap is scaled here
        }
    }
}

SharedVertexArrayObject Terrain::makeTriangleMesh(int startX, int startZ, int endX, int endZ) {
    if (startX < 0) {
        startX = 0;
    }
    if (startZ < 0) {
        startZ = 0;
    }
    if (endX > heightmapWidth) {
        endX = heightmapWidth;
    }
    if (endZ > heightmapHeight) {
        endZ = heightmapHeight;
    }

    SharedArrayBuffer ab = SharedArrayBuffer(new ArrayBuffer());
    SharedVertexArrayObject vao = SharedVertexArrayObject(new VertexArrayObject());
    // Do NOT change the order of this!
    ab->defineAttribute("aPosition", GL_FLOAT, 3);
    ab->defineAttribute("aTexCoord", GL_FLOAT, 2);
    ab->defineAttribute("aNormal", GL_FLOAT, 3);
    
    int rowNum = startX, columnNum = startZ, dataCount=0, floatsPerVertex=8; //initializing:
    bool movingRight = true, isUp = true;
    int numVertices = ((endX - startX) - 1) * ((endZ - startZ) * 2 + 1) + 1;
    float* abData = new float[numVertices * floatsPerVertex];
    
    while(rowNum < endX){ //traversing the Triangle Strip!
        set_abData(abData, dataCount, rowNum, columnNum);
        dataCount += floatsPerVertex;
        if (isUp){
            rowNum = rowNum + 1;
            isUp = false;
        }
        else if (movingRight) {
            if (columnNum == endZ - 1) {
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
            if (columnNum == startZ){
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
    delete[] abData;
    vao->bind();
    vao->setMode(GL_TRIANGLE_STRIP);
    vao->attachAllAttributes(ab);
    return vao;
}



void Terrain::set_abData(float* abData, int dataCount, int rowNum, int columnNum){
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

float** Terrain::getHeightmap(){
    return this->heightmap;
}

int Terrain::getHeightmapHeight(){
    return this->heightmapHeight;
}

int Terrain::getHeightmapWidth(){
    return this->heightmapWidth;
}
