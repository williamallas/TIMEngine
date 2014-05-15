#ifdef USE_OCTREE

#include "OctreeNode.h"
#include "SceneManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{

const vec3 OctreeNode::SIZE_ROOT = vec3(4096);
const size_t OctreeNode::MAX_DEPTH = 5;
const size_t OctreeNode::MAX_ELEMENT = 32;

OctreeNode::OctreeNode(const vec3& center, SceneManager* scene) : TransformableContainer()
{
    _sceneManager = scene;
    _box.setMin(center - vec3(SIZE_ROOT.x()/2));
    _box.setMax(center + vec3(SIZE_ROOT.x()/2));

    _parent = nullptr;
    _root = this;
    _isLeaf = true;
    _depth = 0;

    for(size_t i=0 ; i<8 ; ++i)
        _child[i] = nullptr;
}

OctreeNode::OctreeNode(const vec3& center, SceneManager* scene, OctreeNode* parent, OctreeNode* root, int depth) : TransformableContainer()
{
    _sceneManager = scene;
    _box.setMin(center - parent->box().halfSize()/2);
    _box.setMax(center + parent->box().halfSize()/2);

    _parent = parent;
    _root = root;
    _isLeaf = true;
    _depth = depth;

    for(size_t i=0 ; i<8 ; ++i)
        _child[i] = nullptr;
}

OctreeNode::~OctreeNode()
{
    if(!_actionStack.empty())
        _sceneManager->deferenceModifiedNode(this);

    if(_isLeaf)
    {
        for(size_t i=0 ; i<_container.size() ; ++i)
            _container[i]->removeContainer(this);
    }
    else
    {
        for(size_t i=0 ; i<8 ; ++i)
        {
            delete _child[i];
        }
    }

    if(_depth == 0)
    {
        for(size_t i=0 ; i<_container.size() ; ++i)
        {
            _container[i]->setLowestCommonParent(nullptr);
            _container[i]->_sceneManager=nullptr;
        }
    }
}

Intersection OctreeNode::insert(Transformable* obj, bool fromToNode)
{
    Intersection inter;
    if(obj->volume().obb)
        inter = _box.collide(obj->volume().box());
    else
        inter = _box.collide(obj->volume().sphere);

    if(inter==OUTSIDE)
        return OUTSIDE;
    else if(inter==INSIDE)
    {
        if(obj->lowestCommonParent() && ((OctreeNode*)obj->lowestCommonParent())->depth() < _depth)
            obj->setLowestCommonParent(this);
        else if(!obj->lowestCommonParent())
            obj->setLowestCommonParent(this);
    }

    NodeAction act = { obj, NodeAction::ADD };
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        _actionStack.push(act);
    }

    if(!fromToNode)
        _sceneManager->referenceModifiedNode(this);

    if(_isLeaf)
    {
        obj->addContainer(this);
    }
    else
    {
        for(size_t i=0 ; i<8 ; ++i)
        {
            if(_child[i]->insert(obj, fromToNode) == INSIDE)
            {
                break;
            }
        }
    }

    return inter;
}

bool OctreeNode::internRemoveTo(Transformable* obj, OctreeNode* to)
{
    NodeAction act = { obj, NodeAction::REMOVE };
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        _actionStack.push(act);
    }
    _sceneManager->referenceModifiedNode(this);

    if(_parent)
    {
        _parent->removeTo(obj, to);
    }

    return true;
}

bool OctreeNode::internInsertInChild(Transformable* obj)
{
    bool findInside=false;
    for(size_t i=0 ; i<8 ; ++i)
    {
        if(_child[i]->insert(obj, false) == INSIDE)
        {
            findInside=true;
            break;
        }
    }

    if(!findInside)
    {
        OctreeNode* LCP = (OctreeNode*)obj->lowestCommonParent();
        if(LCP && _depth >= LCP->_depth)
            obj->setLowestCommonParent(this);
        else if(!LCP)
            obj->setLowestCommonParent(this);
    }

    return true;
}

bool OctreeNode::remove(Transformable* obj)
{
    NodeAction act = { obj, NodeAction::REMOVE };
    {
        boost::lock_guard<decltype(_mutex)> g(_mutex);
        _actionStack.push(act);
    }
    _sceneManager->referenceModifiedNode(this);

    if(_isLeaf)
    {
        obj->removeContainer(this);
    }

    return  true;
}

void OctreeNode::flushDepth()
{
    if(!_actionStack.empty())
        _sceneManager->deferenceModifiedNode(this);

    while(!_actionStack.empty())
    {
        NodeAction act = _actionStack.front();
        _actionStack.pop();

        switch(act.action)
        {
        case NodeAction::ADD:
            _container.push_back(act.obj);
            break;
        case NodeAction::REMOVE:
            auto it=std::find(_container.begin(), _container.end(), act.obj);
            if(it != _container.end())
                _container.erase(it);
            break;
        }
    }

    if(_isLeaf)
    {
        if(_container.size() >= MAX_ELEMENT && _depth < MAX_DEPTH)
        {
            toNode();
            for(size_t i=0 ; i<8 ; ++i)
                _child[i]->flushDepth();
        }
    }
    else if(_container.size() < MAX_ELEMENT/2)
    {
        toLeaf();
    }
}

void OctreeNode::toNode()
{
    vec3 center = _box.center();
    vec3 half = _box.halfSize()*0.5;

    size_t index=0;
    for(int i=-1 ; i<=1 ; i+=2)
        for(int j=-1 ; j<=1 ; j+=2)
            for(int k=-1 ; k<=1 ; k+=2)
    {
        _child[index] = new OctreeNode({half.x()*i+center.x(),
                                        half.y()*j+center.y(),
                                        half.z()*k+center.z()}, _sceneManager, this, _root, _depth+1);
        ++index;
    }

    _isLeaf = false;

    for(size_t j=0 ; j<_container.size() ; ++j)
    {
        _container[j]->removeContainer(this);
    }

    for(size_t j=0 ; j<_container.size() ; ++j)
    {
        for(size_t i=0 ; i<8 ; ++i)
        {
            if(_child[i]->insert(_container[j], true) == INSIDE)
                break;
        }
    }
}

void OctreeNode::toLeaf()
{
    for(size_t i=0 ; i<8 ; ++i)
    {
        delete _child[i];
        _child[i] = nullptr;
    }

    _isLeaf = true;

    for(size_t j=0 ; j<_container.size() ; ++j)
    {
        _container[j]->addContainer(this);

        bool in=false;
        if(_container[j]->volume().obb)
            in=_box.inside(_container[j]->volume().box());
        else
            in=_box.inside(_container[j]->volume().sphere);

        if(in)
            _container[j]->setLowestCommonParent(this);
    }
}

std::string OctreeNode::str() const
{
    std::string s;
    str(s);
    return s;
}

void OctreeNode::str(std::string& s) const
{
    for(size_t i=0 ; i<_depth ; ++i)
        s += "  ";
    s += "->"+StringUtils(_depth).str()+":";
    s+="\n";

    if(_isLeaf)
    for(size_t i=0 ; i<_container.size() ; ++i)
    {
        for(size_t j=0 ; j<_depth ; ++j)
            s += "  ";
        s+="   "+StringUtils(_container[i]).str()+"\n";
    }

    if(!_isLeaf)
        for(size_t i=0 ; i<8 ;++i)
            _child[i]->str(s);
}


}
}

#endif
