#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <cmath>
#include "type.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    const float PI = 3.141592653589793238462643383279502884197169399375105820974944;

    inline float toRad(float deg) { return deg*PI/180.0; }
    inline float toDeg(float rad) { return rad*180.0/PI; }
    inline uint uipow(uint x, uint p) { uint r=1; for(uint i=0 ; i<p ; i++)r*=x; return r; }

    inline float interpolate(float a, float b, float x)
    {
        return a*(1.f-x)+b*x;
    }

    inline float interpolateCos(float a, float b, float x)
    {
        return interpolate(a,b, (1.f-cos(x*PI))*0.5);
    }

    inline float interpolate2(float a, float b, float c, float d, float x, float y)
    {
        return interpolate(interpolate(a,b, x), interpolate(c,d, x), y);
    }

    inline float interpolateCos2(float a, float b, float c, float d, float x, float y)
    {
        return interpolateCos(interpolateCos(a,b, x), interpolateCos(c,d, x), y);
    }

    #define BUILD_MASK(x) 1<<x

}
}
#include "MemoryLoggerOff.h"

#endif // MATH_H_INCLUDED
