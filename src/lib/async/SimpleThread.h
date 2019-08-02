#ifndef HHUOS_SIMPLETHREAD_H
#define HHUOS_SIMPLETHREAD_H

#include "kernel/thread/KernelThread.h"

class SimpleThread : public Kernel::KernelThread {

public:

    explicit SimpleThread(void (*work)()) noexcept;

    void run() override;

private:

    void (*work)();
};

#endif
