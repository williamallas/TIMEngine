#ifndef TEST_H
#define TEST_H

#include <stdlib.h>

class Test
{
    public:
        Test();
        virtual ~Test();
        int getx() const;
        int gety() const { return _y; }
    protected:
        int _x=rand();
        int _y=rand();
    private:
};

#endif // TEST_H
