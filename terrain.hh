#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "material.hh"
#include <fstream>
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>

class Terrain {
    public:
        Terrain(std::string filePath);
        Terrain();
        ~Terrain();
        void load();
        void render();
    private:
        Material material;
        std::string filePath;
	unsigned int heightmapWidth, heightmapHeight;
	float** heightmap;				//can be accessed like 'float[][]'
	bool heightmapChanged;
	ACGL::OpenGL::SharedVertexArrayObject triangleMesh;

	void makeTriangleMesh();
};

#endif
