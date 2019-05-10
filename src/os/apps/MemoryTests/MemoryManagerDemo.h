#ifndef HHUOS_MEMORYMANAGERDEMO_H
#define HHUOS_MEMORYMANAGERDEMO_H


#include <kernel/threads/Thread.h>

class MemoryManagerDemo : public Thread {

public:

    MemoryManagerDemo() = default;

    MemoryManagerDemo(const MemoryManagerDemo &copy) = delete;

    MemoryManagerDemo& operator=(const MemoryManagerDemo &copy) = delete;

    ~MemoryManagerDemo() override = default;

    void run() override;

};


#endif
