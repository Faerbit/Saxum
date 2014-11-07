#ifndef SHADER_HH_INCLUDED
#define SHADER_HH_INCLUDED

#include "model.hh"
#include <string>
#include <ACGL/OpenGL/Creator/ShaderProgramCreator.hh>

class OurShader {
    public:
        OurShader(std::string filePath, Model model);
	OurShader();
        ACGL::OpenGL::SharedShaderProgram getReference();
        ~OurShader();
    private:
        ACGL::OpenGL::SharedShaderProgram reference;
};

#endif
