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

    std::ifstream terrain_png(this->filePath + "/heightmap2.png");
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
    ab->defineAttributeWithOffset("aPosition", GL_FLOAT, 3, 0);
    ab->defineAttributeWithOffset("aNormal", GL_FLOAT, 3, 3);
    ab->defineAttributeWithOffset("aTexCoord", GL_FLOAT, 2, 6);

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
	}else if (movingRight){
	    if (columnNum == this->heightmapWidth - 1){
		set_abData(abData, dataCount, rowNum, columnNum);
		dataCount += floatsPerVertex;
		set_abData(abData, dataCount, rowNum, columnNum);
		dataCount += floatsPerVertex;
		movingRight = false;
		rowNum = rowNum + 1;
	    } else{
		rowNum = rowNum - 1;
		columnNum = columnNum + 1;
		isUp = true;
	    }
	}else{
	    if (columnNum == 0){
		set_abData(abData, dataCount, rowNum, columnNum);
		dataCount += floatsPerVertex;
		set_abData(abData, dataCount, rowNum, columnNum);
		dataCount += floatsPerVertex;
		movingRight = true;
		rowNum = rowNum + 1;
	    }else{
		rowNum = rowNum - 1;
		columnNum = columnNum - 1;
		isUp = true;
	    }
	}
    }

    //ab->setDataElements(numVertices, abData);
    float* testData = new float[32];
    testData[0]=0.0f;testData[1]=0.0f;testData[2]=0.0f;
    testData[3]=0.0f;testData[4]=1.0f;testData[5]=0.0f;
    testData[6]=0.0f;testData[7]=0.0f;

    testData[8]=1.0f;testData[9]=0.2f;testData[10]=0.0f;
    testData[11]=0.0f;testData[12]=1.0f;testData[13]=0.0f;
    testData[14]=1024.0f;testData[15]=.0f;

    testData[16]=0.0f;testData[17]=0.2f;testData[18]=1.0f;
    testData[19]=0.0f;testData[20]=1.0f;testData[21]=0.0f;
    testData[22]=0.0f;testData[23]=1024.0f;

    testData[24]=1.0f;testData[25]=0.0f;testData[26]=1.0f;
    testData[27]=0.0f;testData[28]=1.0f;testData[29]=0.0f;
    testData[30]=1024.0f;testData[31]=1024.0f;
    ab->setDataElements(numVertices, testData);
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

    //setNormal
    if (rowNum==0 || rowNum==(this->heightmapHeight-1) || columnNum==0 || columnNum==(this->heightmapWidth-1)){
    	abData[dataCount+3] = 0.0;
    	abData[dataCount+4] = 1.0;
    	abData[dataCount+5] = 0.0;
    }else{
	glm::vec3 sumNormals;
	for (int i=-1; i<2; i+=1){
	    for (int j=-1; j<2; j+=1){
		glm::vec3 vecA, vecB, normal;
		vecA = glm::vec3((float)i, (heightmap[rowNum+i][columnNum] - heightmap[rowNum][columnNum]), 0.0f);
		vecB = glm::vec3(0.0f,     (heightmap[rowNum][columnNum+j] - heightmap[rowNum][columnNum]), (float)j);
		normal = glm::normalize(glm::cross(vecA, vecB));
		if(i+j==0)
		    normal = normal*(-1.0f);
		sumNormals += normal;
	    }
	}
	sumNormals = sumNormals*0.111f;
	abData[dataCount+3] = sumNormals[0];
    	abData[dataCount+4] = sumNormals[1];
    	abData[dataCount+5] = sumNormals[2];
    }

    //set Texture Coordinate
    abData[dataCount+6] = (float)(rowNum % 2);
    abData[dataCount+7] = (float)(columnNum % 2);
}

Model Terrain::getModel(){
    return Model(this->triangleMesh);
}

float** Terrain::getHeightmap(){
    return this->heightmap;
}

unsigned int Terrain::getHeightmapHeight(){
    //return this->heightmapHeight;
    return 2;
}

unsigned int Terrain::getHeightmapWidth(){
    //return this->heightmapWidth;
    return 2;
}











