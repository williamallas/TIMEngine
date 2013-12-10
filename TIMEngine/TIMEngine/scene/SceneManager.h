#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "core.h"
#include "ArraySceneContainer.h"
#include "OctreeNode.h"

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
        Transformable* addTransformable(Factory&, bool inOctree=true);

        void move(Transformable*);
        bool removeTransformable(Transformable*);

        int flushOctree(); // async
        void clearScene(); // async

        std::string str() const;

        void referenceModifiedNode(OctreeNode*);
        void deferenceModifiedNode(OctreeNode*);

    private:
        struct SubScene
        {
            SubScene(const vec3& center, SceneManager* scene) : tree(center, scene), container(tree.box())  {}
            OctreeNode tree;
            ArraySceneContainer container;

            boost::container::map<ivec3, SubScene*> neighbors;
        };

        boost::container::map<ivec3, SubScene*> _subScene;
        mutable boost::mutex _mutexSubScene; // protect _subScene

        boost::container::set<OctreeNode*> _modifiedNodes;
        mutable boost::mutex _mutexModifiedNodes; // protect _modified nodex

        boost::container::set<Transformable*> _toRemove;
        mutable boost::mutex _mutexToRemove; // protect _toRemove

        /* methods private */
        void addCreatedTransformable(Transformable*, bool);
        SubScene* subScene(const ivec3&);
        Option<SubScene*> existSubScene(const ivec3&) const;
        void insertInNeighbors(SubScene*, const ivec3&, Transformable*, bool);

        bool removeTransformable(Transformable*, bool);

    };


    /** Inline implementation */
    inline ivec3 SceneManager::subSceneIndex(const vec3& pos)
    {
        vec3 divPos = pos / OctreeNode::SIZE_ROOT;
        return ivec3(round(divPos.x()), round(divPos.y()), round(divPos.z()));
    }

    template<class Factory>
    Transformable* SceneManager::addTransformable(Factory& factory, bool inOctree)
    {
        Transformable* obj = factory(this);
        addCreatedTransformable(obj, inOctree);
        return obj;
    }

    inline bool SceneManager::removeTransformable(Transformable* obj)
    {
        return removeTransformable(obj, true);
    }

    inline int SceneManager::flushOctree()
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

}
}
#include "MemoryLoggerOff.h"

#endif // SCENEMANAGER_H
