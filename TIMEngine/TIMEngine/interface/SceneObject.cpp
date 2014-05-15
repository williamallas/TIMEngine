#include "SceneObject.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{

SceneObject::SceneObject(const mat4& m, scene::SceneManager* sm) : Transformable(sm)
{
    _type=ObjectType::SCENE;
    _initialVolume.sphere=Sphere({0,0,0}, 1);
    _volume=_initialVolume;
    _volume.sphere.transform(m);
    _matrix=m;
}

SceneObject::~SceneObject()
{
    if(_removeCallback)
    {
        _removeCallback(this);
    }
}

void SceneObject::setInitialVolume(const scene::BoundingVolume& v)
{
    _initialVolume=v;
    _volume=v;
    _volume.sphere.transform(_matrix);
    if(_initialVolume.obb)
    {
        _volume.obb->setMatrix(_volume.obb->matrix()*_matrix);
    }
    move();
}

void SceneObject::setMatrix(const mat4& m)
{
    _matrix=m;

    _volume=_initialVolume;
    _volume.sphere.transform(_matrix);
    if(_initialVolume.obb)
    {
        _volume.obb->setMatrix(_volume.obb->matrix()*_matrix);
    }
}

void SceneObject::setMatrixNoScale(const mat4& m)
{
    _matrix=m;

    _volume=_initialVolume;
    _volume.sphere.setCenter(_matrix*_initialVolume.sphere.center());
    if(_initialVolume.obb)
    {
        _volume.obb->setMatrix(_volume.obb->matrix()*_matrix);
    }
}

void SceneObject::remove()
{
    deleteFromSceneManager();
}

}
}
