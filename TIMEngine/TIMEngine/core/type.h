#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#include <boost/container/map.hpp>
#include <boost/container/set.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/list.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/thread.hpp>
#include <boost/timer/timer.hpp>
#include <queue>

namespace tim
{
namespace core
{
    typedef char byte; // 1 byte (signed)
    typedef unsigned char ubyte; // 1 byte (unsigned)
    typedef int integer; // 4 byte (signed)
    typedef unsigned int uint; // 4 byte (unsigned)
    typedef float real; // 4 byte ( IEEE 754 )
}
}

#endif // TYPE_H_INCLUDED
