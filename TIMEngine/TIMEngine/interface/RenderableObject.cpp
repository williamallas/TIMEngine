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
    //dtor
}

RenderableObject* RenderableObject::addElement(renderer::MaterialPass* pass)
{
    _mesh.push_back(renderer::MaterialInstance(pass, &matrix()));

    scene::BoundingVolume v;
    v.sphere = initialVolume().sphere.max(pass->sphere());
    if(initialVolume().obb)
    {
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
    for(size_t i=0 ; i<_mesh.size() ; i++)
    {
        v.sphere = v.sphere.max(_mesh[i].materialPass()->sphere());
        if(withObb)
        {
            *(v.obb) = OrientedBox(v.box().box().max(_mesh[i].materialPass()->box()), mat4::IDENTITY());
        }
    }
    setInitialVolume(v);
}

bool RenderableObject::isComplete() const
{
    for(size_t i=0 ; i<_mesh.size() ; i++)
    {
        if(_mesh[i].materialPass()->containsStreamingMesh())
            return false;
    }
    return true;
}

}
}
