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
        friend std::ostream& operator<< (std::ostream& stream, const StringUtils& t) { stream << t.str(); return stream;}

        StringUtils();

        template<class T>
        StringUtils(const T& val);

        int toInt() const;
        float toFloat() const;
        double toDouble() const;
        std::string& str();

        StringUtils& toLower();
        StringUtils& toUpper();

        std::string str() const;
        operator std::string() const;

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

#include <locale>

using namespace tim::core;

inline StringUtils::StringUtils() { }

inline int StringUtils::toInt() const { int i=0; std::stringstream(_str)>>i; return i; }
inline float StringUtils::toFloat() const { float i=0; std::stringstream(_str)>>i; return i; }
inline double StringUtils::toDouble() const { double i=0; std::stringstream(_str)>>i; return i; }
inline std::string& StringUtils::str() {  return _str; }
inline std::string StringUtils::str() const { return _str; }
inline StringUtils::operator std::string() const { return _str; }

inline StringUtils & StringUtils::toLower()
{
    for(size_t i=0; i < _str.size(); i++)
        _str[i]=tolower(_str[i]);
    return *this;
}

inline StringUtils & StringUtils::toUpper()
{
    for(size_t i=0; i < _str.size(); i++)
        _str[i]=toupper(_str[i]);
    return *this;
}

#include "MemoryLoggerOff.h"

#endif // STRINGUTILS_H_INCLUDED
