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
        return false;
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

}
}
#include "MemoryLoggerOff.h"

#endif // OCTREENODE_H
