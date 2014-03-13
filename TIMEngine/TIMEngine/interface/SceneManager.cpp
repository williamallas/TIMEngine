#include "SceneManager.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{

SceneManager::SceneManager()
{

}

SceneManager::~SceneManager()
{

}

SceneObject* SceneManager::addSceneObject(const mat4& mat)
{
    class Factory
    {
    public:
        const mat4 *matrix=nullptr;
        SceneObject* operator()(scene::SceneManager* scene) { return new SceneObject(*matrix, scene); }
    };
    static Factory facto;
    facto.matrix=&mat;

    auto tmp=reinterpret_cast<SceneObject*>(_scene.addTransformable(facto));
    tmp->_manager=this;
    return tmp;
}

RenderableObject* SceneManager::addRenderableObject(const mat4& mat)
{
    class Factory
    {
    public:
        const mat4 *matrix=nullptr;
        RenderableObject* operator()(scene::SceneManager* scene) { return new RenderableObject(*matrix, scene); }
    };
    static Factory facto;
    facto.matrix=&mat;

    auto tmp = reinterpret_cast<RenderableObject*>(_scene.addTransformable(facto));
    tmp->_manager=this;
    return tmp;
}

void SceneManager::frustumTest(const Camera& camera, boost::container::vector<SceneObject*>& obj,
                               SceneObject::ObjectTypeMask mask, bool bruteforce,
                               size_t depthLimit, size_t indexMask)
{
    class FrustumTest
    {
    public:
        FrustumTest() {}

        SceneObject::ObjectTypeMask mask;
        boost::container::vector<SceneObject*>* obj;
        scene::Frustum frustum;
        vec3 c;
        size_t indexMask;

        Intersection collide(const Box& b)
        { return frustum.collide(b); }

        const vec3& center() const
        { return c; }

        bool filter(scene::Transformable* t) const
        {
            return (BUILD_MASK(reinterpret_cast<SceneObject*>(t)->type()) & mask) &&
                    !reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask];
        }

        void take(scene::Transformable* t)
        {
            if(frustum.collide(reinterpret_cast<SceneObject*>(t)->volume().sphere) != OUTSIDE)
            {
                reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask]=true;
                obj->push_back(reinterpret_cast<SceneObject*>(t));
            }
        }

        void takeSure(scene::Transformable* t)
        {
            reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask]=true;
            obj->push_back(reinterpret_cast<SceneObject*>(t));
        }
    };

    FrustumTest f;
    f.mask = mask;
    f.obj = &obj;
    f.indexMask=indexMask;
    f.c = (camera.pos + (camera.dir-camera.pos).normalized()*(camera.clipDist.y()-camera.clipDist.x()))*0.5;
    f.frustum.buildCameraFrustum(camera.fov, camera.ratio, camera.clipDist,
                                 camera.pos, camera.dir, camera.up);

    if(bruteforce)
        _scene.take_brute(f);
    else
        _scene.take(f, depthLimit);

    filterDrawList(obj,indexMask);
}

void SceneManager::sphereTest(const Sphere& s, boost::container::vector<SceneObject*>& obj,
                              SceneObject::ObjectTypeMask mask, bool bruteforce,
                              size_t depthLimit, size_t indexMask)
{
    class SphereTest
    {
    public:
        SphereTest() {}

        SceneObject::ObjectTypeMask mask;
        boost::container::vector<SceneObject*>* obj;
        Sphere sphere;
        size_t indexMask;

        Intersection collide(const Box& b)
        { return sphere.collide(b); }

        const vec3& center() const
        { return sphere.center(); }

        bool filter(scene::Transformable* t) const
        {
            return (BUILD_MASK(reinterpret_cast<SceneObject*>(t)->type()) & mask) &&
                    !reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask];
        }

        void take(scene::Transformable* t)
        {
            if(sphere.collide(reinterpret_cast<SceneObject*>(t)->volume().sphere) != OUTSIDE)
            {
                reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask]=true;
                obj->push_back(reinterpret_cast<SceneObject*>(t));
            }
        }

        void takeSure(scene::Transformable* t)
        {
            reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask]=true;
            obj->push_back(reinterpret_cast<SceneObject*>(t));
        }
    };

    SphereTest f;
    f.mask = mask;
    f.indexMask=indexMask;
    f.obj = &obj;
    f.sphere = s;

    if(bruteforce)
        _scene.take_brute(f);
    else
        _scene.take(f, depthLimit);

    filterDrawList(obj,indexMask);
}

void SceneManager::frustumTest(const scene::Frustum& frustum, const vec3& center, boost::container::vector<SceneObject*>& obj,
                               SceneObject::ObjectTypeMask mask, bool bruteforce,
                               size_t depthLimit, size_t indexMask)
{
    class FrustumTest
    {
    public:
        FrustumTest() {}

        SceneObject::ObjectTypeMask mask;
        boost::container::vector<SceneObject*>* obj;
        scene::Frustum frustum;
        vec3 c;
        size_t indexMask;

        Intersection collide(const Box& b)
        { return frustum.collide(b); }

        const vec3& center() const
        { return c; }

        bool filter(scene::Transformable* t) const
        {
            return (BUILD_MASK(reinterpret_cast<SceneObject*>(t)->type()) & mask) &&
                    !reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask];
        }

        void take(scene::Transformable* t)
        {
            if(frustum.collide(reinterpret_cast<SceneObject*>(t)->volume().sphere) != OUTSIDE)
            {
                reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask]=true;
                obj->push_back(reinterpret_cast<SceneObject*>(t));
            }
        }

        void takeSure(scene::Transformable* t)
        {
            reinterpret_cast<SceneObject*>(t)->_filterUtils[indexMask]=true;
            obj->push_back(reinterpret_cast<SceneObject*>(t));
        }
    };

    FrustumTest f;
    f.mask = mask;
    f.indexMask=indexMask;
    f.obj = &obj;
    f.c = center;
    f.frustum=frustum;

    if(bruteforce)
        _scene.take_brute(f);
    else
        _scene.take(f, depthLimit);

    filterDrawList(obj,indexMask);
}

void SceneManager::filterDrawList(boost::container::vector<SceneObject*>& l, size_t filterIndex)
{
    for(SceneObject* obj : l)
        obj->_filterUtils[filterIndex]=false;
}

void SceneManager::sortMaterialList(boost::container::vector<renderer::MaterialInstance*>& drawList, size_t pass, renderer::MaterialRenderer* render)
{
    struct Compar
    {
        bool operator()(renderer::MaterialInstance*m1, renderer::MaterialInstance*m2) const
        { return m1->material(renderer::MaterialPass::COLOR_PASS)->index() <m2->material(renderer::MaterialPass::COLOR_PASS)->index(); }
    };

    std::sort(drawList.begin(), drawList.end(), Compar());

    int firstBlend=-1, lastBlend=-1;
    for(int i=(size_t)drawList.size()-1 ; i>=0 ; i--)
    {
        if(drawList[i]->material(pass))
        {
            if(drawList[i]->material(pass)->blend())
            {
                if(lastBlend == -1)
                    lastBlend = (int)i;
                firstBlend = (int)i;
            }
            else if(!drawList[i]->material(pass)->alwaysLast())
            {
                break;
            }
        }
    }

    struct Compar2
    {
        bool operator()(renderer::MaterialInstance*m1, renderer::MaterialInstance*m2) const
        {
            return (m1->matrix()->translation() - cameraPos).length2() >
                   (m2->matrix()->translation() - cameraPos).length2(); }

            vec3 cameraPos;
    };

    if(firstBlend != -1 && lastBlend != -1)
    {
        Compar2 fonct;
        fonct.cameraPos = render->cameraPos();
        std::sort(drawList.begin()+firstBlend, drawList.begin()+lastBlend, fonct);

    }
}

}
}
