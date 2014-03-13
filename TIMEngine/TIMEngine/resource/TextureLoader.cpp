#include "TextureLoader.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

unsigned char* TextureLoader::toGLFormat(unsigned char* ptr, unsigned char* out_ptr, const ImageFormat& format)
{
    if(!out_ptr)
        out_ptr = new unsigned char[format.size.x()*format.size.y()*format.nbComponent];

    for(size_t i=0 ; i<format.size.y() ; i++)
    {
        memcpy(out_ptr + i*format.size.x()*format.nbComponent,
               ptr + (format.size.y()-i-1)*format.size.x()*format.nbComponent,
               format.size.x()*format.nbComponent);
    }

    delete[] ptr;
    return out_ptr;
}

}
}
