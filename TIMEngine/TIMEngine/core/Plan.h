#ifndef PLAN_H_INCLUDED
#define PLAN_H_INCLUDED

#include "Vector.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    class Plan
    {
    public:
        Plan() {}

        Plan(const vec3& p1, const vec3& p2, const vec3& p3)
        { setPlan(p1, p2, p3); }

        Plan(const vec3& point, const vec3& normal)
        { setPlan(point, normal); }

        const vec4& plan() const { return _plan; }

        void setPlan(const vec3& point, const vec3& normal)
        {
            vec3 n = normal.normalized();
            _plan = {n.x(), n.y(), n.z(), -n.dot(point)};
        }

        void setPlan(const vec3& p1, const vec3& p2, const vec3& p3)
        { setPlan(p2, (p1 - p2).cross(p3 - p2)); }

        float distance(const vec3& p) const
        { return (_plan.x()*p.x() + _plan.y()*p.y() + _plan.z()*p.z() + _plan.w()); }

    private:
        vec4 _plan = {0,0,1,0};

    };
}
}
#include "MemoryLoggerOn.h"

#endif // PLAN_H_INCLUDED
