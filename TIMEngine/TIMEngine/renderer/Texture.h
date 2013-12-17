#ifndef TEXTURE_H
#define TEXTURE_H

#include "GL/glew.h"
#include "core/core.h"

#include "GLState.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class Texture
    {
    public:
        enum Type
        {
            TEXTURE_2D,
            TEXTURE_3D,
            TEXTURE_CUBE,
            TEXTURE_ARRAY,
        };

        enum Format
        {
            R,
            RGB,
            RGBA,
            DEPTH,
        };

        Texture();
        virtual ~Texture();

        uint id() const;
        Type type() const;
        Format format() const;
        uivec2 resolution() const;
        uivec3 size() const;

    private:
        uint _id;
        uivec3 _size;
        Type _type;
        Format _format;

    };

    inline uint Texture::id() const { return _id; }
    inline Texture::Type Texture::type() const { return _type; }
    inline Texture::Format Texture::format() const { return _format; }
    inline uivec2 Texture::resolution() const { return _size.to<2>(); }
    inline uivec3 Texture::size() const { return _size; }

}
}
#include "MemoryLoggerOff.h"

#endif // TEXTURE_H
