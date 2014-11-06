#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "texture.hh"
#include <fstream>
//#include <winsock.h>		//on Windows
#include <netinet/in.h>		//on Unix
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>

class Terrain {
    public:
        Terrain();
        ~Terrain();
        void load();
        void render();
    private:
        float friction;
        Texture texture;
        std::string filePath;
	unsigned int heightmapWidth, heightmapHeight;
	unsigned int** heightmap;	//can be accessed like 'unsigned int[][]'
	bool heightmapChanged;
	ACGL::OpenGL::VertexArrayObject triangleMesh;

	void makeTriangleMesh();
};

#endif
