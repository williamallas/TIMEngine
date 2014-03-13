#include "AbstractRenderer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

AbstractRenderer::AbstractRenderer(const uivec2& res)
{
    _resolution=res;
}

AbstractRenderer::AbstractRenderer(const boost::container::vector<Texture*>& colorBuf, Texture* depthBuf)
{
    _frameBuffer = new FrameBuffer(colorBuf, depthBuf);
}

AbstractRenderer::~AbstractRenderer()
{
    delete _frameBuffer;
}

}
}
