#include "VertexBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

VertexBuffer::VertexBuffer()
{

}

VertexBuffer::~VertexBuffer()
{
    freeBuffer();
}

size_t VertexBuffer::vertexFormatSize(VertexFormat format)
{
    switch(format)
    {
        case V:    return sizeof(vec3)/sizeof(float);
        case VN:   return sizeof(VN_Vertex)/sizeof(float);
        case VNC:  return sizeof(VNC_Vertex)/sizeof(float);
        case VNCT: return sizeof(VNCT_Vertex)/sizeof(float);
        case VC:   return sizeof(VC_Vertex)/sizeof(float);
    };
}

float* VertexBuffer::createBuffer(VertexFormat format, size_t size)
{
    _format = format;
    _size = size;
    _formatSize = vertexFormatSize(format);
    _data = new float[_formatSize*size];

    glGenBuffers(1, &_bufferId);
    return _data;
}

void VertexBuffer::uploadOnGpu(bool sendData, bool staticData) const
{
    openGL.bindArrayBuffer(_bufferId);

    glBufferData(GL_ARRAY_BUFFER,
                 _formatSize*_size*sizeof(float),
                 sendData?_data:nullptr,
                 staticData?GL_STATIC_DRAW:GL_STREAM_DRAW);
}

void VertexBuffer::flushOnGpu(size_t begin, size_t size) const
{
    openGL.bindArrayBuffer(_bufferId);

    glBufferSubData(GL_ARRAY_BUFFER,
                    begin*_formatSize*sizeof(float),
                    size*_formatSize*sizeof(float),
                    _data+begin*_formatSize);
}

void VertexBuffer::freeBuffer()
{
    glDeleteBuffers(1, &_bufferId);
    _size = 0;
    _formatSize = 0;

    freeData();
}

void VertexBuffer::freeData()
{
    delete[] _data;
    _data = nullptr;
}

}
}
