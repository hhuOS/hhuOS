#include <kernel/Cpu.h>
#include <lib/libc/printf.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include "kernel/threads/Scheduler.h"

extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
}

Scheduler* Scheduler::scheduler = nullptr;

Scheduler::Scheduler() : initialized(false) {

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

    Cpu::disableInterrupts();

    if (!isThreadWaiting()) {

        Cpu::enableInterrupts();

        return;
    }

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


