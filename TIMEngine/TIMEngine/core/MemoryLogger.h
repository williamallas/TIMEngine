#ifndef MEMORYLOGGER_H_INCLUDED
#define MEMORYLOGGER_H_INCLUDED

#include <map>
#include <cstdlib>

#include "Type.h"
#include "Exception.h"

#include "MemoryLoggerOn.h"
namespace tim
{
namespace core
{
    class MemoryLogger
    {
    private:
        struct MemoryAlloc
        {
            intptr_t ptr;
            uint size, line;
            std::string file;
            bool isArray;
        };

    public:
        static MemoryLogger & instance();
        static void freeInstance();

        void* alloc(uint, uint, const std::string&, bool) throw(std::bad_alloc);
        void dealloc(void *, bool) throw(BadDealloc);
        void nextDealloc(uint, const std::string&);

        void printLeak() const;

    protected:
        MemoryLogger();
        virtual ~MemoryLogger();

    private:
        std::map<intptr_t, MemoryAlloc> _allocatedMemorys;
        uint _lastDeallocLine;
        std::string _lastDeallocFile;

        static MemoryLogger * _instance;

        #include "MemoryLoggerOff.h"
        MemoryLogger(const MemoryLogger&) = delete;
        MemoryLogger & operator=(const MemoryLogger&) = delete;
        #include "MemoryLoggerOn.h"
    };
}
}
#include "MemoryLoggerOff.h"

#ifdef TIM_DEBUG
void* operator new(size_t size, uint line, const std::string& file);
void* operator new[](size_t size, uint line, const std::string& file);
void operator delete(void * ptr) throw();
void operator delete[](void * ptr) throw();
#endif

#endif // MEMORYLOGGER_H_INCLUDED
