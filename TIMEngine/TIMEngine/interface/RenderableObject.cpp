#include "RenderableObject.h"
#include "SceneManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{

RenderableObject::RenderableObject(const mat4& mat, scene::SceneManager* sm) : SceneObject(mat, sm)
{
    _type = RENDERABLE;
}

RenderableObject::~RenderableObject()
{
    clearElement();
}

void RenderableObject::clearElement()
{
    for(auto& m : _mesh)
    {
        m.materialPass()->decrementReference();
    }
    _mesh.clear();
}

RenderableObject* RenderableObject::addElement(renderer::MaterialPass* pass)
{
    if(!pass)
        return this;

    _mesh.push_back(renderer::MaterialInstance(pass, &matrix()));

    pass->incrementReference();

    scene::BoundingVolume v;
    v.sphere = initialVolume().sphere.max(pass->sphere());
    if(initialVolume().obb)
    {
        v.obb = new OrientedBox();
        *(v.obb) = OrientedBox(initialVolume().box().box().max(pass->box()), mat4::IDENTITY());
    }

    setInitialVolume(v);

    if(pass->containsStreamingMesh())
    {
        _manager->notifyStreamMeshRenderable(this);
    }

    return this;
}

void RenderableObject::computeVolume(bool withObb)
{
    scene::BoundingVolume v;
    if(withObb)
        v.obb = new OrientedBox;
    for(size_t i=0 ; i<_mesh.size() ; ++i)
    {
        v.sphere = v.sphere.max(_mesh[i].materialPass()->sphere());
        if(withObb)
        {
            *(v.obb) = OrientedBox(v.box().box().max(_mesh[i].materialPass()->box()), mat4::IDENTITY());
        }
    }
    setInitialVolume(v);
}

bool RenderableObject::isStreaming() const
{
    for(size_t i=0 ; i<_mesh.size() ; ++i)
    {
        if(_mesh[i].materialPass()->containsStreamingMesh())
            return false;
    }
    return true;
}

}
}
