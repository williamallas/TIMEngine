#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include <cstdio>
#include <mutex>
#include "ThreadPool.h"
#include "Exception.h"
#include "Option.h"
#include "Rand.h"
#include "StringUtils.h"
#include "Logger.h"
#include "MemoryLogger.h"
#include "Vector.h"
#include "Matrix.h"
#include "type.h"
#include "Plan.h"
#include "Sphere.h"
#include "Box.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    void init();
    void quit();
}
}

#include "MemoryLoggerOff.h"

#endif // CORE_H_INCLUDED
