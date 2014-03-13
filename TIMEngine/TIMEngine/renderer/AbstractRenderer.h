#ifndef ABSTRACTRENDERER_H
#define ABSTRACTRENDERER_H

#include "core/core.h"
#include "FrameBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

    class AbstractRenderer
    {
    public:
        AbstractRenderer(const uivec2&);
        AbstractRenderer(const boost::container::vector<Texture*>&, Texture* depthBuf=nullptr);
        virtual ~AbstractRenderer();

        void setAbsTime(float);
        void setFrameTime(float);

    private:
        FrameBuffer* _frameBuffer=nullptr;
        uivec2 _resolution;

        float _absTime=0, _frameTime=0;
    };

    inline void AbstractRenderer::setAbsTime(float t) { _absTime = t; }
    inline void AbstractRenderer::setFrameTime(float t) { _frameTime = t; }

}
}
#include "MemoryLoggerOff.h"

#endif // ABSTRACTRENDERER_H
