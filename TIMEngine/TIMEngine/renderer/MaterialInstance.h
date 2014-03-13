#ifndef MATERIALINSTANCE_H_INCLUDED
#define MATERIALINSTANCE_H_INCLUDED

#include "core/core.h"

#include "MaterialPass.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class MaterialInstance
    {
    public:
        MaterialInstance(MaterialPass*, const mat4*model=nullptr);
        virtual ~MaterialInstance();

        const mat4* matrix() const;
        Material* material(size_t pass) const;
        MaterialPass* materialPass() const;

        bool isSmaller(const MaterialInstance&, size_t) const;

    private:
        const mat4* _matrix;
        MaterialPass* _materials;
    };

    inline const mat4* MaterialInstance::matrix() const { return _matrix; }
    inline Material* MaterialInstance::material(size_t pass) const { return _materials->getPass(pass); }
    inline MaterialPass* MaterialInstance::materialPass() const { return _materials; }

    inline bool MaterialInstance::isSmaller(const MaterialInstance& m, size_t pass) const { return (*_materials->getPass(pass))<(*(m._materials->getPass(pass))); }
}
}
#include "MemoryLoggerOff.h"

#endif // MATERIALINSTANCE_H_INCLUDED
