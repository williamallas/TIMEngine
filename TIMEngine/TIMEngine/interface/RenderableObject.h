#ifndef RENDERABLEOBJECT_H
#define RENDERABLEOBJECT_H

#include "SceneObject.h"
#include "renderer\MaterialInstance.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{
    class RenderableObject : public SceneObject
    {
        friend class SceneManager;

    public:
        RenderableObject* addElement(renderer::MaterialPass*);
        void clearElement();

        void computeVolume(bool withObb=false);

        renderer::MaterialPass* element(size_t) const;
        size_t nbElement() const;

    private:
        boost::container::vector<renderer::MaterialInstance> _mesh;
        bool isStreaming() const;

    protected:
        RenderableObject(const mat4&, scene::SceneManager*);
        virtual ~RenderableObject();
    };

    inline renderer::MaterialPass* RenderableObject::element(size_t i) const { if(i<_mesh.size()) return _mesh[i].materialPass(); else return nullptr; }
    inline size_t RenderableObject::nbElement() const { return _mesh.size(); }
}
}
#include "MemoryLoggerOff.h"

#endif // RENDERABLEOBJECT_H
