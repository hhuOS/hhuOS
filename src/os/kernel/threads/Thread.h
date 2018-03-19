/*****************************************************************************
 *                                                                           *
 *                                 T H R E A D                               *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * Beschreibung:    Implementierung eines kooperativen Thread-Konzepts.      *
 *                  Thread-Objekte werden vom Scheduler in einer verketteten *
 *                  Liste 'readylist' verwaltet.                             *
 *                                                                           *
 *                  Im Konstruktor wird der initialie Kontext des Threads    * 
 *                  eingerichtet. Mit 'start' wird ein Thread aktiviert.     *
 *                  Die CPU sollte mit 'yield' freiwillig abgegeben werden.  * 
 *                  Um bei einem Threadwechsel den Kontext sichern zu        *
 *                  koennen, enthaelt jedes Threadobjekt eine Struktur       *
 *                  ThreadState, in dem die Werte der nicht-fluechtigen      *
 *                  Register gesichert werden koennen.                       *
 *                                                                           *
 *                  Zusaetzlich zum vorhandenen freiwilligen Umschalten der  *
 *                  CPU mit 'Thread_switch' gibt es nun ein forciertes Um-   *
 *                  durch den Zeitgeber-Interrupt ausgeloest wird und in     *
 *                  Assembler in startup.asm implementiert ist. Fuer das     *
 *                  Zusammenspiel mit dem Scheduler ist die Methode          *
 *                  'prepare_preemption' in Scheduler.cc wichtig.            *
 *                                                                           *
 * Autor:           Michael, Schoettner, HHU, 1.1.2017                       *
 *****************************************************************************/

#ifndef __Thread_include__
#define __Thread_include__

#include "kernel/threads/ThreadState.h"

#include <cstdint>
#include <lib/String.h>

class Thread {

public:

    InterruptFrame *interruptFrame;

    Context *context;

    Thread();

    Thread(const char* name);

    virtual ~Thread();

    uint32_t getTid() const;

    const char *getName() const;

    virtual void start() final;

    // Methode des Threads, muss in Sub-Klasse implementiert werden
    virtual void run () = 0;

private:

    Thread(const Thread &copy); // Verhindere Kopieren

    uint32_t tid;       // Thread-ID (wird automatisch vergeben)

    char *name;

    class Stack {

    public:

        explicit Stack(uint32_t size);

        ~Stack();

        uint8_t *getStart();

    private:

        uint8_t *stack;

        uint32_t size;

    };

    Stack *stack;

    void init();

    /* 4KB Stack Size */
    static const uint32_t   STACK_SIZE_DEFAULT            = 4096;

};

#endif
