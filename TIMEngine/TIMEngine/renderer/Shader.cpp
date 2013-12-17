#include "Shader.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

Shader::Shader()
{

}

Shader::~Shader()
{
    glDeleteProgram(_id);
}

std::string Shader::_lastLinkError = "";

Option<Shader*> Shader::combine(const Option<uint>& vs, const Option<uint>& fs, const Option<uint>& gs)
{
    if(!vs.hasValue() || !fs.hasValue())
    {
        _lastLinkError = "Vertex or Fragment shader missing.";
        return Option<Shader*>();
    }

    uint id = glCreateProgram();
    glAttachShader(id, vs.value());
    glAttachShader(id, fs.value());

    if(gs.hasValue())
        glAttachShader(id, gs.value());

    glLinkProgram(id);

    int linkStatus = GL_TRUE;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);

    if(linkStatus != GL_TRUE)
    {
        int logSize;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);

        char* log = new char[logSize];
        glGetProgramInfoLog(id, logSize, &logSize, log);

        _lastLinkError = std::string(log);
        delete[] log;
        glDeleteProgram(id);
        return Option<Shader*>();
    }


    Shader* prog = new Shader;
    prog->_id = id;
    return Option<Shader*>(prog);
}

const std::string& Shader::lastLinkError()
{
    return _lastLinkError;
}

}
}
