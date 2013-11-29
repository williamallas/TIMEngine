#include "OctreeNode.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{

const vec3 OctreeNode::SIZE_ROOT = vec3(1024);
const size_t OctreeNode::MAX_DEPTH = 6;
const size_t OctreeNode::MAX_ELEMENT = 32;

OctreeNode::OctreeNode(const vec3& center) : TransformableContainer()
{
    _box.setMin(center - vec3(SIZE_ROOT.x()/2));
    _box.setMax(center + vec3(SIZE_ROOT.x()/2));

    _parent = nullptr;
    _isLeaf = true;
    _depth = 0;

    for(size_t i=0 ; i<8 ; i++)
        _child[i] = nullptr;
}

OctreeNode::OctreeNode(const vec3& center, OctreeNode* parent, int depth) : TransformableContainer()
{
    _box.setMin(center - parent->box().halfSize()*0.5);
    _box.setMax(center + parent->box().halfSize()*0.5);

    _parent = parent;
    _isLeaf = true;
    _depth = depth;

    for(size_t i=0 ; i<8 ; i++)
        _child[i] = nullptr;
}

OctreeNode::~OctreeNode()
{
    if(_isLeaf)
    {
        for(size_t i=0 ; i<_container.size() ; i++)
            _container[i]->removeContainer(this);
    }
    else
    {
        for(size_t i=0 ; i<8 ; i++)
        {
            delete _child[i];
        }
    }
}

Intersection OctreeNode::insert(Transformable* obj)
{
    Intersection inter;
    if(obj->volume().accurate)
        inter = _box.collide(obj->volume().obb);
    else
        inter = _box.collide(obj->volume().sphere);

    if(inter==OUTSIDE)
        return OUTSIDE;

    if(_container.size() >= MAX_ELEMENT && _depth < MAX_DEPTH && _isLeaf)
        toNode();

    _container.push_back(obj);

    if(!_isLeaf)
    {
        for(size_t i=0 ; i<8 ; i++)
            if(_child[i]->insert(obj) == INSIDE)
                break;
    }
    else
        obj->addContainer(this);

    return inter;
}

bool OctreeNode::removeFromRoot(Transformable* obj)
{
    auto it = std::find(_container.begin(), _container.end(), obj);
    if(it == _container.end())
        return false;

    _container.erase(it);
    if(_isLeaf)
    {
        obj->removeContainer(this);
    }
    else
    {
        if(_container.size() < MAX_ELEMENT)
            toLeaf();
        else
        {
            for(size_t i=0 ; i<8 ; i++)
                _child[i]->removeFromRoot(obj);
        }
    }

    return true;
}

bool OctreeNode::removeFromLeaf(Transformable* obj)
{
    auto it = std::find(_container.begin(), _container.end(), obj);
    if(it == _container.end())
        return false;

    _container.erase(it);
    if(_isLeaf)
    {
        obj->removeContainer(this);
    }
    else
    {
        if(_container.size() < MAX_ELEMENT)
            toLeaf();
    }

    if(_parent)
        _parent->removeFromLeaf(obj);

    return true;
}

void OctreeNode::toNode()
{
    _isLeaf = false;

    vec3 center = _box.center();
    vec3 half = _box.halfSize()*0.5;

    size_t index=0;
    for(int i=-1 ; i<=1 ; i+=2)
        for(int j=-1 ; j<=1 ; j+=2)
            for(int k=-1 ; k<=1 ; k+=2)
    {
        _child[index] = new OctreeNode({half.x()*i+center.x(),
                                        half.y()*j+center.y(),
                                        half.z()*k+center.z()}, this, _depth+1);
        index++;
    }

    for(size_t j=0 ; j<_container.size() ; j++)
        _container[j]->removeContainer(this);

    for(size_t i=0 ; i<8 ; i++)
        for(size_t j=0 ; j<_container.size() ; j++)
        {
            if(_child[i]->insert(_container[j]) == INSIDE)
                break;
        }

}

void OctreeNode::toLeaf()
{
    _isLeaf = true;
    for(size_t i=0 ; i<8 ; i++)
    {
        delete _child[i];
    }

    for(size_t j=0 ; j<_container.size() ; j++)
        _container[j]->addContainer(this);
}

}
}
