#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "core/core.h"
#include "Texture.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class FrameBuffer
    {
    public:
        FrameBuffer(const boost::container::vector<Texture*>&, Texture* depthBuf=nullptr);
        virtual ~FrameBuffer();

        Texture* depthBuffer() const;
        Texture* colorBuffer(size_t) const;
        const uivec2& resolution() const;
        bool isBuild() const;

        static const uint COLOR_BUFFER=0x1;
        void bind(uint flags=COLOR_BUFFER);
        static void unbind();

    private:
        struct Attachment
        {
            Texture* tex=nullptr;
            size_t attachment=0;
        };
        uint _id=0;
        uivec2 _size;
        boost::container::vector<Attachment> _colorBuffers;
        Attachment _depthBuffer;

        #include "MemoryLoggerOff.h"
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        #include "MemoryLoggerOn.h"

        void build();
    };

    inline Texture* FrameBuffer::depthBuffer() const { return _depthBuffer.tex; }
    inline Texture* FrameBuffer::colorBuffer(size_t index) const { if(index<_colorBuffers.size()) return _colorBuffers[index].tex; else return nullptr; }
    inline const uivec2& FrameBuffer::resolution() const { return _size; }
}
}
#include "MemoryLoggerOff.h"

#endif // FRAMEBUFFER_H
