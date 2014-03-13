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
    class SceneGrid;

    typedef SceneContainer<boost::container::vector<Transformable*> > TransformableContainer;

    class Transformable
    {
        friend class SceneManager;
    #ifdef USE_OCTREE
        friend class OctreeNode;
    #else
        friend class SceneGrid;
    #endif

    public:
        const BoundingVolume& volume() const;
        const vec3& volumeCenter() const;
        SceneManager* sceneManager() const;

        void move();

    private:
    #ifdef USE_OCTREE
        TransformableContainer* _lowestCommonParent;
        boost::container::vector<TransformableContainer*> _parentsContainer;

        /*Containers */
        void clearContainer();
        const boost::container::vector<TransformableContainer*>& container() const { return _parentsContainer; }
        Transformable& addContainer(TransformableContainer*);
        bool removeContainer(TransformableContainer*);
    #else
        struct DataContainer
        {
            void* container;
            size_t index;
        };
        boost::container::vector<DataContainer> _cells;
        boost::container::vector<DataContainer> _grids;

        boost::container::vector<DataContainer>& cells();
        boost::container::vector<DataContainer>& grids();
    #endif

        SceneManager* _sceneManager;

    #ifdef USE_OCTREE
        void setLowestCommonParent(TransformableContainer*);
        TransformableContainer* lowestCommonParent() const;
    #endif

    protected:
        BoundingVolume _volume;

    #ifdef USE_OCTREE
        Transformable(SceneManager* scene) : _sceneManager(scene), _lowestCommonParent(nullptr) {}
    #else
        Transformable(SceneManager* scene) : _sceneManager(scene) {}
    #endif

        virtual ~Transformable();
        void deleteFromSceneManager();
    };


    /** Inline implementation */

    inline const BoundingVolume& Transformable::volume() const { return _volume; }
    inline const vec3& Transformable::volumeCenter() const { return _volume.sphere.center(); }
    inline SceneManager* Transformable::sceneManager() const { return _sceneManager; }

#ifdef USE_OCTREE
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
    {
        _parentsContainer.clear();
    }

    inline void Transformable::setLowestCommonParent(TransformableContainer* c)
    { _lowestCommonParent = c; }

    inline TransformableContainer* Transformable::lowestCommonParent() const
    { return _lowestCommonParent; }
#else
    inline boost::container::vector<Transformable::DataContainer>& Transformable::cells() { return _cells; }
    inline boost::container::vector<Transformable::DataContainer>& Transformable::grids() { return _grids; }
#endif

}
}
#include "MemoryLoggerOff.h"


#endif // TRANSFORMABLE_H_INCLUDED
