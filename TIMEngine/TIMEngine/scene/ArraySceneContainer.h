#ifndef ARRAYSCENECONTAINER_H_INCLUDED
#define ARRAYSCENECONTAINER_H_INCLUDED

#include "core.h"
#include "Transformable.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    class ArraySceneContainer : public TransformableContainer
    {
    public:
        ArraySceneContainer(const Box& box) : TransformableContainer() { _box=box; }
        virtual ~ArraySceneContainer() {}

        Intersection insert(Transformable*);
        bool remove(Transformable*);
    };

    inline Intersection ArraySceneContainer::insert(Transformable* obj)
    {
        Intersection inter;
        if(obj->volume().accurate)
            inter = _box.collide(obj->volume().obb);
        else
            inter = _box.collide(obj->volume().sphere);

        if(inter != OUTSIDE)
            _container.push_back(obj);

        return inter;
    }

    inline bool ArraySceneContainer::remove(Transformable* obj)
    {
        auto it = std::find(_container.begin(), _container.end(), obj);
        if(it == _container.end())
            return false;

        _container.erase(it);
        return true;
    }
}
}
#include "MemoryLoggerOff.h"

#endif // ARRAYSCENECONTAINER_H_INCLUDED
