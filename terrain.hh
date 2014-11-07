#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "texture.hh"
#include <fstream>

class Terrain {
    public:
        Terrain(std::string filePath);
	Terrain();
        ~Terrain();
        void load();
        void render();
    private:
        Texture texture;
        std::string filePath;
	unsigned int heightmapWidth, heightmapHeight;
	float** heightmap;				//can be accessed like 'float[][]'
	bool heightmapChanged;
	ACGL::OpenGL::SharedVertexArrayObject triangleMesh;

	void makeTriangleMesh();
};

#endif
