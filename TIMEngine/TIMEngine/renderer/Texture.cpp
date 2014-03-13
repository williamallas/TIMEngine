#include "Texture.h"
#include "TextureSampler.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

Texture::Texture()
{

}

Texture::~Texture()
{
    glDeleteTextures(1, &_id);
}

void Texture::setParameter(const Parameter& param) const
{
    openGL.bindTexture(_id, GL_TEXTURE_2D, 0);
    if(param.mipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        if(param.linear && param.trilinear)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_LINEAR);
        else if(!param.linear && param.trilinear)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST_MIPMAP_LINEAR);
        else if(param.linear && !param.trilinear)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                            GL_LINEAR_MIPMAP_NEAREST);
        else
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
                            GL_NEAREST_MIPMAP_NEAREST);
    }
    else
    {
        if(param.linear)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }

    if(param.repeat)
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if(param.linear)
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    if(openGL.hardward(GLState::Hardward::ANISOTROPY) && param.anisotropy)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, param.anisotropyFilter);

    if(param.depthMode)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    }
}

void Texture::setSampler(TextureSampler* sampler)
{
    _parameter = sampler;
}

Texture* Texture::genTexture2D(const uivec2& size, const float* data, uint nbComponent, const Parameter& param)
{
    return genTexture2D(GL_FLOAT, size, data, nbComponent, param);
}

Texture* Texture::genTexture2D(const uivec2& size, const unsigned char* data, uint nbComponent, const Parameter& param)
{
    return genTexture2D(GL_UNSIGNED_BYTE, size, data, nbComponent, param);
}

Texture* Texture::genTexture2D(uint dataType, const uivec2& size, const void* data, uint nbComponent, const Parameter& param)
{
    uint dataFormat=GL_RGB;
    if(data)
    {
        switch(nbComponent)
        {
            case 1: dataFormat=GL_RED; break;
            case 3: dataFormat=GL_RGB; break;
            case 4: dataFormat=GL_RGBA; break;
            default: return nullptr;
        }
    }

    uint idTex;
    glGenTextures(1, &idTex);
    openGL.bindTexture(idTex, GL_TEXTURE_2D, 0);

    size_t pixelByteSize=0;

    Format format;
    switch(param.intern)
    {
        case Parameter::InternFormat::RGB:
            format=Format::RGB;
            pixelByteSize = 3;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGBA:
            format=Format::RGBA;
            pixelByteSize = 4;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::R:
            format=Format::R;
            pixelByteSize=1;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGB8:
            format=Format::RGB;
            pixelByteSize=3;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGB16:
            format=Format::RGB;
            pixelByteSize=6;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGBA8:
            format=Format::RGBA;
            pixelByteSize=4;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGBA16:
            format=Format::RGBA;
            pixelByteSize=8;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::R8:
            format=Format::R;
            pixelByteSize=1;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::R16:
            format=Format::R;
            pixelByteSize=2;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::R16F:
            format=Format::R;
            pixelByteSize=2;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::R32F:
            format=Format::R;
            pixelByteSize=4;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGB16F:
            format=Format::RGB;
            pixelByteSize=6;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGB32F:
            format=Format::RGB;
            pixelByteSize=12;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGBA16F:
            format=Format::RGBA;
            pixelByteSize=8;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::RGBA32F:
            format=Format::RGBA;
            pixelByteSize=16;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x(), size.y(), 0, dataFormat, dataType, data);
            break;

        case Parameter::InternFormat::DEPTHCOMPONENT:
            format=Format::DEPTH;
            pixelByteSize=4;
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, size.x(), size.y(), 0, GL_DEPTH_COMPONENT, dataType, data);
            break;

    }

    Texture* t = new Texture;
    t->_id = idTex;
    t->_size = uivec3(size.x(),size.y(),0);
    t->_type = Type::TEXTURE_2D;
    t->_format = format;
    t->_pixelByteSize = pixelByteSize;

    if(!param.defaultSampler)
    {
        t->setParameter(param);
        if(param.mipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        if(param.repeat)
            t->setSampler(TextureSampler::defaultSamplerRepeat);
        else
            t->setSampler(TextureSampler::defaultSamplerNoRepeat);

        if(TextureSampler::defaultMipmap)
            glGenerateMipmap(GL_TEXTURE_2D);
    }


    return t;
}

}
}
