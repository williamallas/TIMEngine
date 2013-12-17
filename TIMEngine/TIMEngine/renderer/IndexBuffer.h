#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

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

        unsigned int* data() const;

        unsigned int* createBuffer(size_t);
        void freeBuffer();
        void freeData();

        void uploadOnGpu(bool sendData=true, bool staticData=false) const;
        void flushOnGpu(size_t, size_t) const;

        uint id() const;

    private:
        uint _bufferId=0;

        size_t _size=0;
        unsigned int* _data=nullptr;

        #include "MemoryLoggerOff.h"
        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        #include "MemoryLoggerOn.h"
    };

    /* Inline implementation */
    inline uint IndexBuffer::id() const { return _bufferId; }
    inline unsigned int* IndexBuffer::data() const { return _data; }

}
}
#include "MemoryLoggerOff.h"

#endif // INDEXBUFFER_H
