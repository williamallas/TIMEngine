#ifndef TRANSFORMABLE_H_INCLUDED
#define TRANSFORMABLE_H_INCLUDED

#include <boost/container/vector.hpp>
#include <algorithm>

#include "core.h"
#include "BoundingVolume.h"
#include "SceneContainer.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    class Transformable;
    class SceneManager;

    typedef SceneContainer<boost::container::vector<Transformable*> > TransformableContainer;

    class Transformable
    {
    public:
        const BoundingVolume& volume() const;

        /* Containers */
        Transformable& addContainer(TransformableContainer*);
        bool removeContainer(TransformableContainer*);

    private:
        BoundingVolume _volume;
        boost::container::vector<TransformableContainer*> _parentsContainer;
        SceneManager* _sceneManager;

    protected:
        Transformable(SceneManager* scene) : _sceneManager(scene) {}
        virtual ~Transformable() {}
    };

    inline const BoundingVolume& Transformable::volume() const { return _volume; }

    inline Transformable& Transformable::addContainer(TransformableContainer* container)
    {
        if(container)
            _parentsContainer.push_back(container);
        return *this;
    }

    inline bool Transformable::removeContainer(TransformableContainer* container)
    {
        auto it = std::find(_parentsContainer.begin(), _parentsContainer.end(), container);
        if(it != _parentsContainer.end())
        {
            _parentsContainer.erase(it);
            return true;
        }
        return false;
    }

}
}
#include "MemoryLoggerOff.h"


#endif // TRANSFORMABLE_H_INCLUDED
