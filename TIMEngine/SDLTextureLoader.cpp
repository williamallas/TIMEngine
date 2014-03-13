#include "SDLTextureLoader.h"

#include "MemoryLoggerOn.h"
namespace tim
{

SDLTextureLoader::SDLTextureLoader() : TextureLoader()
{

}

unsigned char* SDLTextureLoader::loadImage(const std::string& file, ImageFormat& format) const
{
    SDL_Surface* img = IMG_Load(file.c_str());
    if(!img)
        return nullptr;

    SDL_LockSurface(img);

    format.nbComponent = img->format->BytesPerPixel==4?4:3;
    format.size = { (size_t)img->w, (size_t)img->h };

    SDL_PixelFormat img_format = *(img->format);

    img_format.Rmask=mask[0];
    img_format.Gmask=mask[1];
    img_format.Bmask=mask[2];
    img_format.Amask=mask[3];
    if(format.nbComponent != 4)
    {
        img_format.BitsPerPixel = 24;
        img_format.BytesPerPixel = 3;
    }

    SDL_Surface* converted = SDL_ConvertSurface(img, &img_format, SDL_SWSURFACE);

    SDL_LockSurface(converted);
    unsigned char* pixels = (unsigned char*)converted->pixels;
    converted->pixels = nullptr;

    SDL_UnlockSurface(converted);
    SDL_UnlockSurface(img);

    SDL_FreeSurface(converted);
    SDL_FreeSurface(img);

    return pixels;
}

}
