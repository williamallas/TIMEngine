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

}

void Transformable::move()
{
    _sceneManager->move(this);
}

void Transformable::deleteFromSceneManager()
{
    _sceneManager->removeTransformable(this);
}

}
}
