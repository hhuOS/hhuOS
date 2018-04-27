#include <kernel/Cpu.h>
#include <lib/libc/printf.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include "kernel/threads/Scheduler.h"

extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void schedulerYield();
    void allowPreemption();
}

InputService *inputService = nullptr;
TimeService *timeService = nullptr;

void allowPreemption() {
    if(inputService->getKeyboard()->checkForData()) {
        inputService->getKeyboard()->trigger();
    }

    if(timeService->getRTC()->checkForData()) {
        timeService->getRTC()->trigger();
    }

    schedulerLock.release();
}

Scheduler* Scheduler::scheduler = nullptr;

void schedulerYield() {

    Scheduler::getInstance()->yield();
}

Scheduler::Scheduler() : initialized(false) {
    inputService = Kernel::getService<InputService>();
    timeService = Kernel::getService<TimeService>();
}

Scheduler *Scheduler::getInstance()  {

    if(scheduler == nullptr) {

        scheduler = new Scheduler();
    }

    return scheduler;
}

void Scheduler::schedule() {

    Thread* first;

    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();

    first = readyQueue.pop();

    currentThread = first;

    initialized = true;

    setSchedInit();

    startThread(currentThread->context);
}

void Scheduler::ready(Thread& that) {

    Cpu::disableInterrupts();

    readyQueue.push(&that);

    Cpu::enableInterrupts();
}

void Scheduler::exit() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();
    
    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* next = readyQueue.pop();
    
    dispatch (*next);
}

void Scheduler::kill(Thread& that) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();

    readyQueue.remove(&that);

    Cpu::enableInterrupts();
}

void Scheduler::yield() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    //schedulerLock.acquire();


    if (!isThreadWaiting()) {

        //schedulerLock.release();

        return;
    }

    Cpu::disableInterrupts();

    Thread *next = readyQueue.pop();

    readyQueue.push(active());

    dispatch(*next);
}

void Scheduler::block() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* next;
    
    Cpu::disableInterrupts();
    
    if (!isThreadWaiting()) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }
    
    next = readyQueue.pop();
    
    dispatch (*next);
}

void Scheduler::deblock(Thread &that) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Cpu::disableInterrupts();

    readyQueue.push(&that);

    Cpu::enableInterrupts();
}

void Scheduler::dispatch(Thread &next) {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    Thread* current = currentThread;

    currentThread = &next;

    switchContext(&current->context, &next.context);
}

bool Scheduler::isInitialized() {

    return initialized;
}

bool Scheduler::isThreadWaiting() {

    return !readyQueue.isEmpty();
}


