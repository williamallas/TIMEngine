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
        V=0,
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
        QUADS,
    };

    struct VN_Vertex
    {
        vec3 v, n;
        bool operator<(const VN_Vertex& in) const
        {
            if(v < in.v)
                return true;
            else if(v != in.v)
                return false;

            if(n < in.n)
                return true;
            else if(n != in.n)
                return false;

            return false;
        }
    };

    struct VNC_Vertex
    {
        vec3 v, n; vec2 c;
        bool operator<(const VNC_Vertex& in) const
        {
            if(v < in.v)
                return true;
            else if(v != in.v)
                return false;

            if(n < in.n)
                return true;
            else if(n != in.n)
                return false;

            if(c < in.c)
                return true;
            else if(c != in.c)
                return false;

            return false;
        }
    };

    struct VNCT_Vertex
    {
        vec3 v, n, t; vec2 c;
        bool operator<(const VNCT_Vertex& in) const
        {
            vec3 v, n; vec2 c; vec3 t;
            if(v < in.v)
                return true;
            else if(v != in.v)
                return false;

            if(n < in.n)
                return true;
            else if(n != in.n)
                return false;

            if(c < in.c)
                return true;
            else if(c != in.c)
                return false;

            if(t < in.t)
                return true;
            else if(t != in.t)
                return false;

            return false;
        }
    };

    struct VC_Vertex
    {
        vec3 v; vec2 c;
        bool operator<(const VC_Vertex& in) const
        {
            vec3 v; vec2 c;
            if(v < in.v)
                return true;
            else if(v != in.v)
                return false;

            if(c < in.c)
                return true;
            else if(c != in.c)
                return false;

            return false;
        }
    };
}
}
#include "MemoryLoggerOff.h"

#endif // VERTEXFORMAT_H_INCLUDED
