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

        virtual ~ArraySceneContainer()
        {
            for(auto it : _container)
            {
                it->setLowestCommonParent(nullptr);
                it->_sceneManager=nullptr;
            }
        }

        Intersection insert(Transformable*);
        bool remove(Transformable*);

    private:
        boost::mutex _mutex;
    };


    /** Inline implementation */
    inline Intersection ArraySceneContainer::insert(Transformable* obj)
    {
        boost::lock_guard<decltype(_mutex)> guard(_mutex);

        Intersection inter;
        if(obj->volume().obb)
            inter = _box.collide(obj->volume().box());
        else
            inter = _box.collide(obj->volume().sphere);

        if(inter != OUTSIDE)
        {
            _container.push_back(obj);
            obj->setLowestCommonParent(this);
        }

        return inter;
    }

    inline bool ArraySceneContainer::remove(Transformable* obj)
    {
        boost::lock_guard<decltype(_mutex)> guard(_mutex);

        auto it = std::find(_container.begin(), _container.end(), obj);
        if(it == _container.end())
            return false;

        obj->setLowestCommonParent(nullptr);
        _container.erase(it);
        return true;
    }
}
}
#include "MemoryLoggerOff.h"

#endif // ARRAYSCENECONTAINER_H_INCLUDED
