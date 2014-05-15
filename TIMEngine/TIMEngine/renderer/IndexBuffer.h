#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "VertexFormat.h"
#include "GLState.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class IndexBuffer
    {
    public:
        IndexBuffer();
        virtual ~IndexBuffer();

        uint* data() const;
        size_t size() const;

        uint* createBuffer(size_t);
        void freeBuffer();
        void freeData();

        void uploadOnGpu(bool sendData=true, bool staticData=false);
        void flushOnGpu(size_t, size_t) const;

        uint id() const;

        void draw(size_t, VertexMode, size_t) const;

    private:
        static const uint GLPrimitive[6]; // see VertexMode
        uint _bufferId=0;

        size_t _size=0;
        uint* _data=nullptr;

        #include "MemoryLoggerOff.h"
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        #include "MemoryLoggerOn.h"
    };

    /* Inline implementation */
    inline uint IndexBuffer::id() const { return _bufferId; }
    inline uint* IndexBuffer::data() const { return _data; }
    inline size_t IndexBuffer::size() const { return _size; }

}
}
#include "MemoryLoggerOff.h"

#endif // INDEXBUFFER_H
