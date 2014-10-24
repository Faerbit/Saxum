#ifndef MODEL_HH_INCLUDED
#define MODEL_HH_INCLUDED

#include <string>
#include <ACGL/OpenGL/Creator/VertexArrayObjectCreator.hh>

class Model {
    public:
        Model(std::string filePath);
	Model();
        ~Model();
        ACGL::OpenGL::SharedVertexArrayObject getReference();
    private:
        ACGL::OpenGL::SharedVertexArrayObject reference;
};

#endif
