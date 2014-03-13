#ifndef SCENEMANAGERI_H
#define SCENEMANAGERI_H

#include <boost/timer/timer.hpp>
#include "scene\SceneManager.h"
#include "scene\Frustum.h"
#include "renderer\MaterialRenderer.h"
#include "RenderableObject.h"
#include "Camera.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{
    class SceneManager
    {
    public:

        SceneManager();
        virtual ~SceneManager();

        SceneObject* addSceneObject(const mat4& mat=mat4::IDENTITY());
        RenderableObject* addRenderableObject(const mat4& mat=mat4::IDENTITY());

        void notifyStreamMeshRenderable(RenderableObject*);

        int endScene();
        void clearScene();

        template <class T>
        static void toRenderableMaterialList(boost::container::vector<renderer::MaterialInstance*>&,
                                             const T&, size_t, renderer::MaterialRenderer*);

        template <class T>
        static size_t drawObjects(const T&, size_t, renderer::MaterialRenderer*);

        /* take method */
        void frustumTest(const Camera&, boost::container::vector<SceneObject*>&,
                        SceneObject::ObjectTypeMask, bool bruteforce,
                        size_t depthLimit=0xFFFFFFFF, size_t indexMask=0);

        void frustumTest(const scene::Frustum&, const vec3&, boost::container::vector<SceneObject*>&,
                        SceneObject::ObjectTypeMask, bool bruteforce,
                        size_t depthLimit=0xFFFFFFFF, size_t indexMask=0);

        void sphereTest(const Sphere&, boost::container::vector<SceneObject*>&,
                        SceneObject::ObjectTypeMask, bool bruteforce,
                        size_t depthLimit=0xFFFFFFFF, size_t indexMask=0);

        std::string str() const;

    private:
        scene::SceneManager _scene;
        boost::container::vector<RenderableObject*> _renderableEmptyMesh;

        static void filterDrawList(boost::container::vector<SceneObject*>&, size_t);

        static void sortMaterialList(boost::container::vector<renderer::MaterialInstance*>&, size_t, renderer::MaterialRenderer*);
    };

    template <class T>
    size_t SceneManager::drawObjects(const T& obj, size_t pass, renderer::MaterialRenderer* render)
    {
        boost::container::vector<renderer::MaterialInstance*> drawList;
        drawList.reserve(obj.size());

        toRenderableMaterialList(drawList, obj, pass, render);
        return render->render(drawList.begin(), drawList.end(), pass);
    }

    template <class T>
    void SceneManager::toRenderableMaterialList(boost::container::vector<renderer::MaterialInstance*>& mlist,
                                                const T& obj, size_t pass, renderer::MaterialRenderer* render)
    {
        mlist.reserve(obj.size());
        for(SceneObject* e:obj)
        {
            for(size_t i=0 ; i<reinterpret_cast<RenderableObject*>(e)->nbElement() ; i++)
                mlist.push_back(&(reinterpret_cast<RenderableObject*>(e)->_mesh[i]));
        }

        sortMaterialList(mlist, pass, render);

    }

    inline void SceneManager::notifyStreamMeshRenderable(RenderableObject* object)
    {
        _renderableEmptyMesh.push_back(object);
    }

    inline int SceneManager::endScene()
    {
        auto tmp = boost::move(_renderableEmptyMesh);

        for(size_t i=0 ; i<tmp.size() ; i++)
        {
            if(tmp[i]->isComplete())
                tmp[i]->computeVolume();
            else _renderableEmptyMesh.push_back(tmp[i]);
        }
        return _scene.flush();
    }

    inline void SceneManager::clearScene() { _scene.clearScene(); }
    inline std::string SceneManager::str() const { return _scene.str(); }
}
}
#include "MemoryLoggerOff.h"

#endif // SCENEMANAGERI_H
