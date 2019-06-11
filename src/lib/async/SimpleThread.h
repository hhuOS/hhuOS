#ifndef HHUOS_SIMPLETHREAD_H
#define HHUOS_SIMPLETHREAD_H

#include "kernel/thread/Thread.h"

class SimpleThread : public Kernel::Thread {

public:

    explicit SimpleThread(void (*work)()) noexcept;

    void run() override;

private:

    void (*work)();
};

#endif
