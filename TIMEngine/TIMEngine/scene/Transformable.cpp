#include "Transformable.h"
#include "SceneManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{

Transformable::~Transformable()
{
    if(_sceneManager)
        _sceneManager->removeTransformable(this);
}

}
}
