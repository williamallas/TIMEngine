#include "FrameBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

FrameBuffer::FrameBuffer(const boost::container::vector<Texture*>& colorBuf, Texture* depthBuf)
{
    _depthBuffer.attachment=0;
    _depthBuffer.tex=depthBuf;

    _colorBuffers.resize(colorBuf.size());
    for(size_t i=0 ; i<colorBuf.size() ; i++)
    {
        _colorBuffers[i].attachment=0;
        _colorBuffers[i].tex=colorBuf[i];
    }

    build();
}

FrameBuffer::~FrameBuffer()
{
    if(_id!=0)
        glDeleteFramebuffers(1, &_id);

    delete _depthBuffer.tex;
    for(size_t i=0 ; i<_colorBuffers.size() ; i++)
    {
        delete _colorBuffers[i].tex;
    }
}

void FrameBuffer::build()
{
    if(_id!=0)
    {
        glDeleteFramebuffers(1, &_id);
        _id=0;
    }

    if(!_depthBuffer.tex && !_colorBuffers.size())
        return;

    glGenFramebuffers(1, &_id);
    openGL.bindFrameBuffer(_id);

    if(_depthBuffer.tex)
    {
        _size = _depthBuffer.tex->resolution();
        switch(_depthBuffer.tex->type())
        {
            case Texture::TEXTURE_2D:
            case Texture::TEXTURE_ARRAY:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthBuffer.tex->id(), _depthBuffer.attachment); break;
            case Texture::TEXTURE_3D:
                glFramebufferTexture3D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_3D, _depthBuffer.tex->id(), 0, _depthBuffer.attachment); break;
            case Texture::TEXTURE_CUBE:
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthBuffer.attachment, _depthBuffer.tex->id(), 0); break;
        }
    }
    for(size_t i=0 ; i<_colorBuffers.size() ; i++)
    {
        if(_colorBuffers[i].tex)
        {
            _size = _colorBuffers[i].tex->resolution();
            switch(_colorBuffers[i].tex->type())
            {
                case Texture::TEXTURE_2D:
                case Texture::TEXTURE_ARRAY:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, _colorBuffers[i].tex->id(), _colorBuffers[i].attachment); break;
                case Texture::TEXTURE_3D:
                    glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_3D, _colorBuffers[i].tex->id(), 0, _colorBuffers[i].attachment); break;
                case Texture::TEXTURE_CUBE:
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, _colorBuffers[i].attachment, _colorBuffers[i].tex->id(), 0); break;
            }
        }
    }
}

bool FrameBuffer::isBuild() const
{
    openGL.bindFrameBuffer(_id);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if(status != GL_FRAMEBUFFER_COMPLETE)
        return false;
    else return true;
}

void FrameBuffer::bind(uint flags)
{
    openGL.bindFrameBuffer(_id);
    openGL.setViewPort(uivec2(), _size);

    if(!(flags & COLOR_BUFFER))
    {
        openGL.drawBuffer(GL_NONE);
        openGL.readBuffer(GL_NONE);
    }
    else
    {
        GLenum * buffers = new GLenum[_colorBuffers.size()];

        for(size_t i = 0; i < _colorBuffers.size(); i++)
            buffers[i] = GL_COLOR_ATTACHMENT0 + i;

        glDrawBuffers(_colorBuffers.size(), buffers);
        delete[] buffers;
    }
}

void FrameBuffer::unbind()
{
    openGL.bindFrameBuffer(0);
    openGL.drawBuffer(GL_BACK);
    openGL.readBuffer(GL_BACK);
}

}
}
