#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <string>
#include <exception>

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    class Exception : public std::exception
    {
    public:
        Exception(const std::string& s) { _msg = s; }
        virtual ~Exception() throw() {}

        virtual const char* what() const throw()
        {
            return _msg.c_str();
        }

    private:
        std::string _msg;
    };

    class BadDealloc : public Exception
    {
    public:
        BadDealloc(const std::string& s) : Exception(s){}
    };
}
}
#include "MemoryLoggerOff.h"

#endif // EXCEPTION_H_INCLUDED
