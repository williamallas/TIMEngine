#include "renderer.h"

#include "GL/glew.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

bool init()
{
    GLenum glewerr;
    if((glewerr=glewInit()) != GLEW_OK)
    {
        err("glewinit() failed : ")<<glewGetErrorString(glewerr)<<"\n";
        return false;
    }
    glGetError();

    out("Initializing on ")<<glGetString(GL_VENDOR)<<" "<<glGetString(GL_RENDERER)<<" using OpenGL "<<glGetString(GL_VERSION)<<"\n";
    err("Initializing on ")<<glGetString(GL_VENDOR)<<" "<<glGetString(GL_RENDERER)<<" using OpenGL "<<glGetString(GL_VERSION)<<"\n";

    if(!glewIsSupported("GL_EXT_framebuffer_object"))
        err("GL_EXT_framebuffer_object not supported\n");
    if(!glewIsSupported("GL_ARB_framebuffer_object"))
        err("GL_ARB_framebuffer_object not supported\n");

    glClearDepth(1);
    openGL.clearDepth();
    openGL.getHardwardProperties();
    openGL.applyAll();

    TextureSampler::defaultSamplerRepeat = new TextureSampler;
    TextureSampler::defaultSamplerNoRepeat = new TextureSampler;

    Texture::Parameter param;
    param.anisotropy = false;
    param.anisotropyFilter = 0;
    param.depthMode = false;

    param.linear = true;
    param.mipmap = true;
    param.trilinear = true;

    TextureSampler::defaultMipmap = param.mipmap;

    param.repeat = true;
    TextureSampler::defaultSamplerRepeat->set(param);

    param.repeat = false;
    TextureSampler::defaultSamplerNoRepeat->set(param);

    if(openGL.hardward(GLState::Hardward::MAJOR_VERSION) > 3 ||
       (openGL.hardward(GLState::Hardward::MAJOR_VERSION)==3 && openGL.hardward(GLState::Hardward::MINOR_VERSION)>=3))
        return true;
    else
    {
        err("You don't have a sufficient openGL version.");
        return false;
    }

}

bool close()
{
    delete TextureSampler::defaultSamplerRepeat;
    delete TextureSampler::defaultSamplerNoRepeat;
    GLState::freeInstance();
}

}
}
