#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "MeshManager.h"
#include "MetaShader.h"
#include "MaterialModel.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "core.h"
#include <tinyxml/tinyxml.h>

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{
    class ResourceManager
    {
    public:
        ResourceManager(ThreadPool*, TextureLoader*);
        virtual ~ResourceManager();

        std::string printUsedGpuMemory() const;

        void addMesh(const std::string&, renderer::MeshBuffers*);
        renderer::MeshBuffers* getMesh(const std::string&);
        renderer::MeshBuffers* async_getMesh(const std::string&);

        bool loadXmlDataInformation(const std::string&, const std::string& group="");

        const MeshManager::MeshContext& meshDataContext(const std::string&) const;

        void addShader(const std::string&, renderer::Shader*);
        renderer::Shader* getShader(const std::string&) const;

        renderer::Texture* getTexture(const std::string&);

        renderer::Material* getMaterial(const std::string&);
        renderer::Material* async_getMaterial(const std::string&);
        const MaterialModel* materialModel(const std::string&) const;

        renderer::MaterialPass* getMaterialPass(const std::string&);
        renderer::MaterialPass* async_getMaterialPass(const std::string&);

        void flush(float time);

    private:
        MeshManager _meshManager;
        TextureManager _textureManager;

        boost::container::map<std::string, MeshManager::MeshContext> _meshDataContext;
        boost::container::map<std::string, MaterialModel> _materialModels;

        boost::container::map<std::string, MaterialPassModel> _materialPassModels;
        boost::container::map<std::string, boost::shared_ptr<renderer::MaterialPass>> _materialPass;

        ThreadPool* _pool;

        boost::container::map<std::string, renderer::Shader*> _shaderLoaded;
        boost::container::map<uivec3, boost::shared_ptr<renderer::Shader>> _mapShader;
        MetaShader* _enginePixelShader;
        MetaShader* _engineVertexShader;
        MetaShader* _engineGeometryShader;

        MaterialManager _materials;

        boost::container::vector<std::pair<renderer::MeshBuffers*, boost::unique_future<renderer::MeshBuffers*>>> _curMeshLoading;

        static bool compare(const std::string&, const char*);
        static std::string str(const char*);

        template <size_t N>
        Vector<float, N> toVec(const std::string&);

        template <size_t N>
        Vector<std::string, N> toStrVec(const std::string&);

        void parseDataGroupNode(TiXmlElement*);
        void parseMaterial(TiXmlElement*, MaterialModel&);
        void parseMaterialPass(TiXmlElement*, MaterialPassModel&);
        void parseOptionalMaterial(TiXmlElement*, MaterialModel&);
    };

    inline const MeshManager::MeshContext& ResourceManager::meshDataContext(const std::string& name) const
    {
        static const MeshManager::MeshContext empy;
        auto it=_meshDataContext.find(name);
        if(it != _meshDataContext.end())
            return it->second;
        else return empy;
    }

    inline const MaterialModel* ResourceManager::materialModel(const std::string& str) const
    {
        auto it=_materialModels.find(str);
        if(it != _materialModels.end())
            return &it->second;
        else return nullptr;
    }

    inline void ResourceManager::addMesh(const std::string& name, renderer::MeshBuffers* mesh)
    {
        _meshManager.addMesh(name, mesh);
    }

    inline void ResourceManager::addShader(const std::string& str, renderer::Shader* s)
    {
        _shaderLoaded[str] = s;
    }

    inline renderer::Shader* ResourceManager::getShader(const std::string& name) const
    {
        auto it = _shaderLoaded.find(name);
        if(it != _shaderLoaded.end())
            return it->second;
        else return nullptr;
    }

    inline bool ResourceManager::compare(const std::string& str, const char* cstr)
    {
        if(!cstr) return false;
        else return str==std::string(cstr);
    }

    inline std::string ResourceManager::str(const char* str)
    {
        if(str) return std::string(str);
        else return std::string();
    }

    template <size_t N>
    Vector<float, N> ResourceManager::toVec(const std::string& str)
    {
        std::string tmp;
        Vector<float, N> res;
        size_t i=0;
        for(size_t index=0 ; index<N ; index++)
        {
            while(i<str.size() && str[i] != ',' && str[i] != ' ')
            {
                tmp += str[i];
                i++;
            }

            i++;
            res[index] = StringUtils(tmp).toFloat();
            tmp.clear();
        }

        return res;
    }

    template <size_t N>
    Vector<std::string, N> ResourceManager::toStrVec(const std::string& str)
    {
        Vector<std::string, N> res;
        size_t i=0;
        for(size_t index=0 ; index<N ; index++)
        {
            while(i<str.size() && str[i] != ',' && str[i] != ' ')
            {
                res[index] += str[i];
                i++;
            }

            i++;
        }

        return res;
    }
}
}
#include "MemoryLoggerOff.h"

#endif // RESOURCEMANAGER_H
