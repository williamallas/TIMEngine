#include "MaterialInstance.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{

MaterialInstance::MaterialInstance(renderer::MaterialPass* pass, const mat4* mat)
{
    _matrix=mat;
    _materials=pass;
}

MaterialInstance::~MaterialInstance()
{

}

}
}
