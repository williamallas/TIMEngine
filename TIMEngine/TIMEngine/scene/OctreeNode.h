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
        OctreeNode* _parent;
        OctreeNode* _child[8];
        bool _isLeaf;
        size_t _depth;

        static const size_t MAX_DEPTH;
        static const size_t MAX_ELEMENT;

        OctreeNode(const vec3&, OctreeNode*, int);

        void toNode();
        void toLeaf();
    };


    /** Inline implementation */
    inline bool OctreeNode::remove(Transformable* obj) { return removeFromRoot(obj); }

}
}
#include "MemoryLoggerOff.h"

#endif // OCTREENODE_H
