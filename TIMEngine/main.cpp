#include "core/core.h"
#include "core/MemoryLoggerOn.h"

using namespace tim;

int main(int argc, char ** argv)
{
    tim::core::init();

    new int;

    tim::core::quit();
    return 0;
}

