#include "terrain.hh"

Terrain::Terrain(){
}

Terrain::~Terrain() {
}


void Terrain::load() {
    std::ifstream terrain_png(this->filePath);
    unsigned int rowNum, columnNum;

    terrain_png.seekg(16);					//skip part of the header
    terrain_png.read((char *)&this->heightmapWidth, 4);		//read width
    terrain_png.read((char *)&this->heightmapHeight, 4);	//read height
    this->heightmapWidth  = ntohl(this->heightmapWidth);	//convert from host to network byte order
    this->heightmapHeight = ntohl(this->heightmapHeight);			

    heightmap = new unsigned int*[this->heightmapHeight];		//initialize the heightmap
    for(rowNum = 0; rowNum < this->heightmapHeight; rowNum++){		//read in the heightmap
	    heightmap[rowNum] = new unsigned int[this->heightmapWidth];
        for(columnNum = 0; columnNum < this->heightmapWidth; columnNum++){
            terrain_png.read((char *)&heightmap[rowNum][columnNum], 1);
        }
    }

    this->makeTriangleMesh();
    heightmapChanged = false;

    texture.load();
}

void Terrain::makeTriangleMesh(){

    ACGL::OpenGL::ArrayBuffer arrayBuffer = ACGL::OpenGL::ArrayBuffer();
    arrayBuffer.defineAttribute("pos", GL_UNSIGNED_INT, 3);

    unsigned int rowNum=0, columnNum=0;				//initializing:
    bool movingRight = true, isUp = true;
    while(rowNum < this->heightmapHeight){			//traversing the Triangle Strip!
	unsigned int newPos[3];
	newPos[0] = rowNum;
	newPos[1] = columnNum;
	newPos[2] = heightmap[rowNum][columnNum];
        arrayBuffer.setDataElements(1, &newPos);
	if (isUp){
	    rowNum = rowNum + 1;
	    isUp = false;
	}else if (movingRight){
	    if (columnNum = this->heightmapWidth - 1){
		arrayBuffer.setDataElements(1, &newPos);
		arrayBuffer.setDataElements(1, &newPos);
		movingRight = false;
		rowNum = rowNum + 1;
	    } else{
		rowNum = rowNum - 1;
		columnNum = columnNum + 1;
		isUp = true;
	    }
	}else{
	    if (columnNum = 0){
		arrayBuffer.setDataElements(1, &newPos);
		arrayBuffer.setDataElements(1, &newPos);
		movingRight = true;
		rowNum = rowNum + 1;
	    }else{
		rowNum = rowNum - 1;
		columnNum = columnNum - 1;
		isUp = true;
	    }
	}
    }

    //this->triangleMesh = ACGL::OpenGL::VertexArrayObject();			//does not work since ACGL::OpenGL::VertexArrayObject is ACGL_NOT_COPYABLE
    this->triangleMesh.bind();
    this->triangleMesh.setMode(GL_TRIANGLE_STRIP);
    //this->triangleMesh.attachAllAttributes(arrayBuffer);
    //TODO unbind?
}

void Terrain::render() {
    if (heightmapChanged)
	this->makeTriangleMesh();
    this->triangleMesh.render();
}













