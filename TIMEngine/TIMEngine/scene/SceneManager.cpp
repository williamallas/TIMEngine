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
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

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
            if(it.second->container.box().collide(obj->volume().sphere) != OUTSIDE)
            {
                it.second->container.remove(obj);
            }
        }
    }

    obj->_sceneManager = nullptr;
    return ret;
}

SceneManager::SubScene* SceneManager::subScene(const ivec3& pos)
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

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
            newSubScene->neighbors[ivec3(x,y,z)] = opt.value();
            opt.value()->neighbors[-ivec3(x,y,z)] = newSubScene;
        }
    }

    return newSubScene;
}

Option<SceneManager::SubScene*> SceneManager::existSubScene(const ivec3& pos) const
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

    auto it = _subScene.find(pos);
    if(it != _subScene.end())
        return Option<SceneManager::SubScene*>(it->second);
    else
        return Option<SceneManager::SubScene*>();
}

void SceneManager::insertInNeighbors(SubScene* sub, const ivec3& index, Transformable* obj)
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

    for(int x=-1 ; x<=1 ; x+=1)
        for(int y=-1 ; y<=1 ; y+=1)
            for(int z=-1 ; z<=1 ; z+=1)
    {
        if(ivec3(x,y,z)==ivec3(0))
            continue;

        auto it = sub->neighbors.find(ivec3(x,y,z));
        if(it != sub->neighbors.end())
            it->second->tree.insert(obj);
        else
        {
            vec3 center(index.x()+x, index.y()+y, index.z()+z);
            center *= OctreeNode::SIZE_ROOT;
            Box box(center-OctreeNode::SIZE_ROOT/2, center+OctreeNode::SIZE_ROOT/2);

            if(obj->volume().accurate)
            {
                if(!box.outside(obj->volume().obb))
                    subScene(ivec3(x,y,z)+index)->tree.insert(obj);
            }
            else
            {
                if(!box.outside(obj->volume().sphere))
                    subScene(ivec3(x,y,z)+index)->tree.insert(obj);
            }
        }
    }
}

}
}
