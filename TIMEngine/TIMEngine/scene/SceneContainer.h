#ifndef SCENECONTAINER_H_INCLUDED
#define SCENECONTAINER_H_INCLUDED

#include "core.h"

#define VISIBLE_STATE_SIZE 1

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    template <class T>
    class SceneContainer
    {
        friend class SceneManager;
    protected:
        T _container;
        Box _box;
        bool _visibleState[VISIBLE_STATE_SIZE]={false};

    public:
        SceneContainer() {};
        virtual ~SceneContainer() {};

        const T& container() const { return _container; }
        const Box& box() const { return _box; }

        bool visibleState(size_t index=0) const { return _visibleState[index]; }
        void setVisibleState(bool b, size_t index=0) { _visibleState[index]=b; }

    };
}
}
#include "MemoryLoggerOff.h"

#endif // SCENECONTAINER_H_INCLUDED
