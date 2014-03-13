#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "GLState.h"
#include "VertexFormat.h"
#include "core.h"

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

        struct VertexAttrib
        {
            uint nbComponent, offset; // nbCompnent=0 if disabled

            bool operator==(const VertexAttrib& v) const
            {
                if(nbComponent == v.nbComponent)
                {
                    if(nbComponent==0) return true;
                    else return offset==v.offset;
                }
                return false;
            }

            bool operator!=(const VertexAttrib& v) const
            {
                return !(*this==v);
            }

            enum
            {
                VERTEX=0,
                NORMAL,
                TEXCOORD,
                TANGENT,
            };
        };

        static const VertexAttrib& vertexEngineAttrib(VertexFormat, int);

        VertexBuffer();
        virtual ~VertexBuffer();

        float* data() const;

        float* createBuffer(VertexFormat, size_t);
        void freeBuffer();
        void freeData();

        void uploadOnGpu(bool sendData=true, bool staticData=false);
        void flushOnGpu(size_t, size_t) const;

        uint id() const;
        VertexFormat format() const;
        size_t formatSize() const;
        size_t size() const;

        void computeBoundingVolume();
        const Sphere& sphere() const;
        const Box& box() const;

        void bindVertexAttrib(uint, const VertexAttrib&);

    private:
        uint _bufferId=0;

        size_t _size=0, _formatSize=0;
        float* _data=nullptr;
        VertexFormat _format=VNC;

        Sphere _bSphere;
        Box _aabb;

        #include "MemoryLoggerOff.h"
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;
        #include "MemoryLoggerOn.h"

        /* Stat VAO (vertex attrib) */
        static const int MAX_VAO_ATTRIB=8;
        VertexAttrib _attribBinding[MAX_VAO_ATTRIB];
    };

    /* Inline implementation */
    inline uint VertexBuffer::id() const { return _bufferId; }
    inline float* VertexBuffer::data() const { return _data; }
    inline VertexFormat VertexBuffer::format() const { return _format; }
    inline size_t VertexBuffer::formatSize() const { return _formatSize; }
    inline size_t VertexBuffer::size() const { return _size; }
    inline const Sphere& VertexBuffer::sphere() const { return _bSphere; }
    inline const Box& VertexBuffer::box() const { return _aabb; }

    inline const VertexBuffer::VertexAttrib& VertexBuffer::vertexEngineAttrib(VertexFormat format, int attrib)
    {
        static const VertexAttrib lookup[(VertexFormat::VC+1)*(VertexAttrib::TANGENT+1)] = {
            {3,0},{0,0},{0,0},{0,0}, // V
            {3,0},{3,3},{0,0},{0,0}, // VN
            {3,0},{3,3},{2,6},{0,0}, // VNC
            {3,0},{3,3},{2,6},{3,8}, // VNCT
            {3,0},{0,0},{2,3},{0,0}, // VC
        };
        return lookup[format*(VertexAttrib::TANGENT+1)+attrib];
    }
}
}
#include "MemoryLoggerOff.h"

#endif // VERTEXBUFFER_H
