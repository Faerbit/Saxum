#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "material.hh"
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include "model.hh"
class Terrain {
    public:
        Terrain(std::string levelNum);
        Terrain();
        ~Terrain();
        void load();
        void render();
        Model getModel();
        float** getHeightmap();
        unsigned int getHeightmapHeight();
        unsigned int getHeightmapWidth();

    private:
        Material material;
        std::string heightmapFilePath;
        unsigned int heightmapHeight, heightmapWidth;
        float** heightmap;				//can be accessed like 'float[][]'
        bool heightmapChanged;
        ACGL::OpenGL::SharedVertexArrayObject triangleMesh;
        void makeTriangleMesh();
        void set_abData(float* abData, unsigned int dataCount, unsigned int rowNum, unsigned int columnNum);
};

#endif
