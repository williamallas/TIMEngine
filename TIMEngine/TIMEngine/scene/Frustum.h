#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "core.h"
#include "Intersection.h"
#include "Sphere.h"
#include "Box.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace scene
{
    class Frustum
    {
    public:
        Frustum();
        virtual ~Frustum();

        enum FrustumPlan
        {
            LEFT=0,
            RIGHT,
            UP,
            DOWN,
            NEAR,
            FAR,
        };

        void buildCameraFrustum(float fov, float ratio, const vec2& dist,
                                const vec3& pos, const vec3& dir, const vec3& up,
                                size_t maskPlanDiscard=0);

        void add(const Plan&);
        void clear();

        /* Collision*/
        Intersection collide(const Sphere&) const;
        Intersection collide(const Box&) const;
        bool collide(const vec3&) const;

    private:
        boost::container::vector<Plan> _plans;

        vec3 getBoxVertexP(const Box&, const vec3&) const;
        vec3 getBoxVertexN(const Box&, const vec3&) const;
    };

    inline void Frustum::clear() { _plans.clear(); }
    inline void Frustum::add(const Plan& p) { _plans.push_back(p); }
}
}
#include "MemoryLoggerOff.h"

#endif // FRUSTUM_H
