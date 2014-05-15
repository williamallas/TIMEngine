#ifndef TEXTURELOADER_H_INCLUDED
#define TEXTURELOADER_H_INCLUDED

#include "core/core.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

    class TextureLoader
    {
    public:
        struct ImageFormat
        {
            uivec2 size;
            size_t nbComponent;
        };

        TextureLoader()
        {
            #if SDL_BYTEORDER == SDL_BIG_ENDIAN
            mask[0] = 0xff000000;
            mask[1] = 0x00ff0000;
            mask[2] = 0x0000ff00;
            mask[3] = 0x000000ff;
            #else
            mask[0] = 0x000000ff;
            mask[1] = 0x0000ff00;
            mask[2] = 0x00ff0000;
            mask[3] = 0xff000000;
            #endif
        }

        virtual unsigned char* loadImage(const std::string&, ImageFormat&) const = 0;
        virtual unsigned char* loadImageArray(const boost::container::vector<std::string>&, ImageFormat&) const = 0;

        static unsigned char* toGLFormat(unsigned char*, unsigned char*, const ImageFormat&);

    protected:
        uivec4 mask;
    };
}
}
#include "MemoryLoggerOff.h"

#endif // TEXTURELOADER_H_INCLUDED
