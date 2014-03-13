#ifndef MATERIALPASS_H_INCLUDED
#define MATERIALPASS_H_INCLUDED

#include "core/core.h"
#include "Material.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    class MaterialPass
    {
    public:
        static const size_t COLOR_PASS = 0;
        static const size_t DEPTH_PASS = 1;
        static const size_t CUSTOM_PASS = 2;

        MaterialPass() {}
        virtual ~MaterialPass() {}

        void setPass(Material*, size_t);
        Material* getPass(size_t) const;

        Sphere sphere() const;
        Box box() const;

        bool containsStreamingMesh() const;

    private:
        boost::container::vector<Material*> _pass;
    };

    inline void MaterialPass::setPass(Material* m, size_t index)
    {
        if(index >= _pass.size())
        {
            _pass.resize(index+1);
        }
        _pass[index] = m;
    }

    inline Material* MaterialPass::getPass(size_t  index) const
    {
        if(index < _pass.size())
            return _pass[index];
        else return nullptr;
    }

    inline Sphere MaterialPass::sphere() const
    {
        for(size_t i=0 ; i<3 ; i++)
        {
            Material* pass = getPass(i);
            if(pass && pass->mesh())
            {
                if(pass->mesh()->vertexBuffer())
                    return pass->mesh()->vertexBuffer()->sphere();
            }
        }
        return Sphere(vec3(), 1);
    }

    inline Box MaterialPass::box() const
    {
        for(size_t i=0 ; i<3 ; i++)
        {
            Material* pass = getPass(i);
            if(pass && pass->mesh())
            {
                return pass->mesh()->vertexBuffer()->box();
            }
        }
        return Box();
    }

    inline bool MaterialPass::containsStreamingMesh() const
    {
        for(size_t i=0 ; i<3 ; i++)
        {
            Material* pass = getPass(i);
            if(pass && pass->mesh() && !pass->mesh()->isComplete())
                return true;
        }
        return false;
    }
}
}
#include "MemoryLoggerOff.h"


#endif // MATERIALPASS_H_INCLUDED
