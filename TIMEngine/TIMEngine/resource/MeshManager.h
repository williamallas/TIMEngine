#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include "renderer/MeshBuffers.h"
#include "MeshLoader.h"
#include "pathdata.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace resource
{

    class MeshManager
    {
    public:

        struct MeshContext
        {
            enum // format
            { OBJ, TIM };

            std::string path;
            bool tangent=false;
            int format=OBJ;
        };

        static boost::unique_future<renderer::MeshBuffers*> async_loadMesh(const MeshContext&, ThreadPool&);

        MeshManager();
        virtual ~MeshManager();

        size_t onGpuMeshMemory() const;
        void addUsedMeshGpuMemory(size_t);

        renderer::MeshBuffers* loadMesh(const std::string&, const MeshContext&);
        void addMesh(const std::string&, renderer::MeshBuffers*);
        size_t loadPackage(boost::container::map<std::string, MeshContext>&, bool, size_t);

        renderer::MeshBuffers* get(const std::string&) const;

    private:
        boost::container::map<std::string, boost::shared_ptr<renderer::MeshBuffers>> _mesh;
        size_t _gpuMemory=0;
    };

    inline size_t MeshManager::onGpuMeshMemory() const { return _gpuMemory; }
    inline void MeshManager::addUsedMeshGpuMemory(size_t s) { _gpuMemory += s; }

}
}
#include "MemoryLoggerOff.h"

#endif // MESHMANAGER_H
