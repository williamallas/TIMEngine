#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "GLState.h"
#include "VertexFormat.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class VertexBuffer
    {
    public:
        static size_t vertexFormatSize(VertexFormat);

        VertexBuffer();
        virtual ~VertexBuffer();

        float* data() const;

        float* createBuffer(VertexFormat, size_t);
        void freeBuffer();
        void freeData();

        void uploadOnGpu(bool sendData=true, bool staticData=false) const;
        void flushOnGpu(size_t, size_t) const;

        uint id() const;

    private:
        uint _bufferId=0;

        size_t _size=0, _formatSize=0;
        float* _data=nullptr;
        VertexFormat _format=VNC;

        #include "MemoryLoggerOff.h"
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        #include "MemoryLoggerOn.h"

    };

    /* Inline implementation */
    inline uint VertexBuffer::id() const { return _bufferId; }
    inline float* VertexBuffer::data() const { return _data; }
}
}
#include "MemoryLoggerOff.h"

#endif // VERTEXBUFFER_H
