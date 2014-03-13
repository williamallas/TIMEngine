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

    return _data;
}

void VertexBuffer::uploadOnGpu(bool sendData, bool staticData)
{
    if(_bufferId == 0)
        glGenBuffers(1, &_bufferId);

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

void VertexBuffer::bindVertexAttrib(uint position, const VertexAttrib& attrib)
{
    #define BUFFER_OFFSET(a) ((char*)NULL + (a))
    if(_attribBinding[position] != attrib)
    {
        _attribBinding[position] = attrib;
        if(!attrib.nbComponent)
            glDisableVertexAttribArray(position);
        else
            glEnableVertexAttribArray(position);
    }

    if(attrib.nbComponent)
        glVertexAttribPointer(position, attrib.nbComponent, GL_FLOAT, 0, _formatSize*sizeof(float), BUFFER_OFFSET(attrib.offset*sizeof(float)));
}

void VertexBuffer::computeBoundingVolume()
{
    vec2 x(INFINITY, -INFINITY),y(INFINITY, -INFINITY),z(INFINITY, -INFINITY);

    for(size_t i=0 ; i<_size ; i++)
    {
        vec3 v={_data[i*_formatSize], _data[i*_formatSize+1], _data[i*_formatSize+2]};
        x.set(std::min(v.x(), x.x()), 0);
        x.set(std::max(v.x(), x.y()), 1);

        y.set(std::min(v.y(), y.x()), 0);
        y.set(std::max(v.y(), y.y()), 1);

        z.set(std::min(v.z(), z.x()), 0);
        z.set(std::max(v.z(), z.y()), 1);
    }

    _aabb = Box({x,y,z});

    vec3 center = _aabb.center();

    float rad2=0;
    for(size_t i=0 ; i<_size ; i++)
    {
        vec3 v={_data[i*_formatSize], _data[i*_formatSize+1], _data[i*_formatSize+2]};
        rad2 = std::max(rad2, (v-center).length2());
    }
    _bSphere = Sphere(center, sqrtf(rad2));
}

}
}
