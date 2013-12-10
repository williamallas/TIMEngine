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
        friend class OctreeNode;
        friend class ArraySceneContainer;

    public:
        const BoundingVolume& volume() const;
        const vec3& volumeCenter() const;
        SceneManager* sceneManager() const;

        void lock() const;
        void unlock() const;

        void move();

    private:
        boost::container::vector<TransformableContainer*> _parentsContainer;
        TransformableContainer* _lowestCommonParent;
        mutable boost::mutex _mutex; // protect _parentsContainer

        SceneManager* _sceneManager;

        /* Containers */
        public:Transformable& addContainer(TransformableContainer*);
        bool removeContainer(TransformableContainer*);
        void clearContainer();
        const boost::container::vector<TransformableContainer*>& container() const { return _parentsContainer; }
        void setLowestCommonParent(TransformableContainer*);
        TransformableContainer* lowestCommonParent() const;

    protected:
        BoundingVolume _volume;

        Transformable(SceneManager* scene) : _sceneManager(scene), _lowestCommonParent(nullptr) {}
        virtual ~Transformable();
    };


    /** Inline implementation */
    inline void Transformable::lock() const { _mutex.lock(); }
    inline void Transformable::unlock() const { _mutex.unlock(); }

    inline const BoundingVolume& Transformable::volume() const { return _volume; }
    inline const vec3& Transformable::volumeCenter() const { return _volume.sphere.center(); }
    inline SceneManager* Transformable::sceneManager() const { return _sceneManager; }

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

    inline void Transformable::clearContainer()
    { _parentsContainer.clear(); }

    inline void Transformable::setLowestCommonParent(TransformableContainer* c)
    { _lowestCommonParent = c; }

    inline TransformableContainer* Transformable::lowestCommonParent() const
    { return _lowestCommonParent; }

}
}
#include "MemoryLoggerOff.h"


#endif // TRANSFORMABLE_H_INCLUDED
