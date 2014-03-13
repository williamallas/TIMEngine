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
    clearScene();
}

std::string SceneManager::str() const
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);
    std::string s;
    for(auto it : _subScene)
        s += it.first.str() + it.second->tree.str();
    return s;
}

#ifdef USE_OCTREE
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
        err("Error id:45618612\n");
        exit(8);
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
#else

bool SceneManager::removeTransformable(Transformable* obj)
{
    for(size_t i=0 ; i<obj->cells().size() ; i++)
        reinterpret_cast<SceneGrid::Cell*>(obj->cells()[i].container)->remove(obj->cells()[i].index);
    for(size_t i=0 ; i<obj->grids().size() ; i++)
        reinterpret_cast<SceneGrid*>(obj->grids()[i].container)->remove(obj->grids()[i].index);

    delete obj;

    return true;
}

void SceneManager::move(Transformable* obj)
{
    if(obj->grids().size() == 1)
    {
        SceneGrid* grid=reinterpret_cast<SceneGrid*>(obj->grids()[0].container);
        if(grid->box().inside(obj->volume().sphere))
        {
            for(size_t i=0 ; i<obj->cells().size() ; i++)
                reinterpret_cast<SceneGrid::Cell*>(obj->cells()[i].container)->remove(obj->cells()[i].index);

            obj->cells().clear();
            grid->inject(obj);
            return;
        }
    }

    for(size_t i=0 ; i<obj->cells().size() ; i++)
        reinterpret_cast<SceneGrid::Cell*>(obj->cells()[i].container)->remove(obj->cells()[i].index);
    for(size_t i=0 ; i<obj->grids().size() ; i++)
        reinterpret_cast<SceneGrid*>(obj->grids()[i].container)->remove(obj->grids()[i].index);

    obj->grids().clear();
    obj->cells().clear();

    addCreatedTransformable(obj);
}

#endif


#ifdef USE_OCTREE
void SceneManager::move(Transformable* obj)
{
    boost::lock_guard<decltype(*obj)> objGuard(*obj);

    auto& container = obj->container();
    if(container.size() == 1)
    {
        if(!obj->volume().obb)
        {
            if(reinterpret_cast<OctreeNode*>(container[0])->box().inside(obj->volume().sphere))
            {
                return;
            }
        }
        else if(reinterpret_cast<OctreeNode*>(container[0])->box().inside(obj->volume().box()))
        {
            return;
        }
    }
    else if(container.empty())
    {
        err("Error id:518318\n");
        exit(8);
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
        addCreatedTransformable(obj);
        obj->lock();
    }
}
#else



#endif

SceneManager::SubScene* SceneManager::subScene(const ivec3& pos)
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

    auto it = _subScene.find(pos);
    if(it != _subScene.end())
        return it->second;

#ifdef USE_OCTREE
    SubScene * newSubScene = new SubScene( vec3({(float)pos.x(),(float)pos.y(),(float)pos.z()})
                                           * OctreeNode::SIZE_ROOT, this );
#else
    SubScene * newSubScene = new SubScene(vec3({(float)pos.x(),(float)pos.y(),(float)pos.z()})
                                          * SceneGrid::GRID_SIZE, this);
#endif

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
        {
        #ifdef USE_OCTREE
            it->second->tree.insert(obj, false);
        #else
            it->second->tree.insert(obj);
        #endif
        }
        else
        {
            vec3 center(index.x()+x, index.y()+y, index.z()+z);
        #ifdef USE_OCTREE
            center *= OctreeNode::SIZE_ROOT;
            Box box(center-OctreeNode::SIZE_ROOT/2, center+OctreeNode::SIZE_ROOT/2);
        #else
            center *= SceneGrid::GRID_SIZE;
            Box box(center-SceneGrid::GRID_SIZE/2, center+SceneGrid::GRID_SIZE/2);
        #endif

            _mutexSubScene.unlock();
            if(obj->volume().obb)
            {
                if(!box.outside(obj->volume().box()))
                {
                #ifdef USE_OCTREE
                    subScene(ivec3(x,y,z)+index)->tree.insert(obj,false);
                #else
                    subScene(ivec3(x,y,z)+index)->tree.insert(obj);
                #endif
                }
            }
            else
            {
                if(!box.outside(obj->volume().sphere))
                {
                #ifdef USE_OCTREE
                    subScene(ivec3(x,y,z)+index)->tree.insert(obj,false);
                #else
                    subScene(ivec3(x,y,z)+index)->tree.insert(obj);
                #endif
                }
            }
            _mutexSubScene.lock();
        }
    }
}

void SceneManager::addCreatedTransformable(Transformable* obj)
{
    ivec3 index = subSceneIndex(obj->volumeCenter());
    SubScene * scene = subScene(index);

#ifdef USE_OCTREE
   if(scene->tree.insert(obj, false) == INTERSECT)
#else
   if(scene->tree.insert(obj) == INTERSECT)
#endif

    {
        insertInNeighbors(scene, index, obj);
    }
}

void SceneManager::clearScene()
{
    boost::lock_guard<decltype(_mutexSubScene)> guard(_mutexSubScene);

#ifdef USE_OCTREE
    _toRemove.clear();
    for(auto it : _subScene)
    {
        _toRemove.insert(it.second->tree.container().begin(), it.second->tree.container().end());
        delete it.second;
    }
    flush();
#else
    boost::container::set<Transformable*> toRemove;
    for(auto& it : _subScene)
    {
        toRemove.insert(it.second->tree._allObject.begin(), it.second->tree._allObject.end());
        delete it.second;
    }

    for(Transformable* o : toRemove)
    {
        delete o;
    }
#endif
    _subScene.clear();
}

}
}
