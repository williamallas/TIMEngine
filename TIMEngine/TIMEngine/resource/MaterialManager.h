#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include "core/core.h"
#include "renderer/renderer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
    namespace renderer{ class Material; };
namespace resource
{

    class MaterialManager
    {
        friend class renderer::Material;

    public:
        MaterialManager();
        virtual ~MaterialManager();

        renderer::Material* add(renderer::Material*, const std::string& name="");
        renderer::Material* get(const std::string&) const;
        size_t updateIndex() const;

    private:
        struct comparator
        {
            bool operator()(renderer::Material* m1, renderer::Material* m2) const
            {
                if(*m1 == *m2) return m1<m2;
                else return (*m1)<(*m2);
            }
        };
        size_t _curMaterialId=0;
        mutable boost::mutex _mutex;
        boost::container::set<renderer::Material*, comparator> _materials;
        boost::container::map<std::string, renderer::Material*> _nameMaterial;

        void pop(renderer::Material*);
        void push(renderer::Material*);
    };

    inline renderer::Material* MaterialManager::add(renderer::Material* m, const std::string& name)
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        if(name.empty())
        {
            _curMaterialId++;
            m->_inManagerId=_curMaterialId;
            _materials.insert(m);
            m->_inManager=true;
        }
        else
        {
            auto it=_nameMaterial.find(name);
            if(it != _nameMaterial.end())
            {
                delete m;
                m=it->second;
            }
            else
            {
                _curMaterialId++;
                m->_inManagerId=_curMaterialId;
                _nameMaterial[name]=m;
                _materials.insert(m);
                m->_inManager=true;
            }
        }

        m->_manager=this;
        return m;
    }

    inline renderer::Material* MaterialManager::get(const std::string& name) const
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        auto it=_nameMaterial.find(name);
        if(it != _nameMaterial.end())
            return it->second;
        else
            return nullptr;
    }

    inline void MaterialManager::pop(renderer::Material* m) { boost::lock_guard<decltype(_mutex)> g(_mutex); _materials.erase(m); }
    inline void MaterialManager::push(renderer::Material* m) { boost::lock_guard<decltype(_mutex)> g(_mutex); _materials.insert(m); }

    inline size_t MaterialManager::updateIndex() const
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        size_t i=0;
        for(auto it : _materials)
        {
            it->_index=i;
            i++;
        }
        return i;
    }

}
}
#include "MemoryLoggerOff.h"

#endif // MATERIALMANAGER_H
