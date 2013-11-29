#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "core.h"
#include "ArraySceneContainer.h"
#include "OctreeNode.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{

    class SceneManager
    {
    public:
        SceneManager();
        virtual ~SceneManager();

    private:
        struct SubScene
        {
            SubScene(const vec3& center) : tree(center), container(tree.box())  {}
            OctreeNode tree;
            ArraySceneContainer container;

            boost::container::vector<SubScene*> neighbors;
        };

        boost::container::vector<SubScene*> _subScene;
        SubScene * _currentScene;

    };

}
}
#include "MemoryLoggerOff.h"

#endif // SCENEMANAGER_H
