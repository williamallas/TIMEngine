#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "boost/container/set.hpp"
#include "boost/container/map.hpp"
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

            boost::container::set<SubScene*> neighbors;
        };

        boost::container::map<ivec3, SubScene*> _subScene;

        /* methods private */
        SubScene* subScene(const ivec3&);
        Option<SubScene*> existSubScene(const ivec3&);

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
        SubScene * scene = subScene(subSceneIndex(obj->volumeCenter()));

        if(inOctree)
            scene->tree.insert(obj);
        else
            scene->container.insert(obj);

        return obj;
    }

}
}
#include "MemoryLoggerOff.h"

#endif // SCENEMANAGER_H
