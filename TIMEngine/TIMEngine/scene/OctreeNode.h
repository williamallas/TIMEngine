#ifndef OCTREENODE_H
#define OCTREENODE_H

#ifdef USE_OCTREE

#include "core.h"
#include "Transformable.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    class SceneManager;

    class OctreeNode : public TransformableContainer
    {
    public:
        static const vec3 SIZE_ROOT;

        OctreeNode(const vec3&, SceneManager*);
        virtual ~OctreeNode();

        OctreeNode* parent() const;
        int depth() const;

        void flushDepth(); // async

        Intersection insert(Transformable*, bool);
        bool removeTo(Transformable*, OctreeNode*);
        bool insertInChild(Transformable*);
        bool remove(Transformable*);

        template <class T>
        Intersection take(T&, size_t) const;

        /* output */
        std::string str() const;

    private:
        SceneManager* _sceneManager;
        OctreeNode* _parent, *_root;
        OctreeNode* _child[8];
        bool _isLeaf;
        size_t _depth;

        struct NodeAction
        {
            enum { ADD, REMOVE };
            Transformable* obj;
            int action;
        };

        boost::mutex _mutex;
        std::queue<NodeAction> _actionStack;

        static const size_t MAX_DEPTH;
        static const size_t MAX_ELEMENT;

        OctreeNode(const vec3&, SceneManager*, OctreeNode*, OctreeNode*, int);

        void toNode();
        void toLeaf();
        bool internRemoveTo(Transformable*, OctreeNode*);
        bool internInsertInChild(Transformable*);

        void str(std::string&) const;
    };


    /** Inline implementation */
    inline OctreeNode* OctreeNode::parent() const { return _parent; }
    inline int OctreeNode::depth() const { return _depth; }

    inline bool OctreeNode::removeTo(Transformable* obj, OctreeNode* to)
    {
        if(this == to)
            return false;
        else return internRemoveTo(obj, to);
    }

    inline bool OctreeNode::insertInChild(Transformable* obj)
    {
        if(_isLeaf)
        {
            obj->setLowestCommonParent(this);
            obj->addContainer(this);
            return false;
        }
        else return internInsertInChild(obj);
    }

    template <class T>
    Intersection OctreeNode::take(T& taker, size_t depthLimit) const
    {
        Intersection inter = taker.collide(_box);

        if(inter == OUTSIDE)
            return OUTSIDE;
        else if(inter == INSIDE)
        {
            for(Transformable* t : _container)
                taker.takeSure(t);
        }
        else if( _isLeaf || depthLimit==_depth)
        {
            for(Transformable* t : _container)
            {
                if(taker.filter(t))
                    taker.take(t);
            }
        }
        else for(size_t i=0 ; i<8 ; ++i)
            _child[i]->take(taker, depthLimit);

        return inter;
    }

}
}
#include "MemoryLoggerOff.h"
#endif

#endif // OCTREENODE_H
