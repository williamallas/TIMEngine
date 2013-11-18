#include "Math.h"
#include "MemoryLoggerOn.h"

namespace tim
{
namespace core
{

float toRad(float deg) { return deg*PI/180.0; }
float toDeg(float rad) { return rad*180.0/PI; }

}
}
