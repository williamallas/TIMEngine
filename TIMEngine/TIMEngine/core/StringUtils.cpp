
#include "StringUtils.h"
#include <locale>

using namespace tim::core;
#include "MemoryLoggerOn.h"

StringUtils::StringUtils() { }

int StringUtils::toInt() const { int i=0; std::stringstream(_str)>>i; return i; }
float StringUtils::toFloat() const { float i=0; std::stringstream(_str)>>i; return i; }
double StringUtils::toDouble() const { double i=0; std::stringstream(_str)>>i; return i; }
const std::string& StringUtils::str() const {  return _str; }

StringUtils::operator const std::string&() { return _str; }

StringUtils & StringUtils::toLower()
{
    for(size_t i=0; i < _str.size(); i++)
        _str[i]=tolower(_str[i]);
    return *this;
}

StringUtils & StringUtils::toUpper()
{
    for(size_t i=0; i < _str.size(); i++)
        _str[i]=toupper(_str[i]);
    return *this;
}
