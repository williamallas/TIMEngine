#ifndef SHADER_H
#define SHADER_H

#include "GL/glew.h"
#include "core/core.h"
#include "GLState.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class Shader
    {
    public:
        static Option<Shader*> combine(const Option<uint>&, const Option<uint>&, const Option<uint>& gs=Option<uint>());
        static const std::string& lastLinkError();

        virtual ~Shader();
        uint id() const;

        void bind();

    private:
        Shader();
        uint _id;

        static std::string _lastLinkError;
    };

    /* inline implementation */
    inline uint Shader::id() const { return _id; }
    inline void Shader::bind() { openGL.bindShader(_id); }
}
}
#include "MemoryLoggerOff.h"

#endif // SHADER_H
