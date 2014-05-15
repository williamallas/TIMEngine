#include "MaterialManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{
    for(auto* m : _materials)
    {
        delete m;
    }
}

renderer::Material* MaterialManager::add(renderer::Material* m, const std::string& name)
{
    boost::lock_guard<decltype(_mutex)> g(_mutex);
    if(name.empty())
    {
        ++_curMaterialId;
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
            ++_curMaterialId;
            m->_inManagerId=_curMaterialId;
            _nameMaterial[name]=m;
            _materials.insert(m);
            m->_inManager=true;
        }
    }

    m->_manager=this;
    return m;
}

renderer::Material* MaterialManager::get(const std::string& name) const
{
    boost::lock_guard<decltype(_mutex)> g(_mutex);
    auto it=_nameMaterial.find(name);
    if(it != _nameMaterial.end())
        return it->second;
    else
        return nullptr;
}

bool MaterialManager::remove(renderer::Material* mat)
{
    boost::lock_guard<decltype(_mutex)> g(_mutex);

    for(auto it=_nameMaterial.begin() ; it != _nameMaterial.end() ; it++)
    {
        if(mat == it->second)
        {
            _nameMaterial.erase(it);
            _materials.erase(mat);
            delete mat;
            return true;
        }
    }
    return false;
}

}
}
