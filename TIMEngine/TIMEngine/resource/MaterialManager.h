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
        bool remove(renderer::Material*);

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

    inline void MaterialManager::pop(renderer::Material* m) { boost::lock_guard<decltype(_mutex)> g(_mutex); _materials.erase(m); }
    inline void MaterialManager::push(renderer::Material* m) { boost::lock_guard<decltype(_mutex)> g(_mutex); _materials.insert(m); }

    inline size_t MaterialManager::updateIndex() const
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        size_t i=0;
        for(auto it : _materials)
        {
            it->_index=i;
            ++i;
        }
        return i;
    }

}
}
#include "MemoryLoggerOff.h"

#endif // MATERIALMANAGER_H
