#ifndef RAND_H_INCLUDED
#define RAND_H_INCLUDED

#include <boost/random/taus88.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include "Vector.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{

    class Rand
    {
    public:

        Rand(int seed) { _generator.seed(seed); }
        float next_f() { return frand_range(_generator); }
        float next_i() { return _generator(); }
        void setSeed(int seed) { _generator.seed(seed); }

        /* Static */
        static float frand() { return frand_range(generator); }

        static float frand(const vec2& range)
        {
            boost::random::uniform_real_distribution<> dis(range.x(), range.y());
            return dis(generator);
        }

        static int rand() { return generator(); }

        static int rand(const ivec2& range)
        {
            boost::random::uniform_int_distribution<> dis(range.x(), range.y());
            return dis(generator);
        }

        static void seed(int seed) { generator.seed(seed); }

    private:
        boost::random::taus88 _generator;

        static boost::random::taus88 generator;
        static boost::random::uniform_01<> frand_range;
    };

}
}
#include "MemoryLoggerOff.h"

#endif // RAND_H_INCLUDED
