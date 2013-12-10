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

std::string SceneManager::str() const
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);
    std::string s;
    for(auto it : _subScene)
        s += it.second->tree.str();
    return s;
}

bool SceneManager::removeTransformable(Transformable* obj, bool toRemove)
{
    bool ret = true;
    boost::lock_guard<decltype(*obj)> objGuard(*obj);
    if(obj->container().size())
    {
        TransformableContainer* LCP = obj->lowestCommonParent();

        for(size_t i=0 ; i<obj->container().size() ; i++)
            ret = ret && ((OctreeNode*)obj->container()[i])->removeTo(obj, ((OctreeNode*)LCP));
        ((OctreeNode*)LCP)->removeTo(obj, nullptr);
    }
    else
    {
        SubScene* scene = subScene(subSceneIndex(obj->volumeCenter()));
        ret=scene->container.remove(obj);

        boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);
        for(auto it : scene->neighbors)
        {
            if(it.second->container.box().collide(obj->volume().sphere) != OUTSIDE)
            {
                it.second->container.remove(obj);
            }
        }
    }

    obj->setLowestCommonParent(nullptr);
    obj->clearContainer();

    if(toRemove)
    {
        boost::lock_guard<decltype(_mutexToRemove)> g(_mutexToRemove);
        _toRemove.insert(obj);
    }

    return ret;
}

void SceneManager::move(Transformable* obj)
{
    boost::lock_guard<decltype(*obj)> objGuard(*obj);
    auto container = obj->container();

    if(container.size() == 1)
    {
        if(!obj->volume().obb)
        {
            if(reinterpret_cast<OctreeNode*>(container[0])->box().inside(obj->volume().sphere))
                return;
        }
        else if(reinterpret_cast<OctreeNode*>(container[0])->box().inside(obj->volume().box()))
        {
            return;
        }
    }
    else if(container.empty())
    {
        obj->unlock();
        removeTransformable(obj, false);
        addCreatedTransformable(obj, false);
        obj->lock();
        return;
    }

    OctreeNode* lowestParent = reinterpret_cast<OctreeNode*>(obj->lowestCommonParent());

    for(size_t i=0 ; i<container.size() ; i++)
        (reinterpret_cast<OctreeNode*>(container[i]))->removeTo(obj, lowestParent);

    obj->clearContainer();
    obj->setLowestCommonParent(nullptr);

    while(lowestParent)
    {
        bool in=false;
        if(obj->volume().obb)
            in=lowestParent->box().inside(obj->volume().box());
        else
            in=lowestParent->box().inside(obj->volume().sphere);

        if(in)
        {
            lowestParent->insertInChild(obj);
            return;
        }
        else
        {

            lowestParent->remove(obj);
            lowestParent = lowestParent->parent();
        }
    }

    if(!lowestParent)
    {
        obj->unlock();
        addCreatedTransformable(obj, true);
        obj->lock();
    }
}


SceneManager::SubScene* SceneManager::subScene(const ivec3& pos)
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

    auto it = _subScene.find(pos);
    if(it != _subScene.end())
        return it->second;

    SubScene * newSubScene = new SubScene( vec3({(float)pos.x(),(float)pos.y(),(float)pos.z()})
                                           * OctreeNode::SIZE_ROOT, this );
    _subScene[pos]=newSubScene;

    for(int x=-1 ; x<=1 ; x+=1)
        for(int y=-1 ; y<=1 ; y+=1)
            for(int z=-1 ; z<=1 ; z+=1)
    {
        if(ivec3(x,y,z)==ivec3(0))
            continue;

        _mutexSubScene.unlock();
        auto opt = existSubScene(pos + ivec3(x,y,z));
        _mutexSubScene.lock();

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

void SceneManager::insertInNeighbors(SubScene* sub, const ivec3& index, Transformable* obj, bool inOctree)
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
        {
            if(inOctree)
                it->second->tree.insert(obj, false);
            else
                it->second->container.insert(obj);
        }
        else
        {
            vec3 center(index.x()+x, index.y()+y, index.z()+z);
            center *= OctreeNode::SIZE_ROOT;
            Box box(center-OctreeNode::SIZE_ROOT/2, center+OctreeNode::SIZE_ROOT/2);

            _mutexSubScene.unlock();
            boost::lock_guard<decltype(*obj)> objGuard(*obj);
            if(obj->volume().obb)
            {
                if(!box.outside(obj->volume().box()))
                {
                    if(inOctree)
                        subScene(ivec3(x,y,z)+index)->tree.insert(obj,false);
                    else
                        subScene(ivec3(x,y,z)+index)->container.insert(obj);
                }
            }
            else
            {
                if(!box.outside(obj->volume().sphere))
                {
                    if(inOctree)
                        subScene(ivec3(x,y,z)+index)->tree.insert(obj, false);
                    else
                        subScene(ivec3(x,y,z)+index)->container.insert(obj);
                }
            }
            _mutexSubScene.lock();
        }
    }
}

void SceneManager::addCreatedTransformable(Transformable* obj, bool inOctree)
{
    boost::lock_guard<decltype(*obj)> objGuard(*obj);

    ivec3 index = subSceneIndex(obj->volumeCenter());
    SubScene * scene = subScene(index);

    if(inOctree)
    {
        if(scene->tree.insert(obj, false) == INTERSECT)
        {
            obj->unlock();
            insertInNeighbors(scene, index, obj, true);
            obj->lock();
        }
    }
    else
    {
        if(scene->container.insert(obj) == INTERSECT)
        {
            obj->unlock();
            insertInNeighbors(scene, index, obj, false);
            obj->lock();
        }
    }
}

void SceneManager::clearScene()
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

    _toRemove.clear();
    for(auto it : _subScene)
    {
        _toRemove.insert(it.second->tree.container().begin(), it.second->tree.container().end());
        _toRemove.insert(it.second->container.container().begin(), it.second->container.container().end());
        delete it.second;
    }
    _subScene.clear();

    flushOctree();
}

}
}
