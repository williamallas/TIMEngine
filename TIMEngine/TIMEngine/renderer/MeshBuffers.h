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
        MeshBuffers() {}
        MeshBuffers(const boost::container::vector<VertexBuffer*>&, IndexBuffer*);
        MeshBuffers(std::initializer_list<VertexBuffer*>, IndexBuffer*);
        virtual ~MeshBuffers() { delete[] _vertexBuffers; }

        MeshBuffers& setIndexBuffer(IndexBuffer*);
        MeshBuffers& setVertexBuffers(const boost::container::vector<VertexBuffer*>&);
        MeshBuffers& setVertexBuffers(std::initializer_list<VertexBuffer*>);
        MeshBuffers& setPrimitive(VertexMode primitive);

        IndexBuffer* indexBuffer() const;
        VertexBuffer* vertexBuffer(size_t) const;
        size_t nbVertexBuffers() const;
        VertexMode primitive() const;

    private:
        IndexBuffer* _indexBuffer = nullptr;
        size_t _vertexBuffersSize = 0;
        VertexBuffer** _vertexBuffers = nullptr;

        VertexMode _primitive = VertexMode::TRIANGLES;
    };

    inline MeshBuffers::MeshBuffers(const boost::container::vector<VertexBuffer*>& vb, IndexBuffer* ib)
    {
        setIndexBuffer(ib);
        setVertexBuffers(vb);
    }

    inline MeshBuffers::MeshBuffers(std::initializer_list<VertexBuffer*> vb, IndexBuffer* ib)
    {
        setIndexBuffer(ib);
        setVertexBuffers(boost::container::vector<VertexBuffer*>(vb.begin(), vb.end()));
    }

    inline MeshBuffers& MeshBuffers::setIndexBuffer(IndexBuffer* ib) { _indexBuffer = ib; return *this; }

    inline MeshBuffers& MeshBuffers::setVertexBuffers(const boost::container::vector<VertexBuffer*>& vb)
    {
        delete[] _vertexBuffers;
        _vertexBuffers = new VertexBuffer*[vb.size()];
        _vertexBuffersSize = vb.size();
        for(size_t i=0 ; i<vb.size() ; i++) { _vertexBuffers[i] = vb[i]; }

        return *this;
    }

    inline MeshBuffers& MeshBuffers::setPrimitive(VertexMode primitive) { _primitive = primitive; }

    inline MeshBuffers& MeshBuffers::setVertexBuffers(std::initializer_list<VertexBuffer*> vb)
    { setVertexBuffers(boost::container::vector<VertexBuffer*>(vb.begin(), vb.end())); }

    inline VertexBuffer* MeshBuffers::vertexBuffer(size_t index) const { _vertexBuffers[index]; }
    inline size_t MeshBuffers::nbVertexBuffers() const { return _vertexBuffersSize; }
    inline IndexBuffer* MeshBuffers::indexBuffer() const { return _indexBuffer; }
    inline VertexMode MeshBuffers::primitive() const { return _primitive; }

}
}
#include "MemoryLoggerOff.h"

#endif // MESHBUFFERS_H_INCLUDED
