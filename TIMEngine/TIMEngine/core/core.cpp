#include "core.h"
#include "scene/scene.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{

void init()
{
    std::freopen("err.txt", "w+", stderr);
    Rand::seed(time(0));
}

void quit()
{
    MemoryLogger::instance().printLeak();
    free(&MemoryLogger::instance());
}

}
}
