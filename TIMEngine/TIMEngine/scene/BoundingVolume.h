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
    struct BoundingVolume
    {
        Sphere sphere;
        OrientedBox obb;
        bool accurate;
    };

}
}
#include "MemoryLoggerOff.h"


#endif // BOUNDINGVOLUME_H_INCLUDED
