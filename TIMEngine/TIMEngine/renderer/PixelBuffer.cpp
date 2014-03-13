#include "PixelBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

PixelBuffer::PixelBuffer()
{
    glGenBuffers(1, &_id);
}

PixelBuffer::~PixelBuffer()
{
    glDeleteBuffers(1, &_id);
}

void* PixelBuffer::allocSpace(size_t size) const
{
    bindUpload();
    glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_STREAM_DRAW);
    void* ptr = glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
    openGL.bindPixelBufferUnpack(0);
    return ptr;
}

void PixelBuffer::endUpload() const
{
    bindUpload();
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
}

}
}
