#ifndef ORIENTEDBOX_H
#define ORIENTEDBOX_H

#include "Box.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{

    struct OrientedBoxAxis
    {
        vec3 center;
        vec3 axis[3];
    };

    class OrientedBox
    {
        public:

            OrientedBox();
            OrientedBox(const Box&, const mat4&);
            virtual ~OrientedBox();

            const Box& box() const;
            const mat4& matrix() const;
            const mat4& inverseMatrix() const;

            OrientedBox& setMatrix(const mat4&);
            OrientedBox& setBox(const Box&);

            static void computeAxis(const Box&, const mat4&, OrientedBoxAxis&);

            /* out */
            std::string str() const;
            friend std::ostream& operator<< (std::ostream& stream, const OrientedBox& t) { stream << t.str(); return stream; }

        private:
            Box _box;
            mat4 _matrix, _invMatrix;
    };


    /** Inline implementation */
    inline const Box& OrientedBox::box() const { return _box; }
    inline const mat4& OrientedBox::matrix() const { return _matrix; }
    inline const mat4& OrientedBox::inverseMatrix() const { return _invMatrix; }

    inline OrientedBox& OrientedBox::setMatrix(const mat4& mat) { _matrix=mat; _invMatrix=mat.inverted(); return *this; }
    inline OrientedBox& OrientedBox::setBox(const Box& box) { _box=box; return *this; }

}
}
#include "MemoryLoggerOff.h"

#endif // ORIENTEDBOX_H
