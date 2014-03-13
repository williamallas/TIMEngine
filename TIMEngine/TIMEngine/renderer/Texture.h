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

    class TextureSampler;
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

        struct Parameter
        {
            enum InternFormat
            {
                R, R8, R16, R16F, R32F,
                RGB, RGB8, RGB16, RGB16F, RGB32F,
                RGBA, RGBA8, RGBA16, RGBA16F, RGBA32F,
                DEPTHCOMPONENT,
            };
            bool repeat=false, mipmap=false, linear=false, trilinear=false, anisotropy=false, depthMode=false;
            uint anisotropyFilter=0;
            InternFormat intern=RGBA;
            bool defaultSampler=false;
        };

        static GLenum toGLType(Type);

        static Texture* genTexture2D(const uivec2&, const float*, uint nbComponent, const Parameter&);
        static Texture* genTexture2D(const uivec2&, const unsigned char*, uint nbComponent, const Parameter&);

        Texture();
        virtual ~Texture();

        #include "MemoryLoggerOff.h"
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        #include "MemoryLoggerOn.h"

        uint id() const;
        Type type() const;
        Format format() const;
        uivec2 resolution() const;
        uivec3 size() const;
        TextureSampler* sampler() const;
        size_t bytePerPixel() const;

        void setParameter(const Parameter&) const;
        void setSampler(TextureSampler*);

        void swap(Texture&);

    private:
        uint _id=0;
        size_t _pixelByteSize;
        uivec3 _size;
        Type _type=TEXTURE_2D;
        Format _format=RGB;
        TextureSampler* _parameter = nullptr;

        static Texture* genTexture2D(uint dataType, const uivec2&, const void*, uint nbComponent, const Parameter&);

    };

    inline uint Texture::id() const { return _id; }
    inline Texture::Type Texture::type() const { return _type; }
    inline Texture::Format Texture::format() const { return _format; }
    inline uivec2 Texture::resolution() const { return _size.to<2>(); }
    inline uivec3 Texture::size() const { return _size; }
    inline TextureSampler* Texture::sampler() const { return _parameter; }
    inline size_t Texture::bytePerPixel() const { return _pixelByteSize; }

    inline void Texture::swap(Texture& tex)
    {
        std::swap(_id, tex._id);
        std::swap(_size, tex._size);
        std::swap(_type, tex._type);
        std::swap(_format, tex._format);
        std::swap(_parameter, tex._parameter);
        std::swap(_pixelByteSize, tex._pixelByteSize);
    }

    inline GLenum Texture::toGLType(Type t)
    {
        static const GLenum glType[] = { GL_TEXTURE_2D,
                                         GL_TEXTURE_3D,
                                         GL_TEXTURE_CUBE_MAP,
                                         GL_TEXTURE_2D_ARRAY,
                                       };
        return glType[t];
    }

}
}
#include "MemoryLoggerOff.h"

#endif // TEXTURE_H
