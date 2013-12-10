#ifndef BOUNDINGVOLUME_H_INCLUDED
#define BOUNDINGVOLUME_H_INCLUDED

#include "Sphere.h"
#include "OrientedBox.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace scene
{
    class BoundingVolume
    {
    public:
        BoundingVolume() : obb(nullptr) {}
        ~BoundingVolume() { delete obb; };

        Sphere sphere;
        OrientedBox* obb;

        const OrientedBox& box() const { return *obb; }
    };

}
}
#include "MemoryLoggerOff.h"


#endif // BOUNDINGVOLUME_H_INCLUDED
