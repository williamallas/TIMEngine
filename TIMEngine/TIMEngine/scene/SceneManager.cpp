#include "SceneManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
    for(auto it : _subScene)
    {
        delete it.second;
    }
}

bool SceneManager::removeTransformable(Transformable* obj)
{
    bool ret = true;

    if(obj->_parentsContainer.size())
    {
        while(obj->_parentsContainer.size())
            ret = ret && ((OctreeNode*)obj->_parentsContainer[0])->removeFromLeaf(obj);

        obj->_parentsContainer.clear();
    }
    else
    {
        SubScene* scene = subScene(subSceneIndex(obj->volumeCenter()));
        ret=scene->container.remove(obj);

        for(auto it : scene->neighbors)
        {
            if(it->container.box().collide(obj->volume().sphere) != OUTSIDE)
            {
                it->container.remove(obj);
            }
        }
    }

    obj->_sceneManager = nullptr;
    return ret;
}

SceneManager::SubScene* SceneManager::subScene(const ivec3& pos)
{
    auto it = _subScene.find(pos);
    if(it != _subScene.end())
        return it->second;

    SubScene * newSubScene = new SubScene( vec3({(float)pos.x(),(float)pos.y(),(float)pos.z()})
                                           * OctreeNode::SIZE_ROOT );
    _subScene[pos]=newSubScene;

    for(int x=-1 ; x<=1 ; x+=1)
        for(int y=-1 ; y<=1 ; y+=1)
            for(int z=-1 ; z<=1 ; z+=1)
    {
        if(ivec3(x,y,z)==ivec3(0))
            continue;

        auto opt = existSubScene(pos + ivec3(x,y,z));
        if(opt.hasValue())
        {
            newSubScene->neighbors.insert(opt.value());
            opt.value()->neighbors.insert(newSubScene);
        }
    }

    return newSubScene;
}

Option<SceneManager::SubScene*> SceneManager::existSubScene(const ivec3& pos)
{
    auto it = _subScene.find(pos);
    if(it != _subScene.end())
        return Option<SceneManager::SubScene*>(it->second);
    else
        return Option<SceneManager::SubScene*>();
}

}
}
