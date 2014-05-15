#include "IndexBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

const uint IndexBuffer::GLPrimitive[6]={GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_LINES, GL_LINE_STRIP, GL_POINTS, GL_QUADS}; // see VertexMode

IndexBuffer::IndexBuffer()
{

}

IndexBuffer::~IndexBuffer()
{
    freeBuffer();
}


uint* IndexBuffer::createBuffer(size_t size)
{
    _size = size;
    _data = new uint[size];

    glGenBuffers(1, &_bufferId);
    return _data;
}

void IndexBuffer::uploadOnGpu(bool sendData, bool staticData)
{
    if(_bufferId == 0)
        glGenBuffers(1, &_bufferId);

    openGL.bindElementArrayBuffer(_bufferId);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 _size*sizeof(uint),
                 sendData?_data:nullptr,
                 staticData?GL_STATIC_DRAW:GL_STREAM_DRAW);
}

void IndexBuffer::flushOnGpu(size_t begin, size_t size) const
{
    openGL.bindElementArrayBuffer(_bufferId);

    glBufferSubData(GL_ARRAY_BUFFER,
                    begin*sizeof(uint),
                    size*sizeof(uint),
                    _data+begin);
}

void IndexBuffer::freeBuffer()
{
    openGL.unbindElementArrayBuffer(_bufferId);
    glDeleteBuffers(1, &_bufferId);
    _size = 0;
    _bufferId = 0;

    freeData();
}

void IndexBuffer::freeData()
{
    delete[] _data;
    _data = nullptr;
}

void IndexBuffer::draw(size_t s, VertexMode primitive, size_t nbInstance) const
{
    if(s<=_size)
        glDrawElementsInstanced(IndexBuffer::GLPrimitive[primitive], s, GL_UNSIGNED_INT, 0, nbInstance);
}

}
}
