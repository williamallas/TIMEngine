#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "core.h"
#include "renderer/Texture.h"
#include "TextureLoader.h"
#include "renderer/PixelBuffer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

    class TextureManager
    {
    public:
        struct TextureModel
        {
            renderer::Texture::Parameter parameter;
            std::string data;
        };

        TextureManager(TextureLoader*);
        virtual ~TextureManager();

        size_t onGpuTextureMemory() const;

        void addModel(const std::string&, const TextureModel&);

        renderer::Texture* getTexture(const std::string&);
        renderer::Texture* async_getTexture(const std::string&, ThreadPool&);

        void flush(ThreadPool&);

    private:
        TextureLoader* _loader;
        boost::container::map<std::string, TextureModel> _models;
        boost::container::map<std::string, boost::shared_ptr<renderer::Texture>> _loadedTexture;
        size_t _gpuMemory=0;

        struct TextureFuture
        {
            unsigned char* data;
            renderer::Texture::Parameter model;
            TextureLoader::ImageFormat format;
        };

        struct KeyTextureUploading
        {
            TextureFuture parameter;
            renderer::PixelBuffer* pbo;
            renderer::Texture* tex;
        };

        boost::container::vector<std::pair<renderer::Texture*, boost::unique_future<TextureFuture>>> _curTextureLoading;
        boost::container::vector<std::pair<KeyTextureUploading, boost::unique_future<bool>>> _curTextureUploading;

    };

    inline size_t TextureManager::onGpuTextureMemory() const { return _gpuMemory; }
}
}
#include "MemoryLoggerOff.h"

#endif // TEXTUREMANAGER_H
