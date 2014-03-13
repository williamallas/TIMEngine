#include "TextureSampler.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace renderer
{

TextureSampler* TextureSampler::defaultSamplerRepeat = nullptr;
TextureSampler* TextureSampler::defaultSamplerNoRepeat = nullptr;
bool TextureSampler::defaultMipmap = true;

void TextureSampler::set(const Texture::Parameter& param)
{
    if(param.mipmap)
    {
        if(param.linear && param.trilinear)
            glSamplerParameteri(_id,GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
        else if(!param.linear && param.trilinear)
            glSamplerParameteri(_id,GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST_MIPMAP_LINEAR);
        else if(param.linear && !param.trilinear)
            glSamplerParameteri(_id,GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_NEAREST);
        else
            glSamplerParameteri(_id,GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST_MIPMAP_NEAREST);
    }
    else
    {
        if(param.linear)
            glSamplerParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        else
            glSamplerParameteri(_id,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }

    if(param.repeat)
    {
        glSamplerParameterf(_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameterf(_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glSamplerParameterf(_id, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glSamplerParameterf(_id, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if(param.linear)
        glSamplerParameteri(_id,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    else
        glSamplerParameteri(_id,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    if(openGL.hardward(GLState::Hardward::ANISOTROPY) && param.anisotropy)
        glSamplerParameterf(_id, GL_TEXTURE_MAX_ANISOTROPY_EXT, param.anisotropyFilter);

    if(param.depthMode)
    {
        glSamplerParameteri(_id, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glSamplerParameteri(_id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glSamplerParameteri(_id, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
}

}
}
