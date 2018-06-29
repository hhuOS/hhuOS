#include <kernel/Cpu.h>
#include <lib/libc/printf.h>
#include <kernel/interrupts/Pic.h>
#include <kernel/Kernel.h>
#include <kernel/services/SerialService.h>
#include <devices/Pit.h>
#include "kernel/threads/Scheduler.h"

extern "C" {
    void startThread(Context* first);
    void switchContext(Context **current, Context **next);
    void setSchedInit();
    void schedulerYield();
    void checkIoBuffers();
}

InputService *inputService = nullptr;
TimeService *timeService = nullptr;
SerialService *serialService = nullptr;

void checkIoBuffers() {
    if(inputService->getKeyboard()->checkForData()) {
        inputService->getKeyboard()->trigger();
    }

    if(timeService->getRTC()->checkForData()) {
        timeService->getRTC()->trigger();
    }

    if(serialService->isPortAvailable(Serial::COM1)) {
        if(serialService->getSerialPort(Serial::COM1)->checkForData()) {
            serialService->getSerialPort(Serial::COM1)->trigger();
        }
    }

    if(serialService->isPortAvailable(Serial::COM2)) {
        if(serialService->getSerialPort(Serial::COM2)->checkForData()) {
            serialService->getSerialPort(Serial::COM2)->trigger();
        }
    }

    if(serialService->isPortAvailable(Serial::COM3)) {
        if(serialService->getSerialPort(Serial::COM3)->checkForData()) {
            serialService->getSerialPort(Serial::COM3)->trigger();
        }
    }

    if(serialService->isPortAvailable(Serial::COM4)) {
        if(serialService->getSerialPort(Serial::COM4)->checkForData()) {
            serialService->getSerialPort(Serial::COM4)->trigger();
        }
    }
}

Scheduler* Scheduler::scheduler = nullptr;

void schedulerYield() {

    Scheduler::getInstance()->yield();
}

Scheduler::Scheduler() : initialized(false) {
    inputService = Kernel::getService<InputService>();
    timeService = Kernel::getService<TimeService>();
    serialService = Kernel::getService<SerialService>();
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

    Pit::getInstance()->setYieldable(this);

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

    if (!isThreadWaiting()) {

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

uint32_t Scheduler::getThreadCount() {
    return readyQueue.size();
}


