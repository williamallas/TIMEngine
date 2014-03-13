#include "TextureManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

TextureManager::TextureManager(TextureLoader* loader)
{
    _loader=loader;
}

TextureManager::~TextureManager()
{
    delete _loader;
}

void TextureManager::addModel(const std::string& name, const TextureModel& model)
{
    _models[name] = model;
}

renderer::Texture* TextureManager::getTexture(const std::string& name)
{
    auto it = _loadedTexture.find(name);
    if(it != _loadedTexture.end())
        return it->second.get();
    else
    {
        auto model = _models.find(name);
        if(model == _models.end())
            return nullptr;

        TextureLoader::ImageFormat format;
        boost::shared_ptr<unsigned char> tex_data(
                         TextureLoader::toGLFormat(_loader->loadImage(model->second.data, format),
                                                   nullptr, format),
                         [](unsigned char* ptr){delete[] ptr;} );

        if(tex_data.get())
        {
            renderer::Texture* tex = renderer::Texture::genTexture2D(format.size, tex_data.get(), format.nbComponent, model->second.parameter);
            if(tex)
            {
                _loadedTexture[name] = boost::shared_ptr<renderer::Texture>(tex);
                _gpuMemory += tex->resolution().x()*tex->resolution().y()*tex->bytePerPixel();
                return tex;
            }
        }
    }

    return nullptr;
}

renderer::Texture* TextureManager::async_getTexture(const std::string& name, ThreadPool& pool)
{
    auto it = _loadedTexture.find(name);
    if(it != _loadedTexture.end())
        return it->second.get();
    else
    {
        auto model = _models.find(name);
        if(model == _models.end())
            return nullptr;

        renderer::Texture* tex = new renderer::Texture;
        _curTextureLoading.push_back(std::pair<renderer::Texture*, boost::unique_future<TextureFuture>>(tex,
                                     pool.schedule_trace([=](){ TextureFuture f;
                                                                f.model = model->second.parameter;
                                                                f.data = _loader->loadImage(model->second.data, f.format);
                                                                return f; }))
                                    );
        _loadedTexture[name] = boost::shared_ptr<renderer::Texture>(tex);
        return tex;
    }

    return nullptr;
}

void TextureManager::flush(ThreadPool& pool)
{
    boost::container::vector<std::pair<KeyTextureUploading, boost::unique_future<bool>>> cpy2;
    for(size_t i=0 ; i<_curTextureUploading.size() ; i++)
    {
        if(_curTextureUploading[i].second.get_state() == boost::future_state::state::ready)
        {
            KeyTextureUploading& f = _curTextureUploading[i].first;
            f.pbo->endUpload();
            renderer::Texture* tex = renderer::Texture::genTexture2D(f.parameter.format.size,
                                                                     (unsigned char*)nullptr,
                                                                     f.parameter.format.nbComponent,
                                                                     f.parameter.model);

            _gpuMemory += tex->resolution().x()*tex->resolution().y()*tex->bytePerPixel();
            f.tex->swap(*tex);
            delete tex;
            renderer::PixelBuffer::unbind();
        }
        else
        {
            cpy2.push_back(std::pair<KeyTextureUploading, boost::unique_future<bool>>
                          (_curTextureUploading[i].first, boost::move(_curTextureUploading[i].second)));
        }
    }
    _curTextureUploading = boost::move(cpy2);


    boost::container::vector<std::pair<renderer::Texture*, boost::unique_future<TextureFuture>>> cpy;
    for(size_t i=0 ; i<_curTextureLoading.size() ; i++)
    {
        if(_curTextureLoading[i].second.get_state() == boost::future_state::state::ready)
        {
            const TextureFuture& f = _curTextureLoading[i].second.get();

            renderer::PixelBuffer* pbo = new renderer::PixelBuffer;
            unsigned char* data = (unsigned char*)pbo->allocSpace(f.format.size.x()*f.format.size.y()*f.format.nbComponent);
            if(!data)
            {
                delete[] f.data;
                continue;
            }

            _curTextureUploading.push_back(std::pair<KeyTextureUploading, boost::unique_future<bool>>(
                                           {f, pbo, _curTextureLoading[i].first},
                                           pool.schedule_trace([=]() {
                                                TextureLoader::toGLFormat(f.data, data, f.format);
                                                return true;
                                            })));
        }
        else
        {
            cpy.push_back(std::pair<renderer::Texture*, boost::unique_future<TextureFuture>>
                         (_curTextureLoading[i].first, boost::move(_curTextureLoading[i].second)));
        }
    }
    _curTextureLoading = boost::move(cpy);
}

}
}
