#ifndef MODEL_HH_INCLUDED
#define MODEL_HH_INCLUDED

#include <string>
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>

class Model {
    public:
        Model(std::string filePath, float scale=1.0f);
	Model(ACGL::OpenGL::SharedVertexArrayObject vao, float scale=1.0f);
	    Model();
        ~Model();
        ACGL::OpenGL::SharedVertexArrayObject getReference();
        void setScale(float scale);
        float getScale();
    private:
        ACGL::OpenGL::SharedVertexArrayObject reference;
        float scale;
};

#endif
