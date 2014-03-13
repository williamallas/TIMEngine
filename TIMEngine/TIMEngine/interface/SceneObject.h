#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include "core.h"
#include "scene\Transformable.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{
    class SceneManager;

    class SceneObject : public scene::Transformable
    {
        friend class SceneManager;

    public:
        enum ObjectType
        {
            SCENE=0,
            RENDERABLE=1,
        };
        enum ObjectTypeMask
        {
            MASK_SCENE=BUILD_MASK(ObjectType::SCENE),
            MASK_RENDERABLE=BUILD_MASK(ObjectType::RENDERABLE),
        };

        ObjectType type() const;
        const mat4& matrix() const;
        const scene::BoundingVolume& initialVolume() const;

        void setInitialVolume(const scene::BoundingVolume&);
        void setMatrix(const mat4&);
        void setMatrixNoScale(const mat4&);

        void remove();

    private:
        mat4 _matrix;
        scene::BoundingVolume _initialVolume;
        bool _filterUtils[8]={false};

    protected:
        ObjectType _type;
        SceneManager* _manager=nullptr;
        SceneObject(const mat4&, scene::SceneManager*);
        virtual ~SceneObject();
    };

    inline const mat4& SceneObject::matrix() const { return _matrix; }
    inline const scene::BoundingVolume& SceneObject::initialVolume() const { return _initialVolume; }
    inline SceneObject::ObjectType SceneObject::type() const { return _type; }

}
}
#include "MemoryLoggerOff.h"

#endif // SCENEOBJECT_H
