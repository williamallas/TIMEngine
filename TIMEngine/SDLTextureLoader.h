#ifndef SDLTEXTURELOADER_H
#define SDLTEXTURELOADER_H

#include <SDL_image.h>
#include "resource/TextureLoader.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    class SDLTextureLoader : public resource::TextureLoader
    {
    public:
        SDLTextureLoader();

        virtual unsigned char* loadImage(const std::string&, ImageFormat&) const;

    private:
    };

}
#include "MemoryLoggerOff.h"

#endif // SDLTEXTURELOADER_H
