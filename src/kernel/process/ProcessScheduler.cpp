#include <lib/system/priority/AccessArrayPriorityPattern.h>
#include "ProcessScheduler.h"
#include "Process.h"
#include "lib/libc/sprintf.h"

extern "C" {
    extern void setSchedInit();
}

namespace Kernel {

ProcessScheduler::ProcessScheduler(PriorityPattern &priorityPattern) : priorityPattern(priorityPattern), readyQueues(priorityPattern.getPriorityCount()) {

}

ProcessScheduler& ProcessScheduler::getInstance() noexcept {
    static AccessArrayPriorityPattern priority(5);

    static ProcessScheduler instance(priority);

    return instance;
}

void ProcessScheduler::startUp() {
    lock.acquire();

    if (!isProcessWaiting()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No process is waiting to be scheduled!");
    }

    currentProcess = &getNextProcess();

    initialized = true;

    setSchedInit();
}

void ProcessScheduler::ready(Process &process) {
    lock.acquire();

    readyQueues[process.getPriority()].push(&process);

    lock.release();
}

void ProcessScheduler::exit() {
    lock.acquire();

    if (!initialized) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                            "Scheduler: 'exit' called but scheduler is not initialized!");
    }

    if (!isProcessWaiting()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No process is waiting to be scheduled!");
    }

    dispatch(getNextProcess());
}

void ProcessScheduler::kill(Process &process) {
    lock.acquire();

    if (!initialized) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                            "Scheduler: 'kill' called but scheduler is not initialized!");
    }

    if (process == getCurrentProcess()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                            "Scheduler: A process is trying to kill itself... Use 'exit' instead!");
    }

    readyQueues[process.getPriority()].remove(&process);

    lock.release();
}

void ProcessScheduler::yield() {
    if (!isProcessWaiting()) {
        return;
    }

    if (lock.tryAcquire()) {
        readyQueues[currentProcess->getPriority()].push(currentProcess);
        dispatch(getNextProcess());
    }
}

void ProcessScheduler::dispatch(Process &next) {
    if (!initialized) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE,
                            "Scheduler: 'dispatch' called but scheduler is not initialized!");
    }

    /*Thread *current = currentThread;

    currentThread = &next;

    switchContext(&current->context, &next.context);*/
}

Process& ProcessScheduler::getNextProcess() {
    if (!isProcessWaiting()) {
        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE, "Scheduler: No thread is waiting to be scheduled!");
    }

    Util::ArrayBlockingQueue<Process*> *currentQueue = &readyQueues[priorityPattern.getNextPriority()];

    while (currentQueue->isEmpty()) {
        currentQueue = &readyQueues[priorityPattern.getNextPriority()];
    }

    Process *next = currentQueue->pop();

    currentQueue->push(next);

    return *next;
}

bool ProcessScheduler::isInitialized() {
    return initialized;
}

bool ProcessScheduler::isProcessWaiting() {
    for (const auto &queue : readyQueues) {
        if (!queue.isEmpty()) {
            return true;
        }
    }

    return false;
}

uint32_t ProcessScheduler::getProcessCount() {

    uint32_t count = 0;

    for (const auto &queue : readyQueues) {
        count += queue.size();
    }

    return count;
}

uint8_t ProcessScheduler::getMaxPriority() {
    return static_cast<uint8_t>(readyQueues.length() - 1);
}

uint8_t ProcessScheduler::changePriority(Process &process, uint8_t priority) {

    priority = static_cast<uint8_t>((priority > getMaxPriority()) ? (getMaxPriority()) : priority);

    lock.acquire();

    if(process == getCurrentProcess()) {
        lock.release();

        return priority;
    }

    readyQueues[process.getPriority()].remove(&process);

    readyQueues[priority].push(&process);

    lock.release();

    return priority;
}

Process& ProcessScheduler::getCurrentProcess() {
    return *currentProcess;
}

}