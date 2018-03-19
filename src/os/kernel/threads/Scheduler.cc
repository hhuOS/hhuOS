/*****************************************************************************
 *                                                                           *
 *                          S C H E D U L E R                                *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung eines einfachen Zeitscheiben-Schedulers. *
 *                  Rechenbereite Threads werden in 'readQueue' verwaltet.   *
 *                  Threads koennen nun blockiert und deblockiert werden.    *
 *                  Dies ist fuer die Implementierung von Semaphoren         *
 *                  notwendig.                                               *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 5.1.2017                       *
 *****************************************************************************/

#include <kernel/Cpu.h>
#include <lib/libc/printf.h>
#include "kernel/threads/Scheduler.h"


extern "C" {
    void Thread_start(Context* first);
    void Thread_switch(Context* current, Context* next);
    void get_thread_vars(uint32_t **current, uint32_t **next);
    void switch_context(Context **current, Context **next);
    void setSchedInit();
}

Scheduler* Scheduler::scheduler = nullptr;

/*****************************************************************************
 * Methode:         Scheduler::Scheduler                                     *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Konstruktor des Schedulers. Registriert den              *
 *                  Leerlauf-Thread.                                         *
 *****************************************************************************/
Scheduler::Scheduler () {
    initialized = false;
    get_thread_vars(&currentRegs, &nextRegs);
}

    
/*****************************************************************************
 * Methode:         Scheduler::schedule                                      *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Scheduler starten. Wird nur einmalig aus main.cc gerufen.*
 *****************************************************************************/
void Scheduler::schedule () {
    Thread* first;

    first = readyQueue.pop();

    if (first) {
        currentThread = first;
        initialized = true;
        setSchedInit();
        Thread_start(currentThread->context);
    } else {
        printf("[PANIC] Schedule is empty!\n");
    }
}


/*****************************************************************************
 * Methode:         Scheduler::ready                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread in Ready-Queue eintragen.                         *
 *                                                                           *
 * Parameter:                                                                *
 *      that        Einzutragender Thread                                    *
 *****************************************************************************/
void Scheduler::ready (Thread& that) {
    // Thread-Wechsel durch PIT verhindern
    Cpu::disableInterrupts();

    readyQueue.push(&that);

    // Thread-Wechsel durch PIT jetzt wieder erlauben
    Cpu::enableInterrupts();
}


/*****************************************************************************
 * Methode:         Scheduler::exit                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread ist fertig und terminiert sich selbst. Er traegt  *
 *                  sich hierzu aus der Ready-Queue aus und wird somit nicht *
 *                  mehr aktiviert.                                          *
 *****************************************************************************/
void Scheduler::exit () {
    // Thread-Wechsel durch PIT verhindern
    Cpu::disableInterrupts();

    // hole naechsten Thread aus ready-Liste.
    Thread* next = readyQueue.pop();

    if (next == nullptr) {
        printf("[PANIC] Schedule is empty!");
        Cpu::halt();
    }
    
    // next aktivieren
    dispatch (*next);

    // Interrupts werden in Thread_switch in Thread.asm wieder zugelassen
    // dispatch kehr nicht zurueck
}


/*****************************************************************************
 * Methode:         Scheduler::kill                                          *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread mit 'Gewalt' terminieren.                         *
 *                                                                           *
 * Parameter:                                                                *
 *      that        Zu terminierender Thread                                 *
 *****************************************************************************/
void Scheduler::kill (Thread& that) {
    // Thread-Wechsel durch PIT verhindern
    Cpu::disableInterrupts();

    readyQueue.remove (&that);
    
    // Thread-Wechsel durch PIT jetzt wieder erlauben
    Cpu::enableInterrupts();
}


/*****************************************************************************
 * Methode:         Scheduler::yield                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    CPU freiwillig abgeben und Auswahl des naechsten Threads.*
 *****************************************************************************/
void Scheduler::yield () {

    Cpu::disableInterrupts();

    if ( isThreadWaiting() == false) {
        return ;
    }

    Thread *next = readyQueue.pop();

    readyQueue.push(active());

    dispatch(*next);
}


/*****************************************************************************
 * Methode:         Scheduler::block                                         *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Aufrufer ist blockiert. Es soll auf den naechsten Thread *
 *                  umgeschaltet werden. Der Aufrufer soll nicht in die      *
 *                  readyQueue eingefuegt werden.                            *
 *                  Die Methode kehrt nicht zurueck, sondern schaltet um.    *
 *****************************************************************************/
void Scheduler::block () {
    Thread* next;
    
    // Thread-Wechsel durch PIT verhindern
    Cpu::disableInterrupts();
    
    // kein anderer Thread rechenbreit?
    if ( isThreadWaiting() == false) {
        printf("Panic: all threads blocked - processor halted.");
        Cpu::halt ();
    }
    
    next = readyQueue.pop();
    
    dispatch (*next);
    
    // Interrupts werden in Thread_switch in Thread.asm wieder zugelassen
    // dispatch kehr nicht zurueck
}


/*****************************************************************************
 * Methode:         Scheduler::deblock                                       *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Thread 'that' deblockieren. 'that' wird nur in die       *
 *                  readyQueue eingefuegt und dann zurueckgekehrt.           *
 *                                                                           *
 * Parameter:       that:  Thread der deblockiert werden soll.               *
 *****************************************************************************/
void Scheduler::deblock (Thread &that) {
    // Thread-Wechsel durch PIT verhindern
    Cpu::disableInterrupts();

    readyQueue.push( &that );

    // Thread-Wechsel durch PIT jetzt wieder erlauben
    Cpu::enableInterrupts();
}


/*****************************************************************************
 * Methode:         Scheduler::prepare_preemption                            *
 *---------------------------------------------------------------------------*
 * Beschreibung:    CPU soll aktuellem Thread entzogen werden. Wird nur      *
 *                  aus dem Zeitgeber-Interrupt-Handler aufgerufen. Daher    *
 *                  muss nicht gegenueber Interrupts synchronisiert werden.  *
 *                                                                           *
 * Rueckgabewert:   true:   Var. fuer Thread-Wechsel in startup.asm gesetzt  *
 *                  false:  kein anderer Thread will rechnen. Nichts tun.    *
 *****************************************************************************/
bool Scheduler::preparePreemption() {
    
    // kein anderer Thread rechenbreit?
    // sollte nier auftreten, der Leerlauf-Thread sollte immer bereit sein
    if ( isThreadWaiting() == false)
        return false;
    
    Thread* act  = active();
    Thread* next = readyQueue.pop();

    readyQueue.push(act);

    *currentRegs = (unsigned int)&(currentThread->context);
    *nextRegs = (unsigned int)&(next->context);

    currentThread = next;
    
    return true;
}

void Scheduler::dispatch(Thread &next) {
    Thread* current = currentThread;
    currentThread = &next;
    switch_context(&current->context, &next.context);
}


