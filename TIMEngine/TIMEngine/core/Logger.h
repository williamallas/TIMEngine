#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

#include <iostream>
#include <string>

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    template<class T>
    static void err(const T & s)
    {
        std::cerr << s;
    }

    template<class T>
    static void out(const T & s)
    {
        std::cout << s;
    }
}
}
#include "MemoryLoggerOff.h"

#endif // LOGGER_H_INCLUDED
