#ifndef METASHADER_H
#define METASHADER_H

#include "core/core.h"
#include "GLState.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class MetaShader
    {
    public:
        enum ShaderType
        {
            VERTEX_SHADER,
            PIXEL_SHADER,
            GEOMETRY_SHADER
        };

        static GLenum GLShaderType(ShaderType);

        MetaShader(ShaderType);
        virtual ~MetaShader();

        ShaderType type() const;

        void setSource(const std::string&);

        Option<uint> compile(std::initializer_list<std::string>);
        Option<uint> compile(const boost::container::set<std::string>&);

        const std::string& error() const;

    private:
        ShaderType _shaderType;
        std::string _source;
        std::string _lastError;
        boost::container::map<boost::container::set<std::string>, uint> _shader;

        void logError(uint);

    };

    inline const std::string& MetaShader::error() const { return _lastError; }

}
}
#include "MemoryLoggerOff.h"


#endif // METASHADER_H
