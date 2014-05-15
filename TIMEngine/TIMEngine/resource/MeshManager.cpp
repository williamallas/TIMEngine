#include "MeshManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{

}

renderer::MeshBuffers* MeshManager::loadMesh(const std::string& name, const MeshContext& context)
{
    renderer::MeshBuffers* mesh=nullptr;
    switch(context.format)
    {
        case MeshContext::OBJ:
            mesh=MeshLoader::importObj(PATH_DATA+context.path, context.tangent); break;
        case MeshContext::TIM:
            mesh=MeshLoader::importMeshTim(PATH_DATA+context.path); break;
        default:
            mesh=nullptr;
    }
    if(mesh)
    {
        _mesh[name] = boost::shared_ptr<renderer::MeshBuffers>(mesh);

        if(mesh->vertexBuffer())
            _gpuMemory += mesh->vertexBuffer()->size()*mesh->vertexBuffer()->formatSize()*4;
        if(mesh->indexBuffer())
            _gpuMemory += mesh->indexBuffer()->size()*4;

        mesh->vertexBuffer()->uploadOnGpu();
        mesh->indexBuffer()->uploadOnGpu();
    }

    return mesh;
}

void MeshManager::addMesh(const std::string& name, renderer::MeshBuffers* mesh)
{
    _mesh[name] = boost::shared_ptr<renderer::MeshBuffers>(mesh);
    if(mesh->vertexBuffer())
        _gpuMemory += mesh->vertexBuffer()->size()*mesh->vertexBuffer()->formatSize()*4;
    if(mesh->indexBuffer())
        _gpuMemory += mesh->indexBuffer()->size()*4;
}

size_t MeshManager::loadPackage(boost::container::map<std::string, MeshContext>& pack, bool exclusive, size_t nbThread)
{
    size_t ret=0;
    boost::container::vector<std::string> toDel;
    for(auto m : pack)
    {
        if(_mesh.find(m.first) != _mesh.end())
            toDel.push_back(m.first);
    }
    for(size_t i=0 ; i<toDel.size() ; ++i)
        pack.erase(toDel[i]);

    if(exclusive)
    {
        toDel.clear();
        for(auto& m : _mesh)
        {
            if(pack.find(m.first) != pack.end())
                toDel.push_back(m.first);
        }
        for(size_t i=0 ; i<toDel.size() ; ++i)
            _mesh.erase(toDel[i]);
    }

    if(nbThread>0)
    {
        boost::container::vector<boost::unique_future<renderer::MeshBuffers*>> futures;
        boost::container::vector<std::string> futuresName;

        ThreadPool pool(nbThread);

        for(auto it : pack)
        {
            futures.push_back(async_loadMesh(it.second, pool));
            futuresName.push_back(it.first);
        }

        for(size_t i=0 ; i<futures.size() ; ++i)
        {
            renderer::MeshBuffers* mesh = futures[i].get();
            if(mesh)
            {
                _mesh[futuresName[i]] = boost::shared_ptr<renderer::MeshBuffers>(mesh);
                mesh->vertexBuffer()->uploadOnGpu();
                mesh->indexBuffer()->uploadOnGpu();
                ++ret;
            }
        }
    }
    else
    {
        for(auto m : pack)
        {
            if(loadMesh(m.first, m.second))
                ++ret;
        }
    }
    return ret;
}

renderer::MeshBuffers* MeshManager::get(const std::string& name) const
{
    auto it=_mesh.find(name);
    if(it != _mesh.end())
        return it->second.get();
    else return nullptr;
}

boost::unique_future<renderer::MeshBuffers*> MeshManager::async_loadMesh(const MeshContext& context, ThreadPool& pool)
{
    switch(context.format)
    {
        case MeshContext::OBJ:
            return pool.schedule_trace([&](){return MeshLoader::importObj(PATH_DATA+context.path, context.tangent);});
        case MeshContext::TIM:
            return pool.schedule_trace([&](){return MeshLoader::importMeshTim(PATH_DATA+context.path);});
    }
    boost::promise<renderer::MeshBuffers*> p;
    p.set_value(nullptr);
    return p.get_future();
}

bool MeshManager::remove(renderer::MeshBuffers* mesh)
{
    for(auto it=_mesh.begin() ; it != _mesh.end() ; it++)
    {
        if(mesh == (it->second).get())
        {
            if(mesh->vertexBuffer())
                _gpuMemory -= mesh->vertexBuffer()->size()*mesh->vertexBuffer()->formatSize()*4;
            if(mesh->indexBuffer())
                _gpuMemory -= mesh->indexBuffer()->size()*4;

            _mesh.erase(it);
            return true;
        }
    }
    return false;
}

}
}
