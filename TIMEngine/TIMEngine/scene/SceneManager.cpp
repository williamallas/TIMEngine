#include "SceneManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace scene
{

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{
    for(size_t i=0 ; i<_subScene.size() ; i++)
    {
        delete _subScene[i];
    }
}

}
}
