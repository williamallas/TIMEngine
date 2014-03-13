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

}
}
