#ifndef MESHBUFFERS_H_INCLUDED
#define MESHBUFFERS_H_INCLUDED

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ResourceInterface.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class MeshBuffers : public ResourceInterface
    {

    public:
        MeshBuffers(VertexBuffer* vb, IndexBuffer* ib);

        virtual ~MeshBuffers()
        {
            delete _vertexBuffer;
            delete _indexBuffer;
        }

        MeshBuffers& setPrimitive(VertexMode primitive);

        IndexBuffer* indexBuffer() const;
        VertexBuffer* vertexBuffer() const;
        VertexMode primitive() const;

        void swap(MeshBuffers*);

    private:
        IndexBuffer* _indexBuffer = nullptr;
        VertexBuffer* _vertexBuffer = nullptr;

        VertexMode _primitive = VertexMode::TRIANGLES;
    };

    inline MeshBuffers::MeshBuffers(VertexBuffer* vb, IndexBuffer* ib) : ResourceInterface(),
                                                                         _vertexBuffer(vb),
                                                                         _indexBuffer(ib)
    { if(_vertexBuffer)_vertexBuffer->computeBoundingVolume(); }

    inline MeshBuffers& MeshBuffers::setPrimitive(VertexMode primitive) { _primitive = primitive; }
    inline VertexBuffer* MeshBuffers::vertexBuffer() const { return _vertexBuffer; }
    inline IndexBuffer* MeshBuffers::indexBuffer() const { return _indexBuffer; }
    inline VertexMode MeshBuffers::primitive() const { return _primitive; }

    inline void MeshBuffers::swap(MeshBuffers* mb)
    {
        if(mb)
        {
            boost::swap(_vertexBuffer, mb->_vertexBuffer);
            boost::swap(_indexBuffer, mb->_indexBuffer);
            boost::swap(_primitive, mb->_primitive);
        }
    }

}
}
#include "MemoryLoggerOff.h"

#endif // MESHBUFFERS_H_INCLUDED
