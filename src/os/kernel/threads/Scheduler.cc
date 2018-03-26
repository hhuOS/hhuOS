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

const IOport ctrl_port(0x64);    // Status- (R) u. Steuerregister (W)
const IOport data_port(0x60);    // Ausgabe- (R) u. Eingabepuffer (W)

InputService *inputService;

void allowPreemption() {
    schedulerLock.unlock();

    // Check if the keyboard has data to get.
    auto control = ctrl_port.inb();

    if(control & 0x1) {
        inputService->getKeyboard()->trigger();
    }
}

Scheduler* Scheduler::scheduler = nullptr;

void schedulerYield() {

    Scheduler::getInstance()->yield();
}

Scheduler::Scheduler() : initialized(false) {
    inputService = Kernel::getService<InputService>();
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

    schedulerLock.lock();

    readyQueue.push(&that);

    schedulerLock.unlock();
}

void Scheduler::exit() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    schedulerLock.lock();
    
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

    schedulerLock.lock();

    readyQueue.remove(&that);

    schedulerLock.unlock();
}

void Scheduler::yield() {

    if (!initialized) {

        Cpu::throwException(Cpu::Exception::ILLEGAL_STATE);
    }

    schedulerLock.lock();

    if (!isThreadWaiting()) {

        schedulerLock.unlock();

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
    
    schedulerLock.lock();
    
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

    schedulerLock.lock();

    readyQueue.push( &that );

    schedulerLock.unlock();
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


