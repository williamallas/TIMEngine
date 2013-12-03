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

        bool removeTransformable(Transformable*);

    private:
        struct SubScene
        {
            SubScene(const vec3& center) : tree(center), container(tree.box())  {}
            OctreeNode tree;
            ArraySceneContainer container;

            boost::container::map<ivec3, SubScene*> neighbors;
        };

        boost::container::map<ivec3, SubScene*> _subScene;
        mutable boost::recursive_mutex _mutexSubScene; // protect _subScene

        /* methods private */
        SubScene* subScene(const ivec3&);
        Option<SubScene*> existSubScene(const ivec3&) const;
        void insertInNeighbors(SubScene*, const ivec3&, Transformable*);

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
        ivec3 index = subSceneIndex(obj->volumeCenter());

        SubScene * scene = subScene(index);

        if(inOctree)
        {
            if(scene->tree.insert(obj) == INTERSECT)
                insertInNeighbors(scene, index, obj);
        }
        else
            scene->container.insert(obj);

        return obj;
    }

}
}
#include "MemoryLoggerOff.h"

#endif // SCENEMANAGER_H
