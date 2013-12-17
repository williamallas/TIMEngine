#ifndef VERTEXFORMAT_H_INCLUDED
#define VERTEXFORMAT_H_INCLUDED

#include "core.h"

#include "MemoryLoggerOn.h"
namespace tim
{
    using namespace core;
namespace renderer
{
    enum VertexFormat
    {
        V,
        VN,
        VNC,
        VNCT,
        VC,
    };

    enum VertexMode
    {
        TRIANGLES=0,
        TRIANGLE_STRIP,
        LINES,
        LINE_STRIP,
        POINTS,
    };

    struct VN_Vertex { vec3 v, n; };
    struct VNC_Vertex { vec3 v, n; vec2 c; };
    struct VNCT_Vertex { vec3 v, n; vec2 c; vec3 t; };
    struct VC_Vertex { vec3 v; vec2 c; };
}
}
#include "MemoryLoggerOff.h"

#endif // VERTEXFORMAT_H_INCLUDED
