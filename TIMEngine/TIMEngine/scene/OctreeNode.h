#ifndef OCTREENODE_H
#define OCTREENODE_H

#include "core.h"
#include "Transformable.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    class OctreeNode : public TransformableContainer
    {
    public:
        static const vec3 SIZE_ROOT;

        OctreeNode(const vec3&);
        virtual ~OctreeNode();

        Intersection insert(Transformable*);
        bool remove(Transformable*);
        bool removeFromRoot(Transformable*);
        bool removeFromLeaf(Transformable*);

    private:
        OctreeNode* _parent, *_root;
        OctreeNode* _child[8];
        bool _isLeaf;
        size_t _depth;
        boost::mutex _mutex;

        static const size_t MAX_DEPTH;
        static const size_t MAX_ELEMENT;

        OctreeNode(const vec3&, OctreeNode*, OctreeNode*, int);

        void toNode();
        void toLeaf();

        Intersection internInsert(Transformable*);
        bool internRemoveFromRoot(Transformable*);
        bool internRemoveFromLeaf(Transformable*);
    };


    /** Inline implementation */
    inline bool OctreeNode::remove(Transformable* obj) { return internRemoveFromRoot(obj); }
    inline Intersection OctreeNode::insert(Transformable* obj) { return internInsert(obj); }
    inline bool OctreeNode::removeFromRoot(Transformable* obj){ return internRemoveFromRoot(obj); }
    inline bool OctreeNode::removeFromLeaf(Transformable* obj) { return internRemoveFromLeaf(obj); }

}
}
#include "MemoryLoggerOff.h"

#endif // OCTREENODE_H
