#include "core.h"
#include "MemoryLoggerOn.h"

namespace tim
{
namespace core
{

void init()
{
    std::freopen("err.txt", "w+", stderr);
}

void quit()
{
    MemoryLogger::freeInstance();
}

}
}
