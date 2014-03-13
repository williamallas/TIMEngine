#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "core.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace interface
{
    struct Camera
    {
        vec3 pos, dir={0,1,0}, up={0,0,1};
        float fov=70, ratio=1;
        vec2 clipDist={1,100};
    };
}
}
#include "MemoryLoggerOff.h"


#endif // CAMERA_H_INCLUDED
