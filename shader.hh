#ifndef SHADER_HH_INCLUDED
#define SHADER_HH_INCLUDED

#include "model.hh"
#include <string>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

class Shader {
    public:
        Shader(std::string filePath, Model model);
	Shader();
        ACGL::OpenGL::SharedShaderProgram getReference();
        ~Shader();
    private:
        ACGL::OpenGL::SharedShaderProgram reference;
};

#endif
