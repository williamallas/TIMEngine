#ifndef STRINGUTILS_H_INCLUDED
#define STRINGUTILS_H_INCLUDED

#include <string>
#include <sstream>

#include "MemoryLogger.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    class StringUtils
    {
    public:
        StringUtils();

        template<class T>
        StringUtils(const T& val);

        int toInt() const;
        float toFloat() const;
        double toDouble() const;
        const std::string& str() const;

        StringUtils & toLower();
        StringUtils & toUpper();

        operator const std::string&();

    private:
        std::string _str;

    };

    template<class T>
    StringUtils::StringUtils(const T& val)
    {
        std::stringstream os;
        os << val;
        _str = os.str();
    }
}
}
#include "MemoryLoggerOff.h"

#endif // STRINGUTILS_H_INCLUDED
