#include "MetaShader.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

GLenum MetaShader::GLShaderType(ShaderType type)
{
    switch(type)
    {
        case VERTEX_SHADER: return GL_VERTEX_SHADER;
        case PIXEL_SHADER : return GL_FRAGMENT_SHADER;
        case GEOMETRY_SHADER : return GL_GEOMETRY_SHADER;
    }
}

MetaShader::MetaShader(ShaderType shaderType)
{
    _shaderType = shaderType;
}

MetaShader::~MetaShader()
{
    for(auto &s : _shader)
    {
        glDeleteShader(s.second);
    }
}

void MetaShader::setSource(const std::string& str)
{
    _source = str;
}

Option<core::uint> MetaShader::compile(std::initializer_list<std::string> flags)
{
    return compile(boost::container::set<std::string>(flags.begin(), flags.end()));
}

Option<core::uint> MetaShader::compile(const boost::container::set<std::string>& flags)
{
    auto it = _shader.find(flags);
    if(it != _shader.end())
        return Option<core::uint>(it->second);

    std::string finalSource="";
    for(auto it=flags.begin() ; it != flags.end() ; ++it)
        finalSource += "#define "+*it+"\n";

    finalSource += _source;

    core::uint id = glCreateShader(GLShaderType(_shaderType));
    const GLchar* gchar = (const GLchar*)finalSource.c_str();
    glShaderSource(id, 1, &gchar, NULL);
    glCompileShader(id);

    int compileStatus = GL_TRUE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);

    if(compileStatus != GL_TRUE)
    {
        _lastError="Compiling with:";
        for(auto it=flags.begin() ; it != flags.end() ; ++it)
            _lastError += *it +" ";
        _lastError += "\n";
        logError(id);
        glDeleteShader(id);
        return Option<core::uint>();
    }

    _shader[flags] = id;

    return Option<core::uint>(id);
}

void MetaShader::logError(core::uint shaderId)
{
    int logSize;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

    char* log = new char[logSize];
    glGetShaderInfoLog(shaderId, logSize, &logSize, log);

    _lastError += std::string(log);
    delete[] log;
}

}
}
