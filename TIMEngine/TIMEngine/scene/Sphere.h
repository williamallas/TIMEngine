#ifndef SPHEREVOLUME_H
#define SPHEREVOLUME_H

#include "core.h"
#include "Intersection.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    class Box;

    class Sphere
    {
    public:
        Sphere();
        Sphere(const vec3&, float);
        Sphere(const vec4&);
        Sphere(const Sphere&);

        virtual ~Sphere();

        /* Getter and Setter */
        vec4 sphere() const;
        const vec3& center() const;
        float radius() const;
        Sphere& setCenter(const vec3&);
        Sphere& setRadius(float);
        Box toBox() const;

        /* Collision */
        bool inside(const Sphere&) const;
        bool outside(const Sphere&) const;

        bool inside(const Box&) const;
        bool outside(const Box&) const;

        template<class T> Intersection collide(const T&) const;

        /* out */
        std::string str() const;
        friend std::ostream& operator<< (std::ostream& stream, const Sphere& t) { stream << t.str(); return stream; }

    private:
        vec3 _center = {0};
        float _radius = 0;
    };


    /** Inline implementation */
    inline vec4 Sphere::sphere() const { return { _center.x(),_center.y(),_center.z(),_radius}; }
    inline const vec3& Sphere::center() const { return _center; }
    inline float Sphere::radius() const { return _radius; }
    inline Sphere& Sphere::setCenter(const vec3& center) { _center = center; return *this; }
    inline Sphere& Sphere::setRadius(float rad) { _radius = rad; return *this; }

    /* Sphere */
    inline bool Sphere::inside(const Sphere& s) const { return (_center-s._center).length() + s._radius < _radius; }
    inline bool Sphere::outside(const Sphere& s) const { return (_center-s._center).length() > _radius + s._radius; }

    template<class T>
    Intersection Sphere::collide(const T& t) const
    {
        if(outside(t)) return OUTSIDE;
        else if(inside(t)) return INSIDE;
        else return INTERSECT;
    }

}
}
#include "MemoryLoggerOff.h"

#endif // SPHEREVOLUME_H
