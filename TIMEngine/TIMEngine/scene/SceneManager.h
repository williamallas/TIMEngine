#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "core.h"

#ifdef USE_OCTREE
#include "OctreeNode.h"
#else
#include "SceneGrid.h"
#endif

#include "Transformable.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace scene
{
    class SceneManager
    {
    public:

        static ivec3 subSceneIndex(const vec3&);

        SceneManager();
        virtual ~SceneManager();

        template<class Factory>
        Transformable* addTransformable(Factory&);

        void move(Transformable*);
        bool removeTransformable(Transformable*);

        void clearScene(); // async
        int flush(); // async

        std::string str() const;

#ifdef USE_OCTREE
        void referenceModifiedNode(OctreeNode*);
        void deferenceModifiedNode(OctreeNode*);
#endif

        template<class T>
        void take(T&, size_t depthLimit=0xFFFFFFFF);

        template<class T>
        void take_brute(T&);

    private:
        struct SubScene
        {
#ifdef USE_OCTREE
            SubScene(const vec3& center, SceneManager* scene) : tree(center, scene)  {}
            OctreeNode tree;
#else
            SubScene(const vec3& center, SceneManager* scene) : tree(center, scene) {}
            SceneGrid tree;
#endif

            boost::container::map<ivec3, SubScene*> neighbors;
        };

        boost::container::map<ivec3, SubScene*> _subScene;
        mutable boost::mutex _mutexSubScene; // protect _subScene

#ifdef USE_OCTREE
        boost::container::set<OctreeNode*> _modifiedNodes;
        mutable boost::mutex _mutexModifiedNodes; // protect _modified nodex

        boost::container::set<Transformable*> _toRemove;
        mutable boost::mutex _mutexToRemove; // protect _toRemove
#endif

        /* methods private */
        void addCreatedTransformable(Transformable*);
        SubScene* subScene(const ivec3&);
        Option<SubScene*> existSubScene(const ivec3&) const;
        void insertInNeighbors(SubScene*, const ivec3&, Transformable*);

        bool removeTransformable(Transformable*, bool);

    };


    /** Inline implementation */
    inline ivec3 SceneManager::subSceneIndex(const vec3& pos)
    {
#ifdef USE_OCTREE
        vec3 divPos = pos / OctreeNode::SIZE_ROOT;
#else
        vec3 divPos = pos / SceneGrid::GRID_SIZE;
#endif
        return ivec3(round(divPos.x()), round(divPos.y()), round(divPos.z()));
    }

    template<class Factory>
    Transformable* SceneManager::addTransformable(Factory& factory)
    {
        Transformable* obj = factory(this);
        addCreatedTransformable(obj);
        return obj;
    }
#ifdef USE_OCTREE
    inline bool SceneManager::removeTransformable(Transformable* obj)
    {
        return removeTransformable(obj, true);
    }

    inline int SceneManager::flush()
    {
        int ret = _modifiedNodes.size();

        while(_modifiedNodes.size())
            (*_modifiedNodes.begin())->flushDepth();

        for(auto obj : _toRemove)
        {
            delete obj;
        }

        return ret;
    }

    inline void SceneManager::referenceModifiedNode(OctreeNode* node)
    {
        boost::lock_guard<decltype(_mutexModifiedNodes)> g(_mutexModifiedNodes);
        _modifiedNodes.insert(node);
    }

    inline void SceneManager::deferenceModifiedNode(OctreeNode* node)
    {
        boost::lock_guard<decltype(_mutexModifiedNodes)> g(_mutexModifiedNodes);
        _modifiedNodes.erase(node);
    }
#else
    inline int SceneManager::flush()
    {
        return 0;
    }

#endif

    template<class T>
    void SceneManager::take(T& taker, size_t depthLimit)
    {
        ivec3 index = subSceneIndex(taker.center());
        SubScene* scene = subScene(index);

        Intersection r=scene->tree.take(taker, depthLimit);
        if(r == INTERSECT)
        {
            for(auto& neighbor : scene->neighbors)
            {
                neighbor.second->tree.take(taker, depthLimit);
            }
        }
    }

    template<class T>
    void SceneManager::take_brute(T& taker)
    {
    #ifndef USE_OCTREE
        ivec3 index = subSceneIndex(taker.center());
        SubScene* scene = subScene(index);

        Intersection r=scene->tree.take_brute(taker);
        if(r == INTERSECT)
        {
            for(auto& neighbor : scene->neighbors)
            {
                neighbor.second->tree.take_brute(taker);
            }
        }
    #endif
    }

}
}
#include "MemoryLoggerOff.h"

#endif // SCENEMANAGER_H
