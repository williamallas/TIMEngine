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
        _lastLinkError.clear();

        if(!vs.hasValue())
            _lastLinkError = "Vertex shader missing.\n";
        if(!fs.hasValue())
            _lastLinkError += "Fragment shader missing.\n";

        return Option<Shader*>();
    }

    uint id = glCreateProgram();
    glAttachShader(id, vs.value());
    glAttachShader(id, fs.value());

    if(gs.hasValue())
    {
        if(gs.value() != 0)
            glAttachShader(id, gs.value());
    }

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
    prog->loadEngineAttrib();
    prog->loadEngineUniform();
    return Option<Shader*>(prog);
}

const std::string& Shader::lastLinkError()
{
    return _lastLinkError;
}

void Shader::loadEngineUniform()
{
    _engineUniform[EngineUniform::MODEL] = glGetUniformLocation(_id, "model");
    _engineUniform[EngineUniform::VIEW] = glGetUniformLocation(_id, "view");
    _engineUniform[EngineUniform::PROJ] = glGetUniformLocation(_id, "projection");
    _engineUniform[EngineUniform::PROJVIEW] = glGetUniformLocation(_id, "projView");
    _engineUniform[EngineUniform::INV_VIEW] = glGetUniformLocation(_id, "invView");
    _engineUniform[EngineUniform::INV_PROJVIEW] = glGetUniformLocation(_id, "invProjView");

    _engineUniform[EngineUniform::COLOR] = glGetUniformLocation(_id, "color");
    _engineUniform[EngineUniform::MATERIAL] = glGetUniformLocation(_id, "material");
    _engineUniform[EngineUniform::EXPONENT] = glGetUniformLocation(_id, "exponent");

    _engineUniform[EngineUniform::TIME] = glGetUniformLocation(_id, "time");
    _engineUniform[EngineUniform::FRAME_TIME] = glGetUniformLocation(_id, "timeFrame");
    _engineUniform[EngineUniform::CAMERA_WORLD] = glGetUniformLocation(_id, "cameraWorld");

    openGL.bindShader(_id);

    bool findOne=false;
    for(int i=0 ; i<MAX_TEXTURE_UNIT ; i++)
    {
        _uniformTextureId[i] = glGetUniformLocation(_id, (std::string("texture")+StringUtils(i).str()).c_str());
        findOne |= (_uniformTextureId[i]>=0);
        if(_uniformTextureId[i] >= 0)
            glUniform1i(_uniformTextureId[i], i);
    }

    if(!findOne)
    {
        for(int i=0 ; i<MAX_TEXTURE_UNIT ; i++)
        {
            _uniformTextureId[i] = glGetUniformLocation(_id, (std::string("texture[")+StringUtils(i).str()+"]").c_str());
            if(_uniformTextureId[i] >= 0)
                glUniform1i(_uniformTextureId[i], i);
        }
    }
}

void Shader::loadEngineAttrib()
{
    _engineAttrib[EngineAttrib::VERTEX] = glGetAttribLocation(_id, "vertex");
    _engineAttrib[EngineAttrib::NORMAL] = glGetAttribLocation(_id, "normal");
    _engineAttrib[EngineAttrib::TEXCOORD] = glGetAttribLocation(_id, "texCoord");
    _engineAttrib[EngineAttrib::TANGENT] = glGetAttribLocation(_id, "tangent");
}

}
}
