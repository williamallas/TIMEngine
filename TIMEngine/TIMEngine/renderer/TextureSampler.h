#ifndef TEXTURESAMPLER_H
#define TEXTURESAMPLER_H

#include "GLState.h"
#include "Texture.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class TextureSampler
    {
    public:
        static TextureSampler* defaultSamplerNoRepeat;
        static TextureSampler* defaultSamplerRepeat;
        static bool defaultMipmap;

        TextureSampler()
        {
            glGenSamplers(1, &_id);
        }

        virtual ~TextureSampler()
        {
            glDeleteSamplers(1, &_id);
        }

        uint id() const { return _id; }

        void set(const Texture::Parameter&);

    private:
        uint _id;
    };

}
}
#include "MemoryLoggerOn.h"

#endif // TEXTURESAMPLER_H
