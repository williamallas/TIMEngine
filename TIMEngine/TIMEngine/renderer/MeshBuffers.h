#ifndef MESHBUFFERS_H_INCLUDED
#define MESHBUFFERS_H_INCLUDED

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class MeshBuffers
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

        bool isComplete() const;
        void setComplete(bool);

    private:
        IndexBuffer* _indexBuffer = nullptr;
        VertexBuffer* _vertexBuffer = nullptr;
        bool _isStreaming=false;

        VertexMode _primitive = VertexMode::TRIANGLES;
    };

    inline MeshBuffers::MeshBuffers(VertexBuffer* vb, IndexBuffer* ib) : _vertexBuffer(vb), _indexBuffer(ib) { if(_vertexBuffer)_vertexBuffer->computeBoundingVolume(); }
    inline MeshBuffers& MeshBuffers::setPrimitive(VertexMode primitive) { _primitive = primitive; }
    inline VertexBuffer* MeshBuffers::vertexBuffer() const { return _vertexBuffer; }
    inline IndexBuffer* MeshBuffers::indexBuffer() const { return _indexBuffer; }
    inline VertexMode MeshBuffers::primitive() const { return _primitive; }
    inline bool MeshBuffers::isComplete() const { return !_isStreaming; }

    inline void MeshBuffers::swap(MeshBuffers* mb)
    {
        boost::swap(_vertexBuffer, mb->_vertexBuffer);
        boost::swap(_indexBuffer, mb->_indexBuffer);
        boost::swap(_primitive, mb->_primitive);
        _isStreaming=false;
    }

    inline void MeshBuffers::setComplete(bool b)
    {
        if(!_indexBuffer && !_vertexBuffer)
            _isStreaming = !b;
    }

}
}
#include "MemoryLoggerOff.h"

#endif // MESHBUFFERS_H_INCLUDED
