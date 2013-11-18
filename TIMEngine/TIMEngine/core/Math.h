#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED

#include <cmath>

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{

const float PI = 3.141592653589793238462643383279502884197169399375105820974944;

float toRad(float deg);
float toDeg(float rad);

}
}
#include "MemoryLoggerOff.h"

#endif // MATH_H_INCLUDED
