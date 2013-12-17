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
    static std::ostream& err(const T & s)
    {
        std::cerr << s;
        return std::cerr;
    }

    template<class T>
    static std::ostream& out(const T & s)
    {
        std::cout << s;
        return std::cout;
    }
}
}
#include "MemoryLoggerOff.h"

#endif // LOGGER_H_INCLUDED
