#ifndef HHUOS_SIMPLETHREAD_H
#define HHUOS_SIMPLETHREAD_H

#include "Thread.h"

class SimpleThread : public Thread {

public:

    explicit SimpleThread(void (*work)()) noexcept;

    void run() override;

private:

    void (*work)();
};

#endif
