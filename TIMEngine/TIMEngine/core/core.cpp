#include "core.h"
#include "MemoryLoggerOn.h"

void tim::core::init()
{
    std::freopen("err.txt", "w+", stderr);
}

void tim::core::quit()
{
    MemoryLogger::freeInstance();
}

