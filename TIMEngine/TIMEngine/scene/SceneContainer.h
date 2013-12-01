#ifndef SCENECONTAINER_H_INCLUDED
#define SCENECONTAINER_H_INCLUDED

#include "core.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    template <class T>
    class SceneContainer
    {

    protected:
        T _container;
        Box _box;

    public:
        SceneContainer() {};
        virtual ~SceneContainer() {};

        decltype(_container.begin()) begin() { return _container.begin(); }
        decltype(_container.begin()) end() { return _container.end(); }

        const Box& box() const { return _box; }

    };
}
}
#include "MemoryLoggerOff.h"

#endif // SCENECONTAINER_H_INCLUDED
