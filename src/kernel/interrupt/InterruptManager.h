#ifndef HHUOS_IODEVICEMANAGER_H
#define HHUOS_IODEVICEMANAGER_H

#include "kernel/thread/Thread.h"
#include "lib/async/Spinlock.h"
#include "lib/util/ArrayList.h"

class InterruptHandler;

class InterruptManager : public Thread {

private:

    Util::ArrayList<InterruptHandler*> interruptHandler;

    Spinlock lock;

public:

    InterruptManager();

    InterruptManager(const InterruptManager &copy) = delete;

    ~InterruptManager() override = default;

    static InterruptManager& getInstance() noexcept;

    void registerInterruptHandler(InterruptHandler *device);

    void deregisterInterruptHandler(InterruptHandler *device);

    void handleDisabledInterrupts();

    void run() override;

};


#endif
