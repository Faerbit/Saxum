#ifndef TERRAIN_HH_INCLUDED
#define TERRAIN_HH_INCLUDED

#include <string>
#include "material.hh"
#include <ACGL/OpenGL/Objects/VertexArrayObject.hh>
#include "model.hh"
class Terrain {
    public:
        Terrain(std::string heightmapFilePath);
        Terrain();
        ~Terrain();
        void load();
        void render();
        Model getModel();
        float** getHeightmap();
        int getHeightmapHeight();
        int getHeightmapWidth();
        void makeTriangleMesh(int startX, int startZ, int endX, int endZ);
    private:
        Material material;
        std::string heightmapFilePath;
        int heightmapHeight, heightmapWidth;
        float** heightmap; //can be accessed like 'float[][]'
        bool heightmapChanged;
        ACGL::OpenGL::SharedVertexArrayObject triangleMesh;
        void set_abData(float* abData, int dataCount, int rowNum, int columnNum);
};

#endif
