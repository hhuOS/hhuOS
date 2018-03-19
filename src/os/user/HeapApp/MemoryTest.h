#ifndef __MemoryTest_include__
#define __MemoryTest_include__


#include <kernel/threads/Thread.h>
#include <kernel/Spinlock.h>

class MemoryTest : public Thread {

public:

    MemoryTest() : Thread("MemoryTest") {}

    MemoryTest(const MemoryTest &other) = delete;

    ~MemoryTest() = default;

    void run() override;

private:

    static Spinlock printLock;

    bool isRunning = true;

};


#endif
