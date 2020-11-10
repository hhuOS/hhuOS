#ifndef HHUOS_PROCESSSCHEDULER_H
#define HHUOS_PROCESSSCHEDULER_H

#include <lib/system/priority/PriorityPattern.h>
#include <lib/util/ArrayBlockingQueue.h>
#include <lib/system/IdGenerator.h>
#include "Process.h"

namespace Kernel {

class ProcessScheduler {

public:

    explicit ProcessScheduler(PriorityPattern &priority);

    ProcessScheduler(const ProcessScheduler &copy) = delete;

    ProcessScheduler &operator=(const ProcessScheduler &copy) = delete;

    ~ProcessScheduler() = default;

    bool isInitialized();

    void startUp();

    void ready(Process &process);

    void exit();

    void kill(Process &process);

    bool isProcessWaiting();

    uint32_t getProcessCount();

    uint8_t changePriority(Process &process, uint8_t priority);

    uint8_t getMaxPriority();

    static ProcessScheduler &getInstance() noexcept;

    Spinlock lock;

    String getAllProcesses();

    uint32_t getLength();

private:

    Process& getNextProcess();

    Process& getCurrentProcess();

    void yield();

    void dispatch(Process &next);

private:

    bool initialized = false;

    Process *currentProcess = nullptr;

    PriorityPattern &priorityPattern;

    Util::Array<Util::ArrayBlockingQueue<Process*>> readyQueues;

};

}

#endif
