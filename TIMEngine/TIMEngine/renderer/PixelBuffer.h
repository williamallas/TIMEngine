#ifndef PIXELBUFFER_H
#define PIXELBUFFER_H

#include "core/core.h"
#include "GLState.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class PixelBuffer
    {
    public:
        PixelBuffer();
        virtual ~PixelBuffer();

        void* allocSpace(size_t) const;
        void endUpload() const;
        static void unbind();

    private:
        uint _id;

        void bindUpload() const;
    };

    inline void PixelBuffer::bindUpload() const { openGL.bindPixelBufferUnpack(_id); }
    inline void PixelBuffer::unbind() { openGL.bindPixelBufferUnpack(0); }
}
}
#include "MemoryLoggerOff.h"

#endif // PIXELBUFFER_H
