#include "IndexBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

IndexBuffer::IndexBuffer()
{

}

IndexBuffer::~IndexBuffer()
{
    freeBuffer();
}


unsigned int* IndexBuffer::createBuffer(size_t size)
{
    _size = size;
    _data = new unsigned int[size];

    glGenBuffers(1, &_bufferId);
    return _data;
}

void IndexBuffer::uploadOnGpu(bool sendData, bool staticData) const
{
    openGL.bindElementArrayBuffer(_bufferId);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 _size*sizeof(unsigned int),
                 sendData?_data:nullptr,
                 staticData?GL_STATIC_DRAW:GL_STREAM_DRAW);
}

void IndexBuffer::flushOnGpu(size_t begin, size_t size) const
{
    openGL.bindElementArrayBuffer(_bufferId);

    glBufferSubData(GL_ARRAY_BUFFER,
                    begin*sizeof(unsigned int),
                    size*sizeof(unsigned int),
                    _data+begin);
}

void IndexBuffer::freeBuffer()
{
    glDeleteBuffers(1, &_bufferId);
    _size = 0;

    freeData();
}

void IndexBuffer::freeData()
{
    delete[] _data;
    _data = nullptr;
}

}
}
