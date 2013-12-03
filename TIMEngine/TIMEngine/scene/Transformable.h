#ifndef TRANSFORMABLE_H_INCLUDED
#define TRANSFORMABLE_H_INCLUDED

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
        friend class SceneManager;

    public:
        virtual ~Transformable();

        const BoundingVolume& volume() const;
        const vec3& volumeCenter() const;

        /* Containers */
        Transformable& addContainer(TransformableContainer*);
        bool removeContainer(TransformableContainer*);

    private:
        boost::container::vector<TransformableContainer*> _parentsContainer;
        boost::recursive_mutex _mutexParentsContainer; // protect _parentsContainer

        SceneManager* _sceneManager;

    protected:
        BoundingVolume _volume;

        Transformable(SceneManager* scene) : _sceneManager(scene) {}
    };


    /** Inline implementation */
    inline const BoundingVolume& Transformable::volume() const { return _volume; }
    inline const vec3& Transformable::volumeCenter() const { return _volume.sphere.center(); }

    inline Transformable& Transformable::addContainer(TransformableContainer* container)
    {
        boost::lock_guard<decltype(_mutexParentsContainer)> guard(_mutexParentsContainer);
        if(container)
            _parentsContainer.push_back(container);

        return *this;
    }

    inline bool Transformable::removeContainer(TransformableContainer* container)
    {
        boost::lock_guard<decltype(_mutexParentsContainer)> guard(_mutexParentsContainer);

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
